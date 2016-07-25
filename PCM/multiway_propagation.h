#ifndef _MULTIWAY_PROPAGATE_H
#define _MULTIWAY_PROPAGATE_H

#include "co_segmentation.h"
#include "pool_allocator.h"
#include "tracer.h"

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/named_graph.hpp"
class QTimer;
class QObject;
struct CVertex;
struct CLabel;
struct CFrame; 

//using namespace boost;

//typedef boost::adjacency_list<boost::listS, boost::vecS,boost::undirectedS> LabelsGraph;

struct CVertex;
struct CLabel;
struct CFrame;

struct HVertex;
struct HLabel;
struct HFrame;
struct GraphVertexProperty;
struct GraphEdgeProperty;
typedef boost::adjacency_list<boost::listS, boost::vecS,boost::undirectedS,GraphVertexProperty,GraphEdgeProperty> LabelsGraph;

typedef boost::graph_traits<LabelsGraph>::vertex_descriptor   VertexDescriptor; 
// 边描述符
typedef boost::graph_traits<LabelsGraph>::edge_descriptor     EdgeDescriptor;
// 下面是邻接链表的一些遍历器
typedef boost::graph_traits<LabelsGraph>::vertex_iterator   VertexIterator;
typedef boost::graph_traits<LabelsGraph>::edge_iterator   EdgeIterator;
typedef boost::graph_traits<LabelsGraph>::adjacency_iterator   AdjacencyIterator;
typedef boost::graph_traits<LabelsGraph>::out_edge_iterator   OutEdgeIterator;


#define INF 1000000

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

//struct HVertex
//{
//	HVertex(){}
//	HVertex(IndexType vi, HLabel* lab):vtx_id(vi), label_parent(lab)
//		,prev_corr(NULL),next_corr(NULL){}
//
//	IndexType vtx_id;
//	IndexType label_id;
//	//HLabel* label_parent;
//	vector<HLabel*> label_parent;
//	HVertex* prev_corr;
//	HVertex* next_corr;
//};

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
	HLabel(){}

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
class DualwayPropagation;

class DualwayPropagation
{
	friend class UtilityClass;
	friend class PropagateThread;

public:
	void propagate_back_with_link(IndexType f);
	
	void propagate_back(IndexType f);

	void propagate_front(IndexType f);

	void read_data(char *label_name,char *corr_name);

	//void init_labeles_graph()
	//{
	//	for (auto citer = components_.begin(); citer!=components_.end(); citer++)
	//	{
	//		IndexType nodeSize = citer->second.label_bucket.size();

	//		LabelsGraph* new_labelGraph_space = allocator_.allocate<LabelsGraph>();

	//		LabelsGraph* new_labelGraph = new (new_labelGraph_space)LabelsGraph;


	//		new_labelGraph->added_vertex(nodeSize);

	//		for (IndexType i = 0; i < nodeSize-1; i ++)
	//		{
	//			for (IndexType j = i + 1; j < nodeSize; j++)
	//			{

	//				CLabel* fir = citer->second.label_bucket[i];

	//				CLabel* sec = citer->second.label_bucket[j];

	//				map<IndexType,CVertex*> fir_vtx = fir->vertex_bucket;

	//				map<IndexType,CVertex*> sec_vtx = sec->vertex_bucket;

	//				//计算两个块之间的最短距离
	//				ScalarType minDis = 1000000;
	//				SampleSet& sample_set = SampleSet::get_instance();
	//				map<IndexType,CVertex*>::iterator biter1 , eiter1 ,biter2,eiter2;
	//				eiter1 = fir_vtx.end();
	//				eiter2 = sec_vtx.end();
	//				ScalarType dia ;
	//				for( biter1 = fir_vtx.begin() ;biter1 != eiter1 ;++biter1){
	//					for( biter2 = sec_vtx.begin() ;biter2 != eiter2 ;++biter2){
	//						IndexType index1 = biter1->first;
	//						Sample& s = sample_set[citer->first];
	//						dia = s.getBox().diag();
	//						
	//						PointType point1 =  s.vertices_matrix().col(index1);

	//						IndexType index2 = biter2->first;
	//					
	//						PointType point2 =  s.vertices_matrix().col(index2);
	//						
	//						ScalarType distance = (point1 - point2).norm();
	//						if(distance < minDis)minDis = distance;
	//						

	//					}

	//				}
	//				 
	//				if (minDis <  dia * 0.0588 )
	//				{

	//		           boost::add_edge(i,j,*new_labelGraph);

	//				}

	//			}
	//		}

	//		citer->second.labelGraph = new_labelGraph;
	//
	//	}

	//}
	void getEdgeVertexs( IndexType _CFrameId ,IndexType lLabelId ,IndexType rLabelId , map<IndexType, map<IndexType ,HVertex*> >& _edgepoints );
	void getEdgeVertexsByMinDIstance( IndexType _CFrameId , distanPriQueue& _PriQuemap, map<IndexType, map<IndexType ,HVertex*> >& _edgepoints );
	void init_labeles_graph_hier();
	void init_node_link(int depth = 0);

	void init_node_link(IndexType _frameId , IndexType _depth);


	//void read_label_file(char *filename)
	//{
	//	FILE* in_file = fopen(filename, "r");
	//	if (in_file==NULL)
	//	{
	//		return;
	//	}
	//	IndexType frame, label, vtx_idx;
	//	while ( true )
	//	{
	//		int stat =  fscanf(in_file, "%d %d %d\n",&frame, &label, &vtx_idx);
	//		if (stat == EOF)
	//			break;
	//		if ( components_.find(frame)==components_.end() )
	//		{
	//			components_.insert(make_pair(frame, CFrame()));
	//			components_[frame].frame_id = frame;
	//		}
	//		if ( label >= components_[frame].label_bucket.size() )
	//		{
	//			components_[frame].label_bucket.resize( label+1 );
	//		}
	//		if (   nullptr==components_[frame].label_bucket[label] )
	//		{
	//			CLabel* new_label_space = allocator_.allocate<CLabel>();
	//			CLabel* new_label = new (new_label_space)CLabel;
	//			components_[frame].label_bucket[label] = new_label;
	//			components_[frame].label_bucket[label]->frame_parent = &components_[frame];
	//			components_[frame].label_bucket[label]->label_id = label;
	//		}
	//		CVertex* new_space = allocator_.allocate<CVertex>();
	//		CVertex* new_vtx = new (new_space)CVertex(vtx_idx, components_[frame].label_bucket[label]);
	//		components_[frame].label_bucket[label]->vertex_bucket.insert( make_pair(vtx_idx,new_vtx) );
	//		components_[frame].label_of_vtx.insert( make_pair(vtx_idx, label) );
	//		//ADD BY HUAYUN 
	//		SampleSet& sample_set = SampleSet::get_instance();
	//		Sample& sample = sample_set[frame];
	//		 
	//	 

	//	 	 

	//	}
	//}

	/*void read_corres_file(char *filename)
	{
		FILE* in_file = fopen(filename,"r");
		if(in_file==NULL)
		{
			return;
		}
		IndexType cur_frame, cur_vtx_idx, next_frame, next_vtx_idx;
		while (true)
		{
			int stat = fscanf(in_file,"%d %d %d %d\n",&cur_frame, &cur_vtx_idx, &next_frame, &next_vtx_idx);
			if(stat==EOF)break;

			if( components_.find(cur_frame)==components_.end() || components_.find(next_frame)==components_.end())
				continue;

			IndexType label = components_[cur_frame].label_of_vtx[cur_vtx_idx];

			IndexType next_label = components_[next_frame].label_of_vtx[next_vtx_idx];
			CVertex& cur_vtx = *components_[cur_frame].label_bucket[label]->vertex_bucket[cur_vtx_idx];
			if ( cur_frame+1 == next_frame  )
			{
				cur_vtx.next_corr = components_[next_frame].label_bucket[ next_label]->vertex_bucket[next_vtx_idx];
			}
			else if (cur_frame-1 == next_frame)
			{
				cur_vtx.prev_corr = components_[next_frame].label_bucket[ next_label]->vertex_bucket[next_vtx_idx];
			}
		}
	}*/
	void read_corres_file_hier(char* filename);

	void  read_label_file_hier(char *filename);

	void show_corresponding(int f);
	void show_correspondingframeandlabel( std::vector<int>& f , std::vector<int>& label);
	void smooth_label(IndexType frame_idx);

	void write_label_file(std::string filename);

	void compute();

	void buildSmapleKDtree(CFrame* smpCloud, Sample* smp);

	void point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat,MatrixXX & transVec);

	//0821
	void split_twoAjacent_graph_next(IndexType srFrame, IndexType tgFrame);

	void split_twoAjacent_graph_prev(IndexType srFrame, IndexType tgFrame);

	void split_nest_graph_prev(IndexType srFrame, IndexType tgFrame);

	void splitAllSquenceGraph();

	//2015/8/24
	void getNextCorVtx(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor);

	IndexType checkNextLabelBucket(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor);

	void minDistBeTwoParts(IndexType cFrame,map<IndexType,HVertex*>& fPart,map<IndexType,HVertex*>& sPart, ScalarType& misDis);

	void determinateUnmarkPoints(IndexType cFrame,map<IndexType,HVertex*>& unMarkPs,vector<HLabel*> oriLabelBucket,IndexType nodeId,IndexType newLabe);

	ScalarType p2PatchAvgDis(IndexType cFrame, PointType& pCoor,map<IndexType,HVertex*> parthPs);

	void copyLabelBucket(vector<HLabel*>& leftLabels, const vector<HLabel*>& oriLabels);
	void wirteSplitGraphLables(std::string filename);
	void changedDepthAndDispaly( int _depth );
	DualwayPropagation();
	~DualwayPropagation();
	static DualwayPropagation& get_instance();

	void buildKdTree(IndexType _cframeid );
	void processButton();
	void startframeAnimation(QTimer*&  p_qtimer );
	void startlayerAnimation();
	void stopframeAnimation();
	void stoplayerAnimation();
	void excFrameAnimation();
	void pauseframeAnimation();
	void processButtonRunOrPuase();
	void wirteGraphLables(std::string filename ,IndexType _depth);
	void initialLabelDisplay();
	void init_labeles_graph_hier2(int _depth =0);
	void getEdgeVertexs2( IndexType _CFrameId , distanPriQueue& _PriQuemap, map<IndexType, map<IndexType ,HVertex*> >& _edgepoints );
	void wirteGraphLablesAtTop(std::string filename );
	void writeOrigCloudWithLabel(const std::string _filename);

public:
	map<IndexType, CFrame> components_;
	map<IndexType,IndexType> frame_index_map_label_; 
	PoolAllocator allocator_;

	map<IndexType, HFrame> hier_componets_;
	Sample* downSample;
	map<IndexType,IndexType> o2d_map; // origin to down
	map<IndexType,IndexType> d2o_map;

	QTimer * frameTimer;
	IndexType	currentVisframe;
	IndexType   startframe;
	IndexType	endframe;

};


#endif