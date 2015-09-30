#ifndef _GCOP_H
#define _GCOP_H

#include <QThread>
#include <basic_types.h>
#include "graph_cut_node.h"//default current folder
#include "bfs_classifier.hpp"
#include "tracer.h"
#include "co_segmentation.h"
#include "multiway_propagation.h"

#include <gco-v3.0/GCoptimization.h>
//#include <cv.h> 
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/highgui/highgui.hpp>


#define  ONLY_THREE
//#define FIT_PLAN
//#define  USING_CUR

#define EXPSWAP 
// #define frame_label_to_key(f,l) ((f<<16)|l)
// #define get_frame_from_key(k) (k>>16)
// #define get_label_from_key(k) (k&0xffff)

//using namespace cv;
//using namespace cv::ml;

class GCop : public QThread
{
	//Q_OBJECT

public:
	void run() ;//Q_DECL_OVERRIDE;

public:
signals:
	void finish_compute();

public:
	GCop();
	~GCop();

public:
	void emAlgoritm(vector<GraphCutNode*>& oriData);

	void refineSegm();

	void setNeighbor();

	void setNeighborSmpTree();

	void setDataItem();

	void setDataItem(MatrixXX&totError,unordered_map<IndexType,set<IndexType> >& edgePoints );
	void setSmoothItem();

	void visulizationLabels();

	void setParamter(IndexType _nlabel,IndexType _nExpansion,IndexType _nSwap,IndexType _nGNeig,
		             IndexType _nCNeig,ScalarType _nTraOf,ScalarType _CSigma,ScalarType _DSigma,
    				 ScalarType _nDiffu,IndexType centerF);
public:
	//co-segmentation 20150528
	void coSegmentation();
	void visualCosegmentation(char *labels_file);
	void visualCosegOriPointCloud(char *labels_file);

public:
	void ransacAlgorithm(vector<GraphCutNode*>& oriData);
	void ransacAllAlgorithm(vector<GraphCutNode*>& oriData);
	void initPlan(Matrix3X & oriCoor,PointType & center,NormalType & planNorm,set<IndexType>& inliers);
	void calculateCenterNorm(Matrix3X& smpCoor,PointType& center,NormalType & planNorm);
	void realRansac(Matrix3X& oriCoor,PointType& center,NormalType& planNorm,set<IndexType>& inliers,ScalarType noiseR,IndexType iterNum);
	void dist2Plan(Matrix3X& oriCoor,PointType& center,NormalType& planNorm,set<IndexType>& inliers,ScalarType noiseR);
	void getSmpCoor(Matrix3X&smpCoor,Matrix3X & oriCoor,set<IndexType>& inliers);
	void setInliers(map<IndexType,IndexType>& id2Id,set<IndexType>& inliers);
	void drawPlan(PointType&center,NormalType&planNorm);
	//fits a  gauss distribution
	void initDistribution(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma,set<IndexType>&inliers);
	void calculateMeanVariance(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma);
	void gaussRansac(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma,set<IndexType>&inliers,ScalarType noiseR,IndexType itNum);
	void dist2Distribution(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma,set<IndexType>&inliers,ScalarType noiseR);
	void getSmpTrans(MatrixXX & smpTrans,MatrixXX&oriTrans,set<IndexType>& inliers);
	//
	void findEdgePoint(unordered_map<IndexType, set<IndexType>> & edgePoints);
	void visualEdgePoints(unordered_map<IndexType, set<IndexType>> & edgePoints);
	void calculateEdgeMean(unordered_map<IndexType, set<IndexType>> & edgePoints,unordered_map<IndexType,PointType>&edgeMeanCoor);
	// ransacRT---5-20
	void ransacRotTan(vector<GraphCutNode*>& oriData);
	void initRotTrans(Matrix3X& trans,MatrixXX& miu,Matrix33& sigma,set<IndexType>&inliers,Matrix3X& tCoor);
	void getCorCoor(PointType& pCoor,IndexType gId);
	void rotRansac(Matrix3X& trans,MatrixXX& miu,Matrix33& sigma,set<IndexType>&inliers,ScalarType noiseR,
		           IndexType itNum,set<IndexType>& graphId,Matrix3X& tCoor);
	void dist2Transformation(Matrix3X& srCoor,MatrixXX& miu,Matrix33& sigma,set<IndexType>&inliers,ScalarType noiseR,set<IndexType>& graphId,Matrix3X& tCoor);
	void getSmpCorCoor(Matrix3X& smpCoor,set<IndexType>&inliers,Matrix3X& tCoor);

	void ransacMultiRotTan(vector<GraphCutNode*>& oriData);//累计的旋转平移
	void ransacMultiRotTan(vector<GraphCutNode*>& oriData,MatrixXX & totDist);//重载
	void smoothError(MatrixXX& totDist,map<IndexType,IndexType>& id2Id);//滤波误差并且设置数据项

public:
	void productionSamples(vector<GraphCutNode*>& oriData,MatrixXX & samples);
	
	void calculateTrans(GraphCutNode* node,MatrixXX & trans);
	void point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat,MatrixXX & transVec);
	void point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat,MatrixXX & transVec,Vec3&sMean,Vec3&tMean);
	//
	void calculateMean(MatrixXX& sample_,MatrixXX & mean_,MatrixXX& cov_0,MatrixXX& cov_1,MatrixXX& pro,MatrixXX& labels,vector<IndexType>& nLabels);
	//void calculateMean(MatrixXX& sample_,MatrixXX & mean_,CvMat* cov_1,CvMat* cov_2,MatrixXX& pro,MatrixXX& labels,vector<IndexType>& nLabels);
	void calculateCov(MatrixXX& sample_,MatrixXX& cov_);
	void calculatePro(MatrixXX& sample_,MatrixXX& mean_,MatrixXX& cov_,MatrixXX pro_);
	void getInitLabels(vector<GraphCutNode*>& oriData_,vector<IndexType> & labels_,vector<IndexType>& nLabels);
	ScalarType calculateProb(IndexType nDim,MatrixXX& mean_,MatrixXX& cov,MatrixXX& sValue);
	void rotMat2EAngle(Matrix33& rotMat,Vec3& eulerAg);
	ScalarType rationCurNode(IndexType frame,IndexType index);
	void smoothSampleLabel(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_smooth);
	void propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_ori);
	void diff_using_bfs( std:: vector<IndexType>& labels,std::vector<IndexType>& centerVtxId,IndexType centerFrame );
	IndexType orderLabels(vector<IndexType>& labels);
	void set2DataItem(vector<GraphCutNode*>& oriData);
	ScalarType dist2Mean(MatrixXX& _sample,MatrixXX& _mean,IndexType Itid,IndexType LabelId);
private:
	GraphNodeCtr* gcNode;
	GCoptimizationGeneralGraph *m_gc;
	float * m_data;
	float * m_smooth;
	vector<bool> m_isSelect;
	vector<IndexType> m_smpId;
	//IndexType m_nLabels;
	IndexType m_nDim;
	vector<IndexType> m_optLabel;

	vector<bool> m_inliers;
	IndexType m_curFId;//当前选中帧
	vector<bool> m_isEdgePoint;//用来可视化边界点

	//parameter from Ui
private:
	IndexType m_nLabels;
	IndexType m_nExpansion;
	IndexType m_nSwap;
	IndexType m_nGraphNeig;
	IndexType m_nCurNeig;
	ScalarType m_nTradeOff;
	ScalarType m_cSigma;
	ScalarType m_dSigma;
	ScalarType m_nDiffu;
	IndexType  m_centerF;
};

#endif // !_GCOPTIMIZATION_H
