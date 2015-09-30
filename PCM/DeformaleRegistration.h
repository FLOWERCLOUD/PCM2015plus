#ifndef  _DEFORMABLEREGISTRATION_H
#define  _DEFORMABLEREGISTRATION_H

#include <QThread> 
#include <set>
#include "tracer.h"
#include "sample_set.h"

struct LifeSpan
{
	IndexType  start;
	IndexType  end; 
};
struct PCloudTraj 
{
	PCloudTraj(){}
	PCloudTraj(IndexType _start,IndexType _end,vector<IndexType> _trajNode)
	{
		trajLifeSpan.start    = _start;
		trajLifeSpan.end      = _end;
		trajNode = _trajNode;
	}
	LifeSpan           trajLifeSpan;
	vector<IndexType>  trajNode;
};

struct PCloudModel
{
	PCloudModel(){}
	LifeSpan  modeLifeSpan;
	vector<Matrix33> fNode;//rigid motion
	vector<Matrix33> bNode;
	Matrix3X         Centers;
};

struct PCloudAffModel
{
	PCloudAffModel(){};
	LifeSpan modelLifeSpan;
	vector<Matrix34> fNode;
	vector<Matrix34> bNode;
};

class DeformableRegistration :public QThread
{
	Q_OBJECT

public:
	void run() ;
public:
	DeformableRegistration();
	~DeformableRegistration(){};
public:
       signals:
	     void finish_compute();

public:

public:
	void   estimateCorrespondence(Sample &srGraph,Sample & tgGraph,MatrixXXi & res_cor);
	ScalarType avgDistancePoint(IndexType pointIndex,Sample & smp,IndexType neigNum,Matrix3X & dst,Vec3 & mean);
	ScalarType totalDistance(Sample &smp,IndexType neigNum,Matrix3X & dst);
	IndexType   findMinCorr(PointType & srPoint,Vec3& mean, int * neigIndex,Sample & smp);
	void   calculateRestPos(Sample& srGraph,Sample& tgGraph,MatrixXXi & vtx_cor,Matrix3X & restPos);
	void   getNeigCoorByIndex(Sample & smp,vector<IndexType> & neigIndex,Matrix3X & res_coor);
	void   updateCoor(PointType& srpoint,Matrix3X & srNeigCoor,Matrix3X & tgNeigCoor);
	void   getCorNeigIndex(IndexType * srNeigIndex,IndexType * tgNeigIndex,MatrixXXi & corMat);
	void   combinationCoor(ScalarType stiffCoff,Sample & srGraph,Sample & trGraph,Matrix3X & restPos,MatrixXXi& vtx_cor);
	void   combinationCoor(ScalarType stiffCoff,Sample & srGraph,Sample & trGraph,
		                   Matrix3X & restPos,MatrixXXi& vtx_cor,MatrixXX& isDeformable,std::set<IndexType> & distLrgIndex);
	void   updateCooByTPS(Sample & srCloud,Matrix3X & oriCoor,Sample & srGraph,Sample& tgGraph);
	//2014-10-21-using TPS
	void deformationUsingTPS(Sample& srCloud,Sample& tgCloud,IndexType feaNum);
	void getSamplePoints(Sample&oriCloud,Matrix3X& samplePoints,IndexType feaNum);
	void getSamplePointsAll(Sample&srCloud,Matrix3X& sampleSrPoints,
		                    Sample&tgCloud,Matrix3X& sampleTgPoints,
							IndexType pointNum,vector<bool>& isSample);
	//
	void   calculateMapFunction(Matrix3X& srGraphCoor,Matrix3X& tgGraphCoor,IndexType axisIndex, Matrix3X & res_mapFunc);
	void   interpolateCoordinate(Sample & srCloud,Matrix3X & mapFunctionCoff,Matrix3X& tgGraphCoor,vector<bool>& isSample);
	void   constructUpperMat(MatrixXX & upperMat,Matrix3X & srCoor,Matrix3X& tgCoor,IndexType axisIndex);
	void   sloveLinearSysByLU(MatrixXX & LeftMat,MatrixXX & RightMat,Matrix3X& res_mapFunc,IndexType axisIndex);
	ScalarType  tpsRBF(ScalarType radius);
	void   loaclICP(Sample & srGraph,Sample& tgGraph,MatrixXXi & vtx_cor,Matrix3X& restPos);
	void   localICP(Sample & srGraph,Sample& tgGraph,MatrixXXi & vtx_cor,
		            Matrix3X& restPos,MatrixXX& isDeformable); 
	//从新计算扭曲量
	void   localICPDisto(Sample & srGraph,Sample& tgGraph,MatrixXXi & vtx_cor,
		Matrix3X& restPos,MatrixXX& isDeformable);
	//
	void   optimizeCoor(Sample&srGraph,Sample&tgGraph,MatrixXXi & vtx_cor);

	//processing real data
public:
	void calculateTrajFrature(MatrixXX & featureMat);//record trajectory feature
	void point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat);
	void non_rigidRegister(Sample& srGraph,Sample& tgGraph,MatrixXXi & vtxMap,
		                   MatrixXX & isDeformbale,std::set<IndexType> & distLrgIndex);
	void intersectionCorrespondence(IndexType * srLoaclNeig,IndexType * tgLoaclNeig,MatrixXXi & vtx_map,
		                            vector<IndexType> & srInterRes,vector<IndexType> & tgInterRes);
	void intersectionCorrespondenceInverse(IndexType * srLoaclNeig,IndexType * tgLoaclNeig,MatrixXXi & vtx_map,
		vector<IndexType> & srInterRes,vector<IndexType> & tgInterRes);
	void getConstantNeigCoorByIndex(Sample & smp,IndexType * neigIndex,Matrix3X & res_coor);
	void calculateNeigDis(Matrix3X & neigCoor,VecX& resDis);
	ScalarType deformableValue(VecX& srNeigDis,VecX & resNeigDis);
	void calculateTrajEndIndex(MatrixXXi & TrajBeginIndex,MatrixXXi& globalCor2Frame,MatrixXXi& trajEndIndex);
	void initFrontVtxMap(MatrixXXi& frontVtxMap,IndexType trajNum);
	void alignTargetCoor(Matrix3X & tgCloud,MatrixXXi & vertexMap);
	IndexType recordCorIndex(MatrixXXi & glb_cor,std::set<IndexType> recordCorSet);

	// in order to clustering
	void calculateLifeSpansTraj(vector<PCloudTraj> & totalTraj,IndexType LifeMin);
	//2015-7-22
	void calculateDownSmpLifeSpanTraj(vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap,
		                              IndexType length,IndexType octreeRes ,ScalarType dfThreshold, IndexType LifeMin);
	//2015-7-22
	//7-28
	void calculateDownSmpLifeSpanTrajCenter(vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap,
		IndexType length,IndexType octreeRes ,ScalarType dfThreshold, IndexType LifeMin);

	void drawLifespansTraj(vector<PCloudTraj> & lifeSpans);
	void makeCorrespondenceOne2One(MatrixXXi& globalMap,MatrixXX& deformableValue,IndexType tgVtxNum);
	PCloudModel constrcutRigidMotionMode(vector<PCloudTraj> & oriTraj,LifeSpan & comTime);
	PCloudModel constrcutRigidMotionMode(PCloudTraj& traj);

	//construct affine transformation 7-30
	PCloudAffModel constrcutAffineMotionMode(vector<PCloudTraj> & oriTraj,LifeSpan & comTime);
	void calculateAffineTrans(Matrix3X& srCoor, Matrix3X& tgCoor,Matrix34& affineMat);
	//Index the node coordinate
	void getTrajNodeCoordinate(PCloudTraj & traj,IndexType frameIndex,PointType & resCoor);

	//get the traj nodes coordinate no the same frame
	void getMultiTrajNodeCoordinate(vector<PCloudTraj>& multiTraj,IndexType frameId,Matrix3X &resCoorMat);

	// get the affine transformation from srPCloud to tgPCloud--using least squares
	void calculateTransformation2PClouds(Matrix3X & srPCoud,Matrix3X & tgPCloud,Matrix33 & transMat);

	// rand produce 3 traj from total tajs
	bool sample3validTraj(vector<PCloudTraj> & totTraj,vector<PCloudTraj>& resTraj,LifeSpan & lifesp);

	// rand sample N traj
	bool sampleValidTraj(vector<PCloudTraj> & totTraj,vector<PCloudTraj>& resTraj,LifeSpan & lifesp,IndexType num);

	// get the trajs  common time
	bool getCommonTime(vector<PCloudTraj> & eleTraj,LifeSpan & lifesp);

	// arbitraily traj and arbitraily model 's distance ,if no commontime then distance is infinty
	ScalarType traj2ModlDistance(PCloudTraj& traj, PCloudModel& model);

	//get a subset of the traj with comTime
	void intersectionTrajWithComTime(PCloudTraj & oriTraj,LifeSpan & comTime,PCloudTraj & newTraj);

	// get a subset of the motion model  with comTime
	void intersectionModelWithComTime(PCloudModel & oriModel,LifeSpan & comTime,PCloudModel & newModel);

	// get the smallest distance between traj and model
	ScalarType minTraj2ModelDis(PCloudTraj& traj, PCloudModel& model,LifeSpan & comTime);

	//give a point from a traj and produce a  new traj under a motin model
	void calNewTrajUnderModel(IndexType nodeId,PCloudModel & model,PointType oriNodePos,MatrixXX & rowNewTraj);

	// total test calculate pmodel
	void testingcalPmodel();

	// rand produce three valid trajs and calculate motion model with common time
	bool sampleModel(vector<PCloudTraj>& oriTraj,vector<PCloudModel>& smpOriModel,IndexType modelNum);

	//rand produce K motion models
	bool sampleModelByNeig(vector<PCloudTraj>& oriTraj,vector<PCloudModel>& smpOriModel,IndexType modelNum);

	// 随机生成仿射变换模型
	bool sampleAffineModel(vector<PCloudTraj>& oriTraj,vector<PCloudAffModel>& smpOriModel,IndexType modelNum);

	// a  traj produce a motion model and the life-span is the same
	PCloudModel constructRigidMotionByNeig(PCloudTraj & oriTraj);

	// calculate the distance between a traj and a model
	void staticDistance(vector<PCloudTraj>& oritrj,vector<PCloudModel>& oriModel);

	//test J-linkage cluster

	// produce a dream traj set
	void produceDreamTraj(vector<PCloudTraj>& resTraj);
	void produceDreamTraj(vector<PCloudTraj>& resTraj,vector<IndexType>& sampleCenter);
	//calculate center
	void calculateCenter(Matrix3X& oriCoordinate,Eigen::Vector3f& mean);

	//calculate fix length traj
	void calculateFixedLengthTraj( vector<PCloudTraj>& trajs, IndexType smp_idx,IndexType length );

	//calculate fix length traj Add Vtx_map
	void calculateFixedLengthTraj( vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap, IndexType length,IndexType octreeRes );

	//calculate traj feature for kmeans--available only traj have the same length
	void calculateTrajFeature(vector<PCloudTraj>& oriTraj,IndexType trajLen,MatrixXX& featureMat);

	//down sample traj
	void downSampleTraj(vector<PCloudTraj>& oriTraj,vector<PCloudTraj>&sampleTraj,Sample&centerFrame,vector<IndexType>&sampleVtxId);
	
	//global non-rigid and sample traj;
	void calculateFixedLengthTrajBySampleTraj( vector<PCloudTraj>& trajs, IndexType smp_idx,IndexType length ,vector<IndexType>& sampleVtxMap);
	
	//连续俩帧做对应
	void calculateFixedLengthTrajWithTracingAlong(vector<PCloudTraj>& trajs, IndexType smp_idx,vector<IndexType>& centerVtxMap,IndexType length, IndexType octreeRes );

	//propagate the sample labels to original points//2014-12-5
	void propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& labels,vector<IndexType>& label_ori);

	//smooth sample label using neighborhood
	void smoothSampleLabel(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& labels,vector<IndexType>& label_ori);

	//对采样点建立一个kdtree进行查找运算
	void smoothSmapleLabel_KDTree(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& labels,vector<IndexType>& label_ori);

	// read sample labels file
	void readSampleLabelFile(char *filename,vector<IndexType>&labels,vector<IndexType>&label_smooth,vector<IndexType>&sampleCenter,IndexType & centerFrame);
	
	// calculate all vertex normal
	void calculateNorm();
	//
	void calNewTrajNormalUnderModel(IndexType nodeId,PCloudModel & model,NormalType oriNodePos,MatrixXX & rowNewTrajNorm);

	//calculateNormalRoatmate
	void calculateNormalRotMat(NormalType& sN,PointType& tN,Matrix33& rotMat);
public:	
	//visualization used by testing---
	void visNeigVertex(Sample& oriCloud,VecX& vtxIndex);
	void updateLocalCoordinate(Sample& oriCloud,Matrix3X& vtxCoor,VecX& vtxIndex);
	ScalarType calcualteInnerProduct(MatrixXX & oriNorm,MatrixXX& newNorm);
	void sampleModel_norm(vector<PCloudTraj>& oriTraj,vector<PCloudModel>& smpOriModel,IndexType modelNum);

	//2015-01-21-
public:
	void sampleLifeSpanTraj(vector<PCloudTraj> & oriTraj,vector<PCloudTraj>& resTraj,IndexType & cFrame,ScalarType& thresH);
	PCloudTraj constructNewLifeTraj(PCloudTraj & oriTraj,IndexType& cFrame,ScalarType& thresH);
	ScalarType measureDeformableCorVer(IndexType sFrame,IndexType sId,IndexType tFrame,IndexType tId);
	void calculateVtxDistor(vector<PCloudTraj>& sampleTraj,vector<ScalarType>& disVal,IndexType cFrame);
	ScalarType maxDistOfTraj(PCloudTraj & traj,IndexType Cframe);
	ScalarType minDistOfTraj(PCloudTraj & traj,IndexType Cframe);

	// 计算两对应点的扭曲量(𝐶𝑛^2)
	ScalarType deformableTotal(Matrix3X& srCoor, Matrix3X& tgCoor);
	ScalarType deformableICP(Matrix3X& srCoor, Matrix3X& tgCoor);

	//
    void setNeigNum(IndexType _neigN);

private:
	SampleSet & m_smpSet;
	IndexType m_neigNum;
};
#endif