#ifndef _PLANS_FIT_H
#define _PLANS_FIT_H

#include <QThread>
//#include "sample_set.h"
#include "point2Plan_distance.h"
#include <vector>
#include "globals.h"
//#include "basic_types.h"
//#include "sample.h"
#include <algorithm>

#include "RANSAC_PLAN.h"

//#include <pcl/ModelCoefficients.h>
//#include <pcl/io/pcd_io.h>
//#include <pcl/point_types.h>
//#include <pcl/kdtree/kdtree_flann.h>
//#include <pcl/features/normal_3d.h>
//#include <pcl/surface/gp3.h>

// #include <pcl/sample_consensus/method_types.h>
// #include <pcl/sample_consensus/model_types.h>
// #include <pcl/segmentation/sac_segmentation.h>
//using namespace pcl;

class SampleSet;
class Sample;
using namespace std;


class PlanClassifier:public QThread
{
	//Q_OBJECT//加了之后编译连接错误

public:
	PlanClassifier(IndexType cFrame);

public:
	void run(); // Q_DECL_OVERRIDE;

public:
signals:
	void finish_compute(){};

public:
 	void samplePlanModels(Sample& sFrame,std::vector<PlanModel>& planModels,ScalarType ration);
 	bool sample3Points(Matrix3X&pCoor,Sample&sframe);
 	void fitPlan(Matrix3X&pCoor,PlanModel& plan);
 	void bubleSort(std::vector<IndexType>& oriData,std::vector<IndexType>& labels,IndexType lSize);
	void setParem(IndexType _activeF,ScalarType _tradeOff);
	//
	void detectiveFloor(std::vector<IndexType>& oriData,std::vector<IndexType>& labels);
	void ransacPlan(vector<bool>&inliers);
	//
	void eliminateFloorAllFrame();
	//using PCL functions
	void usingPCL();
private:
	IndexType m_activeFrame;
	ScalarType m_tradeOff;
	std::vector<IndexType> m_floorId;
	PointType  m_floorCenter;
	NormalType m_floorNorm;
	SampleSet & m_smpSet;
};

#endif