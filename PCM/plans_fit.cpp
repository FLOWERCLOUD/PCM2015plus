#include "plans_fit.h"
#include "J_linkage_Matlab.hpp"
#include"sample_properity.h"
#include <fstream>
#include <sstream>
#include "vertex.h"
#include "sample_set.h"


 void PlanClassifier::run()
 {
 	Logger << "Begin Fit Plans.\n";
 
 	//SampleSet& smp_set = SampleSet::get_instance();
 
 	if (m_smpSet.empty())
 	{
 		Logger<< " End Clustering(Warning: Empty Set).\n";
 		emit finish_compute();
 		return;
 	}
 
	//usingPCL();


 	vector<PointType> smpPoints;
 	vector<PlanModel> smpPlans; 
 	vector<IndexType> vtx_Map;
 	 
 	Sample& oriFrame = m_smpSet[m_activeFrame];
 	Sample smpFrame ;
 	SampleManipulation::SubSampleSolution solv( oriFrame, smpFrame,32);
 	vtx_Map = solv.compute();
 
     
 	for (IndexType i = 0; i < smpFrame.num_vertices(); i ++)
 	{
 		PointType temp = smpFrame.vertices_matrix().col(i);
 		smpPoints.push_back(temp);
 	}
 	samplePlanModels(smpFrame,smpPlans,0.5);
 
 	vector<IndexType> labels;
 	labels.resize(smpFrame.num_vertices(),0);
 	J_LinkageAdapter_Matlab<PointType,PlanModel,Point2PlanDisFunc> algo_adaptor(smpPoints,smpPlans,labels,Point2PlanDisFunc(0.5),0.25/*m_tradeOff*/);
 	algo_adaptor.compute();
 


 	bubleSort(vtx_Map,labels,labels.size());
 	vector<bool> isSelector;
 	isSelector.resize(oriFrame.num_vertices());
 
 	for (int i = 0; i < vtx_Map.size(); i++)
 	{
 		isSelector[vtx_Map[i]] = true;
 	}
     
 	//visualization  all plans

 	//IndexType i = 0;
 	//IndexType k = 0;
 	//for (Sample::vtx_iterator v_iter = oriFrame.begin();
 	//	v_iter != oriFrame.end();
 	//	v_iter++,i++ )
 	//{
 	//	if (isSelector[i])
 	//	{
 	//		(*v_iter)->set_visble(true);
 	//		(*v_iter)->set_label( labels[k] );//smooth label
 	//		k++;
 	//	}else
 	//	{
 	//		(*v_iter)->set_visble(false);
 	//	}
 	//}
    
	
	detectiveFloor(vtx_Map,labels);//get floor  plan  and visualization

	vector<bool> inliers;

    ransacPlan(inliers);//calculate the floor's center  and normal

	eliminateFloorAllFrame();
	//对剩下的点云数据利用ＰＣＬ库进行分割处理



	//////////////////////////////////////////////////////////////////////////
	for (Sample::vtx_iterator v_iter = oriFrame.begin();//visualization
		v_iter != oriFrame.end();
		v_iter++)
	{
		(*v_iter)->set_visble(false);
	}


	IndexType ii = 0;
	IndexType kk = 0;
	for (Sample::vtx_iterator v_iter = oriFrame.begin();
		v_iter != oriFrame.end() && ii < m_floorId.size();
		v_iter++,kk++ )
	{
		if (kk == m_floorId[ii])
		{	
            (*v_iter)->set_visble(true);
			if(inliers[ii])
			{

			   (*v_iter)->set_label(0) ;//inliers
			}else
			{
				(*v_iter)->set_label(1) ;
			}
			ii++;
		}
	}

    
 	Logger << "End Fit Plans.\n";
 }
//------------------
 void PlanClassifier::samplePlanModels(Sample& sFrame,vector<PlanModel>& planModels,ScalarType ration)
 {
 	IndexType pSize = sFrame.num_vertices();
 	assert(pSize > 3);
 
 	IndexType smpSize = pSize * ration;
 	while (smpSize-->0)
 	{
 		Matrix3X pCoor;
 		while (true)
 		{
 			pCoor.setZero(3,3);
 			if (sample3Points(pCoor,sFrame))
 			{
 				break;
 			}
 		}
 		PlanModel plan;
 		fitPlan(pCoor,plan);
 		planModels.push_back(plan);
 	}
 
 }
//-------------------------
 bool PlanClassifier::sample3Points(Matrix3X&pCoor,Sample&sframe)
 {
 	IndexType iter = 3;
 	while ( iter -- > 0)
 	{
 		IndexType pId = rand()%sframe.num_vertices();
 		pCoor.col(iter) = sframe.vertices_matrix().col(pId);
 	}
 
   return true;
 }
 void PlanClassifier::fitPlan(Matrix3X&pCoor,PlanModel& plan)
 {
 	plan.pCoor =  pCoor.rowwise().mean();
 
 	for (IndexType i = 0; i<pCoor.cols();i++)
 	{
 		pCoor.col(i) -= plan.pCoor;
 	}
 
 	Matrix33 sigma = pCoor * pCoor.transpose();
 
 	Eigen::EigenSolver<Matrix33> eigen_solver(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
 
 	auto ev = eigen_solver.eigenvectors();
 	auto eval = eigen_solver.eigenvalues();
 	ScalarType tmp[3] = { eval(0).real(),eval(1).real(),eval(2).real() };
 	IndexType min_idx = std::min_element(tmp,tmp+3) - tmp;
 	NormalType pNorm;
 
 	pNorm(0) = (ev.col(min_idx))(0).real();
 	pNorm(1) = (ev.col(min_idx))(1).real();
 	pNorm(2) = (ev.col(min_idx))(2).real();
 
 	pNorm.normalize();
 
 	plan.pNorm = pNorm;
 }

//------------------------------
 void PlanClassifier::bubleSort(vector<IndexType>& oriData,vector<IndexType>& labels,IndexType lSize)
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

 void PlanClassifier::setParem(IndexType _activeF,ScalarType _tradeOff)
 {
	 m_activeFrame = _activeF;
	 m_tradeOff = _tradeOff;
 }
 
 //--------------------
 void PlanClassifier::detectiveFloor(std::vector<IndexType>& oriData,std::vector<IndexType>& labels)
 {
	 std::vector< std::vector<IndexType> > label_bucket;

	 for (IndexType i = 0; i < oriData.size(); i ++)
	 {
		 IndexType lab = labels[i];
		 if (lab >= label_bucket.size())
		 {
			 label_bucket.resize(lab + 1);
		 }
		 label_bucket[lab].push_back(oriData[i]);
	 }

	 IndexType *labSize = new IndexType[label_bucket.size()];

	 for (IndexType i = 0; i < label_bucket.size(); i ++)
	 {
		 labSize[i] = label_bucket[i].size();
	 }

	 IndexType max_id = std::max_element(labSize,labSize + label_bucket.size()) - labSize;

	 m_floorId = label_bucket[max_id];
	 
	 delete []labSize;

	 //reject Outliers using RANSAC


 }

 //--------------------------
 void PlanClassifier::ransacPlan(vector<bool>&inliers)
 {
	 assert(m_floorId.size() > 3);

	 Matrix3X fltCoor;

	 inliers.resize(m_floorId.size(),false);

	 Sample& curF = m_smpSet[m_activeFrame];

	 fltCoor.setZero(3,m_floorId.size());

	 for (int i = 0; i < m_floorId.size(); i++)
	 {
		 fltCoor.col(i) = curF.vertices_matrix().col(m_floorId[i]);
	 }
	 
	 Ransac_Plan  algo_ransac(fltCoor,0.7,15,1);

	 algo_ransac.compute();

	 inliers = algo_ransac.getInliers();

	 m_floorCenter = algo_ransac.getCenter();

	 m_floorNorm = algo_ransac.getNormal();

 }

 //---------------------------
 void PlanClassifier::eliminateFloorAllFrame()
 {
	 ScalarType distErr = 40;

	 IndexType fSize = m_smpSet.size();

	 for (IndexType i = 0; i < fSize; i++)
	 {
		 if (m_activeFrame == i) continue;
		 
		 Sample& curF = m_smpSet[i];

		 IndexType pId = 0;
		 for (Sample::vtx_iterator v_iter = curF.begin();
			 v_iter != curF.end();
			 v_iter++,pId ++)
		 {
			 PointType pCoor = curF.vertices_matrix().col(pId);

			 ScalarType dist = abs( m_floorNorm.transpose() * (pCoor - m_floorCenter) );
			 
			 if (dist <= distErr)
			 {
				 (*v_iter)->set_visble(false);
			 }else
			 {
				 (*v_iter)->set_visble(true);
			 }

		 }

	 }
 }
 //--------------------
 void  PlanClassifier::usingPCL()
 {
 	 Sample& oriFrame = m_smpSet[m_activeFrame];
 
	 /* 	 IndexType pSize = oriFrame.num_vertices();
	 pcl::PointCloud<pcl::PointXYZ>::Ptr oriCloud(new pcl::PointCloud<pcl::PointXYZ>);

	 oriCloud->points.resize(pSize);

	 for (IndexType i = 0; i < pSize; i ++)
	 {
	 PointType oriP = oriFrame.vertices_matrix().col(i);
	 PointXYZ pclP(oriP.x(),oriP.y(),oriP.z());
	 oriCloud->points[i] = pclP ;
	 }

	 pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	 pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
	 pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
	 tree->setInputCloud (oriCloud);
	 n.setInputCloud (oriCloud);
	 n.setSearchMethod (tree);
	 n.setKSearch (20)
	 n.compute (*normals)*/;


 	 //pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
 
 	 //pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
 
  	 //// Create the segmentation object
  	 //pcl::SACSegmentation<pcl::PointXYZ> seg(false);
     
  	 //// Optional
  	 //seg.setOptimizeCoefficients (true);
 
 	 //// Mandatory
 	 //seg.setModelType (pcl::SACMODEL_PLANE);
 
 	 //seg.setMethodType (pcl::SAC_RANSAC);
 
 	 //seg.setDistanceThreshold (100);
 
 	 //seg.setInputCloud (oriCloud);
 
 	 //seg.segment (*inliers, *coefficients);

// 	 if (inliers->indices.size () == 0)
// 	 {
// 		 PCL_ERROR ("Could not estimate a planar model for the given dataset.");
// 		 return ;
// 	 }

 }

 PlanClassifier::PlanClassifier(IndexType cFrame) :m_smpSet(SampleSet::get_instance())
 {
	 m_activeFrame = cFrame;
	 m_tradeOff = 0.5;
	 m_floorId.clear();
 }
