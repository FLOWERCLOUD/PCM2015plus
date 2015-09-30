#ifndef _TRAJECTORY_CLASSIFIER_H
#define _TRAJECTORY_CLASSIFIER_H

#include <QThread>
#include "sample_set.h"
#include "DeformaleRegistration.h"

//#define SAVE_CORRESPONDENCE

class TrajectoryClassifier : public QThread
{
	Q_OBJECT

public:
	TrajectoryClassifier(IndexType cFrame)
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
	}

	void run() ; //Q_DECL_OVERRIDE;

	signals:
		void finish_compute();
public:
	void derive_rotation_by_svd(VecX& rot,const MatrixX3 &X,  MatrixX3& Y,MatrixXXi& vtx_map);
	void bubleSort(vector<IndexType>& oriData,vector<IndexType>& labels,IndexType size);
	void bubleSort(vector<IndexType>& oriData,vector<ScalarType>& labels,IndexType size);
	void visDistor();
	int orderLabels(vector<IndexType>& labels);
	void  setParamter(IndexType _trajLen,IndexType _octreeReso,ScalarType _perC,
		              ScalarType _thresHold,IndexType _modelT, IndexType _smallL,bool _isEqual,bool _isRigid);
	void setNeigNum(IndexType _neigbNum);
private:
	void derive_rotation_by_svd( VecX& rot,const MatrixX3 &X, const MatrixX3& Y);
	void diff_using_bfs( std:: vector<IndexType>& labels,std::vector<IndexType>& centerVtxId,IndexType centerFrame );
	
	IndexType centerFrame;
	IndexType trajLen;
	IndexType octreeRes;
	ScalarType perC;
	ScalarType threshold;
	IndexType modelT;
	IndexType lifeT;

	bool isEqual;
	bool isRigid;
	IndexType neigborNum;

};

#endif