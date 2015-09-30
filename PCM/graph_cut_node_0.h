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

#define frame_index_to_key(f,i) ((f<<16)|i)
#define frame_label_to_key(f,l) ((f<<16)|l)
#define get_index_from_key(k) (k&0xffff)
#define get_frame_from_key(k) (k>>16)

struct GraphCutNode{
	IndexType frame;
	IndexType label;
	IndexType index;
	IndexType graph_index;
	map<IndexType,IndexType> cor_frame_index;
	GraphCutNode(IndexType f, IndexType l, IndexType id, IndexType gi):frame(f),label(l),index(id),graph_index(gi){}
};

struct PCABox
{
	PointType center;
	PointType minPoint;
	PointType maxPoint;
};

class GraphNodeCtr : public QThread
{
	//Q_OBJECT

public:
	void run() Q_DECL_OVERRIDE;

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
	ScalarType weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node);

public:
	ScalarType measureDeformableCorVer(IndexType sFrame,IndexType sId,IndexType tFrame,IndexType tId);
	void getConstantNeigCoorByIndex(Sample & smp,IndexType * neigIndex,Matrix3X & res_coor);
	void GraphNodeCtr::calculateNeigDis(Matrix3X & neigCoor,VecX& resDis);
	ScalarType GraphNodeCtr::deformableValue(VecX& srNeigDis,VecX & resNeigDis);
	ScalarType minCorrespondenceDis(GraphCutNode* s_node,GraphCutNode* e_node);
	ScalarType minVoxelRation(GraphCutNode* s_node,GraphCutNode* e_node);

public:
	void pca_box_ctr();
private:
	void add_node(IndexType frame, IndexType label, IndexType index);
	void add_corresponding_relation( IndexType frame, IndexType index, IndexType cor_frame, IndexType cor_idx );
public:

	void read_label_file(char *filename);
	void read_corres_file(char *filename);

	unordered_map<IndexType, set<IndexType>> label_bucket;//Index by frame and label
	unordered_map<IndexType, PCABox*> box_bucket;		//Index by frame and label
	unordered_map< IndexType, GraphCutNode* > node_map;

private:
	IndexType	cur_graph_index_;
	PoolAllocator allocator_;

private:
	SampleSet & m_smpSet;
	IndexType m_neigNum;
};

#endif