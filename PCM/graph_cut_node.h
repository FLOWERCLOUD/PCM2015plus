#ifndef _GRAPH_H
#define _GRAPH_H

#include <QThread>
#include <basic_types.h>
#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include "pool_allocator.h"
#include "sample_set.h"
using namespace std;

// #ifndef frame_index_to_key
// #define frame_index_to_key(f,i) ((f<<16)|i)
// #endif // !frame_index_to_key(f,i) ((f<<16)|i
// 
// #ifndef frame_label_to_key
// #define frame_label_to_key(f,l) ((f<<16)|l)
// #endif
// 
// #ifndef get_index_from_key
// #define get_index_from_key(k) (k&0xffff)
// #endif
// 
// #ifndef get_frame_from_key
// #define get_frame_from_key(k) (k>>16)
// #endif
// 
// #ifndef get_label_from_key
// #define get_label_from_key(k) (k&0xffff)
// #endif

#define frame_index_to_key(f,i) ((f<<16)|i)
#define frame_label_to_key(f,l) ((f<<16)|l)
#define get_index_from_key(k) (k&0xffff)
#define get_frame_from_key(k) (k>>16)
#define get_label_from_key(k) (k&0xffff)	

struct GraphCutNode{
	IndexType frame;
	IndexType label;
	IndexType index;
	IndexType graph_index;
	IndexType graph_label;
	map<IndexType,IndexType> cor_frame_index;//save correspondence<frame,vertex_Id> 
	GraphCutNode(IndexType f, IndexType l, IndexType id, IndexType gi):frame(f),label(l),index(id),graph_index(gi){}
};

struct PCABox
{
	PointType center;
	PointType minPoint;
	PointType maxPoint;
	ScalarType volume;
	ScalarType diagLen;
	IndexType  vtxSize;
};



class GraphNodeCtr : public QThread
{
	//Q_OBJECT
typedef ScalarType (GraphNodeCtr::*pDistFunc)(GraphCutNode*,GraphCutNode*);

public:
	void run(); // Q_DECL_OVERRIDE;

public:
signals:
	void finish_compute();

public:
	GraphNodeCtr():cur_graph_index_(0),allocator_(),m_smpSet(SampleSet::get_instance())
	{
		m_neigNum = 50;
	}
	~GraphNodeCtr(){}
	ScalarType dist_inside_frame(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType dist_between_frame(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType dist_inside_frame_all(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType& adjDis,ScalarType& corDis);
	ScalarType weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType a_weight);
	//mix two difference distances of weight-2015-1-3
	ScalarType weight2nodes_dis(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType a_weight);
	//
	void compute_mid_and_avg(ScalarType &mid, ScalarType &avg, pDistFunc dist_func);
public:
	ScalarType measureDeformableCorVer(IndexType sFrame,IndexType sId,IndexType tFrame,IndexType tId);
	void getConstantNeigCoorByIndex(Sample & smp,IndexType * neigIndex,Matrix3X & res_coor);
	void GraphNodeCtr::calculateNeigDis(Matrix3X & neigCoor,VecX& resDis);
	ScalarType GraphNodeCtr::deformableValue(VecX& srNeigDis,VecX & resNeigDis);
	ScalarType minCorrespondenceDis(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType minVoxelRation(GraphCutNode* s_node,GraphCutNode* e_node);
	//0106
	ScalarType calculateJa_dis(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType calculateJa_dis(IndexType rowKey,IndexType colKey);
	ScalarType dist_inside_frame_all_paper(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType& ration);
	ScalarType dist_between_frame_paper(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType& jDis);

public:
	void pca_box_ctr();
private:
	void add_node(IndexType frame, IndexType label, IndexType index);
	void add_corresponding_relation( IndexType frame, IndexType index, IndexType cor_frame, IndexType cor_idx );
public:

	void read_label_file(char *filename);
	IndexType readnLabelFile(char *filename);
	void read_corres_file(char *filename);
	
	//single frame
	void read_smooth_label_file(char *filename);
	ScalarType weight2nodes_smooth(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType weight2nodes_smooth_Sy(GraphCutNode* s_node,GraphCutNode* e_node);
	void diff_using_bfs( std:: vector<IndexType>& labels,std::vector<IndexType>& centerVtxId,IndexType centerFrame );
	void propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_ori);
	void constructBoxDis(unordered_map<IndexType, set<IndexType>>& label_set);
	void calculateNorm();
	//
	unordered_map<IndexType, set<IndexType>> label_bucket;//Index by frame and label
	unordered_map<IndexType, PCABox*> box_bucket;		//Index by frame and label
	unordered_map< IndexType, GraphCutNode* > node_map;
	vector<GraphCutNode*> node_vec;

public:
	map<IndexType,IndexType> boxId;//index by frame and label,out the id of  the box in map
    MatrixXX m_boxDis;

private:
	IndexType	cur_graph_index_;
	PoolAllocator allocator_;

//private:
public:
	SampleSet & m_smpSet;
	IndexType m_neigNum;

private:
	IndexType  m_totNode;
};

#endif