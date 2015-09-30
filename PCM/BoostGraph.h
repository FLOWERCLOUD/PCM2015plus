#ifndef _BOOSTGRAPH_H
#define _BOOSTGRAPH_H

#include "basic_types.h"

using namespace  std;

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/named_graph.hpp"
#include <queue>
#include <vector>


#define frame_index_to_key(f,i) ((f<<16)|i)
#define frame_label_to_key(f,l) ((f<<16)|l)
#define get_index_from_key(k) (k&0xffff)
#define get_frame_from_key(k) (k>>16)


struct CVertex;
struct CLabel;
struct CFrame;
struct HVertex;
struct HLabel;
struct HFrame;
struct GraphVertexProperty;
struct GraphEdgeProperty;





typedef boost::adjacency_list<boost::listS, boost::vecS,boost::undirectedS,GraphVertexProperty,GraphEdgeProperty> LabelsGraph;//节点描述符


struct CVertex
{

	CVertex(IndexType vi, CLabel* lab):vtx_id(vi), label_parent(lab)
		,prev_corr(NULL),next_corr(NULL){}

	IndexType vtx_id;
	CLabel* label_parent;
	CVertex* prev_corr;
	CVertex* next_corr;
};

struct CLabel
{
	IndexType label_id;
	CFrame* frame_parent;
	map<IndexType,CVertex*> vertex_bucket;
	CLabel* prev_corr;
	CLabel* next_corr;
};

struct CFrame
{
	IndexType frame_id;
	vector<CLabel*> label_bucket;
	map<IndexType, IndexType> label_of_vtx;

	LabelsGraph* labelGraph; //保存块之间的图结构,一块对应一个节点
	vector<LabelsGraph*> hier_graph;
	//PCABox * wrapbox;
};



struct HVertex
{
	HVertex(){}
	HVertex(IndexType vi, HLabel* lab)
	{
		vtx_id = vi;
		if (lab != nullptr)
		{
			label_parent.push_back(lab);
		}
		prev_corr = NULL;
		next_corr = NULL;
	}

	IndexType vtx_id;
	vector<HLabel*> label_parent;
	HVertex* prev_corr;
	HVertex* next_corr;
};

struct HLabel
{
	IndexType label_id;
	HFrame* frame_parent;
	map<IndexType,HVertex*> vertex_bucket;

	HLabel* prev_corr;
	HLabel* next_corr; //path correspondence

	HLabel()
	{
		prev_corr = NULL;
		next_corr = NULL;
	}

	HLabel(IndexType LId, HFrame* hframe, map<IndexType,HVertex*> vtxBucket, HLabel* prev,HLabel* next)
	{
		label_id = LId;
		frame_parent = hframe;
		vertex_bucket = vtxBucket;
		prev_corr = prev;
		next_corr = next;
	}

};


struct HFrame
{
	IndexType frame_id;
	vector< vector<HLabel*> > hier_label_bucket; //帧的层次块结构
	vector< map<IndexType,IndexType> > hier_label_vtxBucket_index;//为了处理顶点的label的容量和最大label号码不匹配问题
	map<IndexType,IndexType> label_of_vtx;
	vector<LabelsGraph*> hier_graph;//帧的层次图
};
struct GraphVertexProperty
{
	GraphVertexProperty(){}
	GraphVertexProperty(IndexType id, IndexType _prev, IndexType _next):
		index(id),prev(_prev),next(_next){}

	IndexType index;
	IndexType prev;  
	IndexType next;
	IndexType label_id;
};

struct GraphEdgeProperty
{
	IndexType index;
	IndexType start_;
	IndexType end_;
	map<IndexType, map<IndexType ,HVertex*> > edgePoints;  //边界点集合

	GraphEdgeProperty(){}

	bool operator<(const GraphEdgeProperty& first) const //后面必须要const,set插入原始必须要有<运算符.
	{
		return this->index < first.index;
	}
};




typedef boost::graph_traits<LabelsGraph>::vertex_descriptor   VertexDescriptor; 
// 边描述符
typedef boost::graph_traits<LabelsGraph>::edge_descriptor     EdgeDescriptor;
// 下面是邻接链表的一些遍历器
typedef boost::graph_traits<LabelsGraph>::vertex_iterator   VertexIterator;
typedef boost::graph_traits<LabelsGraph>::edge_iterator   EdgeIterator;
typedef boost::graph_traits<LabelsGraph>::adjacency_iterator   AdjacencyIterator;
typedef boost::graph_traits<LabelsGraph>::out_edge_iterator   OutEdgeIterator;


struct pointdistance
{
	pointdistance(): vtx1Id_(-1),labelofvtx1_(-1),vtx2Id_(-1),labelofvtx2_(-1) ,distance_( 0.0)
	{

	}
	pointdistance( IndexType _vtx1Id,IndexType _labelofvtx1,IndexType _vtx2Id ,IndexType _labelofvtx2,
		ScalarType _distance): vtx1Id_(_vtx1Id),labelofvtx1_(_labelofvtx1),vtx2Id_(_vtx2Id),labelofvtx2_(_labelofvtx2) ,distance_(_distance)
	{

	}
	//应该保持labelofvtx1_ < labelofvtx2_
	IndexType vtx1Id_;
	IndexType labelofvtx1_;
	IndexType vtx2Id_;
	IndexType labelofvtx2_;
	ScalarType distance_;
};



struct point_distance_greater_than
{
	inline bool operator()( const pointdistance & lhs, const pointdistance& rhs )
	{
		return lhs.distance_ > rhs.distance_;
	}
};

typedef priority_queue< pointdistance, vector<pointdistance>, point_distance_greater_than> distanPriQueue;

struct PatchMatch
{
	PatchMatch( set<IndexType> _srPatches,set<IndexType> _tgPatches ,ScalarType _Er)
		:srPatches(_srPatches) , tgPatches(_tgPatches),Er(_Er){}
	set<IndexType> srPatches;
	set<IndexType> tgPatches;
	ScalarType	   Er;

};

struct patchmatch_error_greater_than
{
	inline bool operator()( const PatchMatch & lhs, const PatchMatch& rhs )
	{
		return lhs.Er > rhs.Er;
	}
};

#endif // !_BOOSTGRAPH_H
