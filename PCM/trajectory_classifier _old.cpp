#include "trajectory_classifier.h"
#include "globals.h"
#include "basic_types.h"
#include "sample.h"
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include "traj2model_distance.h"
#include "ICP/J_linkage.h"

#include"ICP/T_linkage.h"
#include "ICP/J_linkage_II.h"
#include "J_linkage_Matlab.hpp"

#include <fstream>
#include <sstream>

void TrajectoryClassifier::run(){

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

 	//MatrixXX featureMat;
    //nonrigid.calculateTrajFrature(featureMat);

	// test life spans traj
  	vector<PCloudTraj> totalTraj;
     //nonrigid.calculateLifeSpansTraj(totalTraj,2);
 	////nonrigid.produceDreamTraj(totalTraj);


	////2014-11-1
	//IndexType centerFrame = 0;
	//IndexType trajLen = 1;
	//IndexType modeNum = 6500;
	//IndexType octreeRes = 64;
	//vector<IndexType> sampleCenterVtxId;
	////nonrigid.calculateFixedLengthTraj(totalTraj,5,5);
	////nonrigid.calculateFixedLengthTrajWithTracingAlong(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes);//连续配准
	//nonrigid.calculateFixedLengthTraj(totalTraj,centerFrame,sampleCenterVtxId,trajLen,octreeRes);//先采样再配准--间隔配准
	////nonrigid.calculateFixedLengthTrajBySampleTraj(totalTraj,centerFrame,trajLen,sampleCenterVtxId);//先配准再采样



	//2014-12-16
	//vector<PCloudModel> totalModel;
	//nonrigid.sampleModel(totalTraj,totalModel,modeNum);

	////nonrigid.sampleModelByNeig(totalTraj,totalModel,3000);

	//static distance--output.txt
	//nonrigid.staticDistance(totalTraj,totalModel);


	///J-linkage-clustering
	//ScalarType threshold = 0.012;//horse -
	//ScalarType threshold = 5.00;//horse -dancing girl
	//ScalarType threshold = 6.0; //scanned hand myself
	//ScalarType threshold = 2.3; //hand2
	//11-2
	//ScalarType threshold = .0280; // walk man--1105WalkMan
	//ScalarType threshold = 0.5280; // walk man--1105WalkMan-T-linkage
	//ScalarType threshold = 4.5; //pink
	//ScalarType threshold = 15.0;//lab  walk --手向上伸展，脚向前踢--1105human pose
	//ScalarType threshold = 2.95;//hand_dis
	//ScalarType threshold = 50.0;//background--

	//ScalarType threshold = 1.8;//hao li test
	//2014-12-11
	//ScalarType threshold = 0.03500;//animation  dance
	//ScalarType threshold = 0.0091100;//hip_hop man(0_9)

// 	vector<IndexType> labels;//12-25
// 	labels.resize(totalTraj.size());
// 	//J_LinkageAdapter_II<PCloudTraj, PCloudModel, Traj2ModDistanceFunc>	algo_adaptor(totalTraj, totalModel, labels, Traj2ModDistanceFunc(0.3));
// 	//J_LinkageAdapter<PCloudTraj, PCloudModel, Traj2ModDistanceFunc>	algo_adaptor(totalTraj, totalModel, labels, Traj2ModDistanceFunc(threshold));
// 	J_LinkageAdapter_Matlab<PCloudTraj, PCloudModel, Traj2ModDistanceFunc>	algo_adaptor(totalTraj, totalModel, labels, Traj2ModDistanceFunc(threshold));
// 	algo_adaptor.compute();


	//using t-linkage-2014-12-11
	//T_LinkageAdapter<PCloudTraj, PCloudModel, Traj2ModDistanceFunc>algo_adaptor(totalTraj, totalModel, labels, Traj2ModDistanceFunc(threshold));
	//algo_adaptor.compute();


	///
// 	Logger<<"octree resolution = "<<octreeRes<<endl;
// 	Logger<<"centerFrame = "<<centerFrame<<endl;
// 	Logger<<"Traj len = "<<trajLen<<endl;
// 	Logger<<"j_linkage's threshold  = "<<threshold<<endl;


	/// test sample 9-30
	///nonrigid.testingcalPmodel();
	///Logger<<"cluster end !.\n";





	///2014-10-21-text TPS using data:same radius_but_rotate//
	//nonrigid.testingcalPmodel();//变形后的坐标不对



	////2014-10-22 test kmeans
	//nonrigid.calculateTrajFeature(totalTraj,5,featureMat);

// 	using namespace cv;
 	//cv::Mat	cluster_data;
 	//cv::Mat labels;
 	//cv::Mat centers;
 	//cv::eigen2cv(featureMat, cluster_data );
 	//cv::kmeans( cluster_data, 2, labels,  
 	//	cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 100, 0.0001),
 	//	5, cv::KMEANS_PP_CENTERS, centers);



 	//Step 2: Begin Clustering, use opencv 's k-means clustering
//  	using namespace cv;
//  	Mat	cluster_data;
//  	Mat labels;
//  	Mat centers;
//  	cv::eigen2cv( rot_feature_mat, cluster_data );
//  	cv::kmeans( cluster_data, 10, labels,  
//  		TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 200, 0.00001),
//  		5, KMEANS_PP_CENTERS, centers);
 
 





 	//Step 3: Store the results

	////12-26-read file to visual 
    vector<IndexType> labels;
	vector<IndexType> label_smooth;
	vector<IndexType> sampleCenterVtxId;
	IndexType centerFrame;
	nonrigid.readSampleLabelFile("label_smp000_0.01200.txt",labels,label_smooth,sampleCenterVtxId,centerFrame); 
	////


  	LOCK( set[centerFrame] );
	Sample& sample0 = set[centerFrame];
	vector<bool> isSelector(sample0.num_vertices(),false);

	//bubleSort(sampleCenterVtxId,labels,labels.size());//ordered by vertex id
    //vector<IndexType> label_smooth(labels.size(),0);
	//nonrigid.smoothSampleLabel(sample0,sampleCenterVtxId,labels,label_smooth);


	//////////////////////////////////////////////////////////////////////////
	///save labels and label-smooth
	//IndexType smp_range[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	//char label_labsmooth[256];
	//sprintf(label_labsmooth,"label_smp%.3d_%.5f.txt",centerFrame,threshold);
	//FILE *in_label_smmoth = fopen(label_labsmooth, "w");
	//for (IndexType s_vtx = 0; s_vtx < labels.size(); s_vtx++)
	//{
	//	fprintf( in_label_smmoth, "%d %d %d %d\n", centerFrame, labels[s_vtx], label_smooth[s_vtx],sampleCenterVtxId[s_vtx]);
	//}
	//fclose(in_label_smmoth);

	/////////////




	for (int i = 0; i < sampleCenterVtxId.size(); i++)
	{
		isSelector[sampleCenterVtxId[i]] = true;
	}

  	IndexType i = 0;
	IndexType k = 0;
  	for (Sample::vtx_iterator v_iter = sample0.begin();
  		v_iter != sample0.end();
  		v_iter++,i++ )
  	{
		if (isSelector[i])
		{
			(*v_iter)->set_visble(true);
			(*v_iter)->set_label( labels[k] );//orignal label
			//(*v_iter)->set_label( label_smooth[k] );//smooth label
			k++;
		}else
		{
			(*v_iter)->set_visble(false);
			/*smp0[ i ].set_visble(false);*/
		}
  		//(*v_iter)->set_label( labels.at<IndexType>(i) );
		//(*v_iter)->set_label( labels[i]);
  	}


	///2014-12-5--visulization-to orignal point cloud

	//vector<IndexType> result_label(sample0.num_vertices(),0);
	//nonrigid.propagateLabel2Orignal(sample0,sampleCenterVtxId,label_smooth,result_label); 



   ////save file 2014-12-18
	///IndexType smp_range[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
// 	char label_file_name[256];
// 	char corr_file_name[256];
// 	sprintf(label_file_name,"labels%.3d_%.5f.txt",centerFrame,threshold);
// 	sprintf(corr_file_name,"corr%.3d_%.5f.txt",centerFrame,threshold);
// 	FILE *in_label = fopen(label_file_name, "w");
// 	FILE *in_correspond = fopen(corr_file_name,"w");
// 	vector<PCloudTraj> trajs;
// 	
// 	for ( int i=0; i<set[centerFrame].num_vertices(); i++ )
// 	{
// 		fprintf( in_label, "%d %d %d\n", centerFrame, result_label[i], i );
// 	}
// 
// 	for ( int i=0; i<sampleCenterVtxId.size();i++ )
// 	{
// 		for (int j=0; j<(sizeof(smp_range)/sizeof(IndexType));j++)
// 		{
// 			if(centerFrame==smp_range[j] || totalTraj[i].trajLifeSpan.start > smp_range[j] )continue;
// 			if(totalTraj[i].trajLifeSpan.end < smp_range[j])break;
// 			IndexType v = sampleCenterVtxId[i];
// 			fprintf(in_correspond,"%d %d %d %d\n", centerFrame, v, smp_range[j], totalTraj[i].trajNode[smp_range[j] - totalTraj[i].trajLifeSpan.start]);
// 		}
// 	}
// 
// 	fclose(in_label);
// 	fclose(in_correspond);


  ///save file 2014-12-18--[12-25]

// 	IndexType i = 0;
// 	for (Sample::vtx_iterator v_iter = sample0.begin();
//   	v_iter != sample0.end();
//   	v_iter++,i++ )
// 	{
// 		(*v_iter)->set_label( result_label[i] );
// 	}

	Logger<<"ori data vertex size = "<<sample0.num_vertices()<<endl;
	UNLOCK(set[centerFrame]);




//2014-10-27-
	//Visualize bool vertex----Mesh
	//Sample& smp0 = SampleSet::get_instance()[7];
	//int k=0;
	//for ( int i=0; i<smp0.num_vertices();i++ )
	//{
	//	if ( k<labels.size() && totalTraj[k].trajNode[5]== i )
	//	{
	//		smp0[ i ].set_visble(true);
	//		smp0[ i ].set_label( labels[k] );
	//		++k;
	//	}
	//	else
	//	{
	//		smp0[ i ].set_visble(false);
	//	}
	//}


// test non-rigid
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