#include "trajectory_classifier.h"
#include "globals.h"
#include "basic_types.h"
#include "sample.h"
#include "vertex.h"
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include "traj2model_distance.h"

#include "traj2AffineModel_distance.h"
#include "ICP/J_linkage.h"
#include"ICP/T_linkage.h"
#include "ICP/J_linkage_II.h"
#include "J_linkage_Matlab.hpp"
#include"sample_properity.h"
#include "bfs_classifier.hpp"
#include <fstream>
#include <sstream>
#include "InOutputDefinition.h"



void TrajectoryClassifier::run()
{

	Logger << "Begin Clustering.\n";

	SampleSet& set = SampleSet::get_instance();

	if (set.empty())
	{
		Logger<< " End Clustering(Warning: Empty Set).\n";
		emit finish_compute();
		return;
	}


	//Step 1: compute rotate feature

	// calculate feature
 	DeformableRegistration nonrigid;
	//nonrigid.setNeigNum(neigborNum);
	Logger<<"Neighbor Number = "<<neigborNum<<endl;
 	//MatrixXX featureMat;
    //nonrigid.calculateTrajFrature(featureMat);

	// test life spans traj
  	vector<PCloudTraj> totalTraj;
	
	vector<IndexType> sampleCenterVtxId;
	
	totalTraj.clear();
	
	sampleCenterVtxId.clear();

	//calculate trajectories with either equal or unequal.

	if (isEqual)
	{
		
		Logger<<"Using Equal Length Traj"<<endl;
		
		nonrigid.calculateFixedLengthTrajWithTracingAlong(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes);//连续配准
		
		//nonrigid.calculateFixedLengthTraj(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes);//先采样再配准--间隔配准--always

	}else
	{
		Logger<<"Using Unequal Length Traj"<<endl;
		//nonrigid.calculateDownSmpLifeSpanTraj(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes,threshold,lifeT);
		nonrigid.calculateDownSmpLifeSpanTrajCenter(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes,threshold,lifeT); 
	}


	//calculate motion models
	IndexType modeNum = modelT * sampleCenterVtxId.size();
	vector<IndexType> labels;
	labels.resize(totalTraj.size());

    
	if (totalTraj.size() < 50)
	{
		Logger<<"Traj size is small!.\n";
		Logger<< " End Clustering.\n";
		emit finish_compute();
		return;
	}

	//rigid or affine motion model

	if (isRigid)
	{
		Logger<<"Using Rigid motion  model"<<endl;

		vector<PCloudModel> totalModel;
		
		totalModel.clear();

		nonrigid.sampleModel(totalTraj,totalModel,modeNum);


		J_LinkageAdapter_Matlab<PCloudTraj, PCloudModel, Traj2ModDistanceFunc>	algo_adaptor(totalTraj, totalModel, labels, Traj2ModDistanceFunc(threshold),perC);

		algo_adaptor.compute();
	
	}else
	{
		Logger<<"Using Affine motion  model"<<endl;

		vector<PCloudAffModel> totalModel;

		totalModel.clear();

		nonrigid.sampleAffineModel(totalTraj,totalModel,modeNum);

		J_LinkageAdapter_Matlab<PCloudTraj, PCloudAffModel, Traj2AffModelDistFunc>	algo_adaptor(totalTraj, totalModel, labels, Traj2AffModelDistFunc(threshold),perC);

		algo_adaptor.compute();
	}


  
	Logger<<"centerFrame = "<<centerFrame<<endl;

	LOCK( set[centerFrame] );

	Sample& sample0 = set[centerFrame];

	vector<bool> isSelector(sample0.num_vertices(),false);

	//in order to visual sample vertexes 

	for (int i = 0; i < sampleCenterVtxId.size(); i++)
	{
		isSelector[sampleCenterVtxId[i]] = true;
	}

	if (isEqual)
	{
		#ifndef SAVE_CORRESPONDENCE
		///save labels and label-smooth
		IndexType smp_range[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
			51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
			101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,
			141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173};
		
		char corr_file_name[1024];









		sprintf(corr_file_name,"%s%s%.2d_%.2f.txt",OUTPUT_DIR2, OUTPUT_FILENAME2,centerFrame,perC);
		Logger<<"corr_file_name = "<<corr_file_name<<endl;
		FILE *in_correspond = fopen(corr_file_name,"w");

		for ( int i=0; i<sampleCenterVtxId.size();i++ )
		{
			for (int j=0; j<(sizeof(smp_range)/sizeof(IndexType));j++)
			{
				if(centerFrame==smp_range[j] || totalTraj[i].trajLifeSpan.start > smp_range[j] )continue;
				if(totalTraj[i].trajLifeSpan.end < smp_range[j])break;
				IndexType v = sampleCenterVtxId[i];
				
				fprintf(in_correspond,"%d %d %d %d\n", centerFrame, v, smp_range[j], totalTraj[i].trajNode[smp_range[j] - totalTraj[i].trajLifeSpan.start]);
			}
		}



		fclose(in_correspond);

#endif
    	
		bubleSort(sampleCenterVtxId,labels,labels.size());//
 		
		vector<IndexType> label_smooth(labels.size(),0);
 			//huayun nonrigid.smoothSampleLabel(sample0,sampleCenterVtxId,labels,label_smooth);//去除散乱集合点
 			//diff_using_bfs(labels,sampleCenterVtxId,centerFrame);//相同颜色不同连同块标记不同颜色
 		
		
		//smooth the segmentation results.
 		nonrigid.smoothSmapleLabel_KDTree(sample0,sampleCenterVtxId,labels,label_smooth);
		
		IndexType nLabels = 1;

		// in order to set the label system continuous and the 0 as the first label
  		nLabels = orderLabels(label_smooth);
 			
		Logger<<"seg size = "<<nLabels; 
		bool isPropagtateToOri =false; 		
		vector<IndexType> result_label(sample0.num_vertices(),0);
		if(isPropagtateToOri){
			// propagate the sample vertexes to original point cloud.
			nonrigid.propagateLabel2Orignal(sample0,sampleCenterVtxId,label_smooth,result_label); 
		}
		// 

        char label_labsmooth[1024];
		// 
		 #ifndef SAVE_CORRESPONDENCE
		 
			Logger<<OUTPUT_DIR1<<std::endl;
		   


			sprintf(label_labsmooth,"%s%s%.2d_%.2f.txt",OUTPUT_DIR1,OUTPUT_FILENAME1,centerFrame,perC);
			Logger<<"label_labsmooth = "<<label_labsmooth<<endl;
		   	FILE *in_label_smooth = fopen(label_labsmooth, "w");
		   	IndexType tpd = 0;		 
		 	fprintf(in_label_smooth,"%d\n",nLabels);		 
		   	for ( int i=0; i<set[centerFrame].num_vertices(); i++ )
		   	{
		   		if (isSelector[i])
		   		{ 
					if(isPropagtateToOri) fprintf( in_label_smooth, "%d %d %d\n", centerFrame, result_label[i], i );
		   			else fprintf( in_label_smooth, "%d %d %d\n", centerFrame, label_smooth[tpd]/*labels[tpd]label_smooth[tpd]*/, i );//需要对labels排序
		   			tpd++;
		   		} 
		   	}		   
		   	fclose(in_label_smooth); 		 
		 #endif
		//visualize the segmentation results in point cloud.
		#ifndef SAVE_CORRESPONDENCE
       			IndexType i = 0;
     				IndexType k = 0;
       			for (Sample::vtx_iterator v_iter = sample0.begin();
       				v_iter != sample0.end();
       				v_iter++,i++ )
       			{
     					if (isSelector[i])
     					{
     						(*v_iter)->set_visble(true);
     						//(*v_iter)->set_label( labels[k] );//orignal label
     						(*v_iter)->set_label( label_smooth[k] );//smooth label
     						k++;
     					}else
     					{
     						(*v_iter)->set_visble(false);
							
     					}
       			}
		#endif 

		if(isPropagtateToOri)
		{
			IndexType i = 0;
			for (Sample::vtx_iterator v_iter = sample0.begin();
				v_iter != sample0.end();
				v_iter++,i++ )
			{
				(*v_iter)->set_label(result_label[i] );
			}
			Logger<<"ori data vertex size = "<<sample0.num_vertices()<<endl;
		}
		
	}

	

//////////////////////////////////////////////////////////////////////////
// 可视化不等长轨迹2015/07/23(没有标签的点暂时同一标记某一颜色);涉及到的帧统一着色!
    
	//IndexType frames[] = {1,2,3,4,5,6,7,8,9,10/*,11,12,13,14,15,16,17,18,19,20*/};
    if ( !isEqual)
    {
		for ( IndexType i= (centerFrame - trajLen/2 ); i<= (centerFrame + trajLen/2 ); i++ )
		{
			for (IndexType j=0; j<set[i].num_vertices(); j++)
			{
				set[i][j].set_visble(false);
			}
		}

		//设置中心帧扭曲量大的点label为0
		IndexType i = 0;
		for (Sample::vtx_iterator v_iter = sample0.begin();
			v_iter != sample0.end();
			v_iter++,i++ )
		{
			if (isSelector[i])
			{
				(*v_iter)->set_visble(true);
				(*v_iter)->set_label( 0 );//smooth label
			}/*else
			 {
			 (*v_iter)->set_visble(false);
			 }*/
		}

		//可视化轨迹的label
		IndexType trajSize = totalTraj.size();

		for (IndexType tId = 0; tId < trajSize; tId ++)
		{
			IndexType nodeId = 0;
			for (IndexType stF = totalTraj[tId].trajLifeSpan.start; stF <= totalTraj[tId].trajLifeSpan.end; stF ++, nodeId ++)
			{
				set[stF][totalTraj[tId].trajNode[nodeId] ].set_visble(true);

				set[stF][totalTraj[tId].trajNode[nodeId] ].set_label(labels[tId]);
			}
		}
    }

	UNLOCK(set[centerFrame]);
 	Logger<< " End Clustering.\n";
	emit finish_compute();
}


void TrajectoryClassifier::derive_rotation_by_svd( VecX& rot, const MatrixX3& X, const MatrixX3& Y)
{


	Matrix33 sigma = (X.rowwise() - X.colwise().mean()).transpose() * (Y.rowwise() - Y.colwise().mean());
	Matrix33 rot_mat;

	Eigen::JacobiSVD<Matrix33> svd(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
	if(svd.matrixU().determinant()*svd.matrixV().determinant() < 0.0) {
		Vec3 S = Vec3::Ones(); S(2) = -1.0;
		 rot_mat = svd.matrixV()*S.asDiagonal()*svd.matrixU().transpose();
	} else {
		rot_mat = svd.matrixV()*svd.matrixU().transpose();
	}

	rot.block<3,1>(0,0) << rot_mat.row(0).transpose();
	rot.block<3,1>(3,0) << rot_mat.row(1).transpose();
	rot.block<3,1>(6,0) << rot_mat.row(2).transpose();
}
void TrajectoryClassifier::derive_rotation_by_svd(VecX& rot,const MatrixX3 &X, MatrixX3& Y,MatrixXXi& vtx_map)
{
	MatrixXX temp = Y;
	int verN = temp.rows();
	for (int i = 0; i < verN; i++)
	{
		Y.row(i) = temp.row(vtx_map(0,i));
	}

	Matrix33 sigma = (X.rowwise() - X.colwise().mean()).transpose() * (Y.rowwise() - Y.colwise().mean());
	Matrix33 rot_mat;

	Eigen::JacobiSVD<Matrix33> svd(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
	if(svd.matrixU().determinant()*svd.matrixV().determinant() < 0.0) {
		Vec3 S = Vec3::Ones(); S(2) = -1.0;
		rot_mat = svd.matrixV()*S.asDiagonal()*svd.matrixU().transpose();
	} else {
		rot_mat = svd.matrixV()*svd.matrixU().transpose();
	}

	rot.block<3,1>(0,0) << rot_mat.row(0).transpose();
	rot.block<3,1>(3,0) << rot_mat.row(1).transpose();
	rot.block<3,1>(6,0) << rot_mat.row(2).transpose();
}
void TrajectoryClassifier::bubleSort(vector<IndexType>& oriData,vector<IndexType>& labels,IndexType lSize)
{
    int temp;
	IndexType labelTemp = 0;
    bool flag=false;
	for (int i=0;i<lSize;i++)
    {
		 flag=true;
		 for (int j=0;j<lSize-i-1;j++)
         {
             if(oriData[j]>oriData[j+1])
             {
                 temp=oriData[j];
				 labelTemp = labels[j];

                 oriData[j]=oriData[j+1];
				 labels[j] = labels[j+1];

                 oriData[j+1]=temp;
				 labels[j+1] = labelTemp;

				 flag = false;
             }
        }
        if(flag) break;
     }
}

void TrajectoryClassifier::bubleSort(vector<IndexType>& oriData,vector<ScalarType>& labels,IndexType lSize)
{
	int temp;
	ScalarType labelTemp = 0;
	bool flag=false;
	for (int i=0;i<lSize;i++)
	{
		flag=true;
		for (int j=0;j<lSize-i-1;j++)
		{
			if(oriData[j]>oriData[j+1])
			{
				temp=oriData[j];
				labelTemp = labels[j];

				oriData[j]=oriData[j+1];
				labels[j] = labels[j+1];

				oriData[j+1]=temp;
				labels[j+1] = labelTemp;

				flag = false;
			}
		}
		if(flag) break;
	}
}
void TrajectoryClassifier::visDistor()
{
	Logger << "Begin Visual distortion.\n";

	SampleSet& set = SampleSet::get_instance();

	if (set.empty())
	{
		Logger<< " End Clustering(Warning: Empty Set).\n";
		emit finish_compute();
		return;
	}

 	DeformableRegistration nonrigid;
	vector<PCloudTraj> totalTraj;

	IndexType trajLen = 1;
	IndexType octreeRes = 32;
	vector<IndexType> sampleCenterVtxId;

	LOCK( set[centerFrame] );

	//nonrigid.calculateFixedLengthTraj(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes);//先采样再配准--间隔配准
	//nonrigid.produceDreamTraj(totalTraj,sampleCenterVtxId);
	nonrigid.calculateFixedLengthTrajWithTracingAlong(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes);//连续配准
	///20150122 -caculate vertex distortion
    vector<ScalarType> disVal;
	nonrigid.calculateVtxDistor(totalTraj,disVal,centerFrame);


// 	for (int i = 0; i < 20; i++)
// 	{
// 		Logger<<"distor"<<disVal[i]<<endl;
// 	}



	Sample& sample0 = set[centerFrame];
	vector<bool> isSelector(sample0.num_vertices(),false);

	bubleSort(sampleCenterVtxId,disVal,disVal.size());//ordered by vertex id


	///////////////////////////////////// in order to visual sample vertex
	for (int i = 0; i < sampleCenterVtxId.size(); i++)
	{
		isSelector[sampleCenterVtxId[i]] = true;
	}
	//  

	auto minId = std::minmax_element(disVal.begin(),disVal.end());
	ScalarType minV = *minId.first;
    ScalarType maxV = *minId.second;

	IndexType i = 0;
	IndexType k = 0;
	for (Sample::vtx_iterator v_iter = sample0.begin();
		v_iter != sample0.end();
		v_iter++,i++ )
	{
		if (isSelector[i])
		{
			(*v_iter)->set_visble(true);
			ScalarType ratio = (disVal[k] - minV) / maxV;
			//(*v_iter)->set_value( disVal[k] );//orignal label

			(*v_iter)->set_value( ratio );//orignal label
			k++;
		}else
		{
			(*v_iter)->set_visble(false);

		}

	}

	UNLOCK(set[centerFrame]);

	Logger<< " End VISUAL DISTORSION.\n";
	emit finish_compute();
}

void TrajectoryClassifier::diff_using_bfs( std:: vector<IndexType>& labels,std::vector<IndexType>& centerVtxId,IndexType centerFrame )
{
	SampleSet& set = SampleSet::get_instance();
	IndexType max_label= *max_element(labels.begin(),labels.end());
	vector< std::set<IndexType> > label_bucket(max_label+1);

	//IndexType centerFrame = 10;
	for ( int i=0; i<labels.size(); i++ )
	{
		label_bucket[labels[i]].insert( i );
	}
	IndexType new_label = max_label;
	Logger<<"max label before post:"<<new_label<<endl;
	for ( IndexType l=0; l<label_bucket.size(); l++ )
	{
		std::set<IndexType>& idx_set = label_bucket[l];
		if (idx_set.size()==0)
		{
			continue;
		}
		IndexType idx_set_size = idx_set.size();
		Matrix3X vtx_data;
		vtx_data.setZero( 3, idx_set_size );
		size_t i=0;
		for (std::set<IndexType>::iterator iter = idx_set.begin();
			iter != idx_set.end();
			iter++,i++)
		{
			IndexType a = *iter;
			vtx_data(0,i)  = set[centerFrame][centerVtxId[*iter]].x();
			vtx_data(1,i)  = set[centerFrame][centerVtxId[*iter]].y();
			vtx_data(2,i)  = set[centerFrame][centerVtxId[*iter]].z();
		}


#ifdef USE_RADIUS_NEAREST
		ScalarType rad = set[centerFrame].box_diag();
		BFSClassifier<ScalarType> classifier(vtx_data,rad);
#else
		ScalarType rad = set[centerFrame].box_diag()/20;
		BFSClassifier<ScalarType> classifier(vtx_data,rad,10);
#endif

		classifier.run();
		int *sep_label = classifier.get_class_label();
		i=0;
		for (std::set<IndexType>::iterator iter = idx_set.begin();
			iter != idx_set.end();
			iter++,i++)
		{
			if(sep_label[i]==0)continue;
			labels[*iter] = new_label + sep_label[i];
		}
		new_label += classifier.get_num_of_class()-1;


	}
	Logger<<"max label after post:"<<new_label<<endl;
}

//---------------------------------------------------------------------------------
int TrajectoryClassifier::orderLabels(vector<IndexType>& labels)
{
	if (labels.size() == 0)
	{
		Logger<<" label's vector is empty!.\n";
		return 0;
	}

	map<IndexType,IndexType> recordLabel;
	map<IndexType,IndexType>::iterator fIt;


	IndexType temp = 0;
    auto iter = labels.begin();

	recordLabel[(*iter)] = temp;

	for (;iter != labels.end(); iter ++)
	{
		fIt = recordLabel.find(*iter);
		if (fIt != recordLabel.end())
		{
			(*iter) = fIt->second;
		}else
		{
			temp++;
			recordLabel[(*iter)] = temp;
			(*iter) = temp;
		}
	}

	recordLabel.clear();

	return temp + 1;
}

void TrajectoryClassifier::setParamter(IndexType _trajLen,IndexType _octreeReso,ScalarType _perC,
									   ScalarType _thresHold,IndexType _modelT, IndexType _smallL,bool _isEqual, bool _isRigid)
{
	trajLen = _trajLen;
	octreeRes = _octreeReso;
	perC = _perC;
	threshold = _thresHold;
	modelT = _modelT;
	lifeT = _smallL;
	isEqual = _isEqual;
	isRigid = _isRigid;
	
}

void TrajectoryClassifier::setNeigNum(IndexType _neigbNum)
{
	neigborNum = _neigbNum;
}

TrajectoryClassifier::TrajectoryClassifier(IndexType cFrame)
{
	centerFrame = cFrame;
	trajLen = 2;   //1
	octreeRes = 32;
	perC = 0.6;
	threshold = 0.7;
	modelT = 1;
	lifeT = 2;
	isEqual = true;
	//isEqual = false;
	isRigid = false;
	neigborNum = 30;
}
