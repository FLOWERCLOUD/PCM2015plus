#ifndef _TRAJ_TO_AFFINE_MODEL_DISTANCE_H
#define _TRAJ_TO_AFFINE_MODEL_DISTANCE_H

#include "DeformaleRegistration.h"
#include "sample.h"

class Traj2AffModelDistFunc{

public:
	typedef double DistanceType;

	Traj2AffModelDistFunc(DistanceType l):lamda(l){}
	~Traj2AffModelDistFunc(){};

	DistanceType operator()(PCloudTraj& traj, PCloudAffModel& model)
	{
		ScalarType dis = 0.0;

		//1--common time 
		LifeSpan commonTime = traj.trajLifeSpan; //traj and model common life-span
		if (commonTime.start < model.modelLifeSpan.start) // start get large value
		{
			commonTime.start = model.modelLifeSpan.start;
		}

		if (commonTime.end > model.modelLifeSpan.end)//end get small value
		{
			commonTime.end = model.modelLifeSpan.end;
		}

		if(commonTime.end > commonTime.start)
		{
			//2-- produce a new trajectory from traj have  common time with Model
			PCloudTraj comTraj;
			intersectionTrajWithComTime(traj,commonTime,comTraj);

			//3--
			PCloudAffModel comModel;
			intersectionModelWithComTime(model,commonTime,comModel);//可以优化-其实并不需要截断模型长度-但是要对齐起始和结束帧

			// 4--
			return minTraj2ModelDis(comTraj,comModel,commonTime);


		}else // no  common time
		{
			return 1e5;
		}

	}

//--------------
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

	void intersectionModelWithComTime(PCloudAffModel & oriModel,LifeSpan & comTime,PCloudAffModel & newModel)
	{
		IndexType spans = 0;
		IndexType sPos = 0;   //start node index
		spans = comTime.end - comTime.start;
		sPos = comTime.start - oriModel.modelLifeSpan.start;
		newModel.modelLifeSpan = comTime;

		for (IndexType nodeId = 0; nodeId < spans; nodeId ++,sPos ++)
		{
			newModel.fNode.push_back(oriModel.fNode[sPos]);
			newModel.bNode.push_back(oriModel.bNode[sPos]);
		}

	}


	ScalarType minTraj2ModelDis(PCloudTraj& traj, PCloudAffModel& model,LifeSpan & comTime)
	{
		assert(traj.trajLifeSpan.start == comTime.start && traj.trajLifeSpan.end == comTime.end);
		assert(model.modelLifeSpan.start == comTime.start && model.modelLifeSpan.end == comTime.end);

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


		SampleSet& m_smpSet = SampleSet::get_instance();
		//get the traj's coordinate  of  each  node  
		for (IndexType nodeId = 0; nodeId < totFrame; nodeId ++,startFrame ++)
		{
			vtxIndex = traj.trajNode[nodeId];
			trajNode = m_smpSet[startFrame].vertices_matrix().col(vtxIndex);
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
			tempdis = ((tempTraj.row(0) - rowTrajCoordinate.row(0)).norm()) / totFrame;


			if(tempdis < dis)
			{
				dis = tempdis;
			}
		}
		return dis;
	}


	void calNewTrajUnderModel(IndexType nodeId,PCloudAffModel & model,PointType oriNodePos,MatrixXX & rowNewTraj)
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

		IndexType start = model.modelLifeSpan.start;
		IndexType end   = model.modelLifeSpan.end;
		IndexType totFrame = end - start ;

		//nodeId  ∈ [ 0-1-2--nodeId--(totFrame)]
		rowNewTraj.block(0, 3 * nodeId,1,3) = oriNodePos.transpose().block(0,0,1,3);

		//0  <= nodeId <=  totFrame 
		//(Front)---->[nodeId,nodeId + 1,---,totFrame] 2014-10-2
		//(Back)<-----[0,---,nodeId - 1,nodeId]

		IndexType fNum = 0;
		IndexType bNum = 0;
		PointType transPoint;

		Matrix34 transf;
		transf.setZero();
		
		Vec4 homoCoor;
		homoCoor.setOnes();

		if(nodeId > 0 && nodeId < totFrame)
		{
			fNum = totFrame - nodeId;
			bNum = nodeId;
			transPoint = oriNodePos;

			for (IndexType fId = 0; fId < fNum; fId ++)
			{
				transf = model.fNode[fId];

				homoCoor.block(0,0,3,1) = transPoint.block(0,0,3,1);

				transPoint = transf * homoCoor;

				rowNewTraj.block(0, 3 * (nodeId + fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
			}

			transPoint = oriNodePos;
			for(IndexType fId = bNum - 1 ; fId >= 0; fId --)
			{
				transf = model.fNode[fId];

				homoCoor.block(0,0,3,1) = transPoint.block(0,0,3,1);

				transPoint = transf * homoCoor;

				rowNewTraj.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

			}
		}else
		{
			if (0 == nodeId)//
			{
				transPoint = oriNodePos;
				for (IndexType fId = 0; fId < totFrame; fId ++)
				{
					transf = model.fNode[fId];

					homoCoor.block(0,0,3,1) = transPoint.block(0,0,3,1);

					transPoint = transf * homoCoor;

					rowNewTraj.block(0, 3 * (fId + 1),1,3) = transPoint.transpose().block(0,0,1,3);
				}
			}else// totFrame == nodeId 
			{			 
				transPoint = oriNodePos;
				for(IndexType fId = totFrame - 1; fId >= 0;fId --)
				{
					transf = model.bNode[fId];

					homoCoor.block(0,0,3,1) = transPoint.block(0,0,3,1);

					transPoint = transf * homoCoor;

					rowNewTraj.block(0, 3 * fId,1,3) = transPoint.transpose().block(0,0,1,3);

				}
			}
		}
	}

public:
	DistanceType lamda;

};


#endif