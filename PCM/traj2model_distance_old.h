#ifndef _TRAJ_TO_MODEL_DISTANCE_H
#define _TRAJ_TO_MODEL_DISTANCE_H

#include "DeformaleRegistration.h"
#include "sample_set.h"

class Traj2ModDistanceFunc{

public:
	typedef double DistanceType;

	Traj2ModDistanceFunc(DistanceType l):lamda(l){}
	~Traj2ModDistanceFunc(){};


	DistanceType operator()( PCloudTraj& traj, PCloudModel& model )
	{
		ScalarType dis = 0.0;

		//1--common time 
		LifeSpan commonTime = traj.trajLifeSpan; //traj and model common life-span
		if (commonTime.start < model.modeLifeSpan.start) // start get large value
		{
			commonTime.start = model.modeLifeSpan.start;
		}

		if (commonTime.end > model.modeLifeSpan.end)//end get small value
		{
			commonTime.end = model.modeLifeSpan.end;
		}

		if(commonTime.end > commonTime.start)
		{
			//2-- produce a new trajectory from traj have  common time with Model
			PCloudTraj comTraj;
			intersectionTrajWithComTime(traj,commonTime,comTraj);

			//3--
			PCloudModel comModel;
			intersectionModelWithComTime(model,commonTime,comModel);//可以优化-其实并不需要截断模型长度-但是要对齐起始和结束帧

			// 4--
			return minTraj2ModelDis(comTraj,comModel,commonTime);
		}else // no  common time
		{
			return 1e5;
		}
	}

	void intersectionTrajWithComTime(PCloudTraj & oriTraj,LifeSpan & comTime,PCloudTraj & newTraj)
	{
		IndexType spans = 0;
		IndexType sPos = 0;   //start node index
		spans = comTime.end - comTime.start + 1;
		sPos = comTime.start - oriTraj.trajLifeSpan.start;
		newTraj.trajLifeSpan = comTime;
		//vector must resize before assign
		for (IndexType nodeId = 0; nodeId < spans; nodeId ++,sPos ++)
		{
			newTraj.trajNode.push_back(oriTraj.trajNode[sPos]);
		}
	}

	void intersectionModelWithComTime(PCloudModel & oriModel,LifeSpan & comTime,PCloudModel & newModel)
	{
		 IndexType spans = 0;
		 IndexType sPos = 0;   //start node index
		 spans = comTime.end - comTime.start;
		 sPos = comTime.start - oriModel.modeLifeSpan.start;
		 newModel.modeLifeSpan = comTime;
		 newModel.Centers.setZero(3,spans + 1);
		 for (IndexType nodeId = 0; nodeId < spans; nodeId ++,sPos ++)
		 {
			 newModel.fNode.push_back(oriModel.fNode[sPos]);
			 newModel.bNode.push_back(oriModel.bNode[sPos]);
		 }

		 sPos = comTime.start - oriModel.modeLifeSpan.start;
		 for (IndexType nodeId = 0; nodeId <= spans; nodeId ++,sPos ++)
		 {
			 newModel.Centers.col(nodeId) = oriModel.Centers.col(sPos);
		 }
	}

	ScalarType minTraj2ModelDis(PCloudTraj& traj, PCloudModel& model,LifeSpan & comTime)
	{
		assert(traj.trajLifeSpan.start == comTime.start && traj.trajLifeSpan.end == comTime.end);
		assert(model.modeLifeSpan.start == comTime.start && model.modeLifeSpan.end == comTime.end);

		//start end ={3,7}
		//node      ={21, 34, 87,45,76}
		//frame     =[3   4   5  6  7]
		ScalarType dis = 1e6;
		IndexType vtxIndex = 0;
		IndexType startFrame = comTime.start;// = 3
		IndexType totFrame = comTime.end - comTime.start + 1;// 7 - 3 + 1
		PointType trajNode;

		Matrix3X TrajCoordinate;
		TrajCoordinate.setZero(3,totFrame);

		MatrixXX rowTrajCoordinate; //(x1,y1,z1,---,xn,yn,zn) in order to calculate distances between two trajectories
		rowTrajCoordinate.setZero(1,3 * totFrame);

		//get the traj's coordinate  of  each  node
		for (IndexType nodeId = 0; nodeId < totFrame; nodeId ++,startFrame ++)
		{
			vtxIndex = traj.trajNode[nodeId];
			SampleSet& smpSet = SampleSet::get_instance();
			trajNode = smpSet[startFrame].vertices_matrix().col(vtxIndex);
			TrajCoordinate.col(nodeId) = trajNode;
			rowTrajCoordinate.block(0, 3 * nodeId,1,3) = trajNode.transpose().block(0,0,1,3);
		}

		// produce a  new traj under motion mode [model]
		ScalarType tempdis = 0.0;
		MatrixXX tempTraj;
		tempTraj.setZero(1,3 * totFrame);

		for (IndexType nodeId = 0; nodeId < totFrame; nodeId ++)
		{
			tempTraj.setZero();
			calNewTrajUnderModel(nodeId,model,TrajCoordinate.col(nodeId),tempTraj);
			tempdis = ((tempTraj.row(0) - rowTrajCoordinate.row(0)).norm() ) / totFrame;
			if(tempdis < dis)
			{
				dis = tempdis;
			}
		}
		return dis;
	}

	void calNewTrajUnderModel(IndexType nodeId,PCloudModel & model,PointType oriNodePos,MatrixXX & rowNewTraj)
	{
		 //aim to fill the row vector

		 // frame -- 2  3   4   5   6   7
		 //vtxId    45  78  92  39  53  16
		 //nodeId   0   1   [2] 3   4   5
		 //FRotate  0   1   2   3   4   
		 //BRotate      0   1   2   3   4  
		 //start = 2
		 //end   = 7
		 //totFrame = 5

		 IndexType start = model.modeLifeSpan.start;
		 IndexType end   = model.modeLifeSpan.end;
		 IndexType totFrame = end - start ;

		 //nodeId  ∈ [ 0-1-2--nodeId--(totFrame)]
		 rowNewTraj.block(0, 3 * nodeId,1,3) = oriNodePos.transpose().block(0,0,1,3);

		 //0  <= nodeId <=  totFrame 
		 //(Front)---->[nodeId,nodeId + 1,---,totFrame] 2014-10-2
		 //(Back)<-----[0,---,nodeId - 1,nodeId]

		 IndexType fNum = 0;
		 IndexType bNum = 0;
		 PointType transPoint;
		 Eigen::Vector3f x_mean,y_mean;
		 Matrix33 rotmate;
		 rotmate.setIdentity();
		 x_mean.setZero();
		 y_mean.setZero();

		 if(nodeId > 0 && nodeId < totFrame)
		 {
			 fNum = totFrame - nodeId;
			 bNum = nodeId;
			 transPoint = oriNodePos;

			 for (IndexType fId = 0; fId < fNum; fId ++)
			 {
				 rotmate = model.fNode[nodeId + fId];
				 x_mean = model.Centers.col(nodeId + fId);
				 y_mean = model.Centers.col(nodeId + fId + 1);

				 transPoint = rotmate * (transPoint - x_mean) + y_mean;
				 rowNewTraj.block(0, 3 * (nodeId + fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
			 }

			 transPoint = oriNodePos;
			 for(IndexType fId = bNum - 1 ; fId >= 0; fId --)
			 {
				 rotmate = model.bNode[fId];
				 x_mean  = model.Centers.col(fId + 1);
				 y_mean  = model.Centers.col(fId);

				 transPoint = rotmate * (transPoint - x_mean) + y_mean;
				 rowNewTraj.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

			 }
		 }else
		 {
			 if (0 == nodeId)//
			 {
				 transPoint = oriNodePos;
				 for (IndexType fId = 0; fId < totFrame; fId ++)
				 {
					 rotmate = model.fNode[fId];
					 x_mean = model.Centers.col(fId);
					 y_mean = model.Centers.col(fId + 1);

					 transPoint = rotmate * (transPoint - x_mean) + y_mean;
					 rowNewTraj.block(0, 3 * (fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
				 }
			 }else// totFrame == nodeId 
			 {			 
				 transPoint = oriNodePos;
				 for(IndexType fId = totFrame - 1; fId >= 0;fId --)
				 {
					 rotmate = model.bNode[fId];
					 x_mean = model.Centers.col(fId + 1);
					 y_mean = model.Centers.col(fId);

					 transPoint = rotmate * (transPoint - x_mean) + y_mean; 
					 rowNewTraj.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

				 }
			 }
		 }
	}

//private:
public:
	DistanceType lamda;


};



#endif