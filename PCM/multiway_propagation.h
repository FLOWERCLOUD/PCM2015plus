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
	
	void propagate_back(IndexType f)
	{
		if ( components_.find(f+1)==components_.end() )
		{
			return;
		}
		CFrame& cur_frame = components_[f];
		CFrame& next_frame = components_[f+1];

		set<IndexType> new_label_set;

		for ( IndexType l=0; l<cur_frame.label_bucket.size(); ++l )
		{
			CLabel& cur_label = *(cur_frame.label_bucket[l]);
			map<IndexType, set<IndexType> > mapper;
			for (auto v = cur_label.vertex_bucket.begin();
					v != cur_label.vertex_bucket.end();
					v++)
			{
				CVertex& cur_vtx = *(v->second);
				CVertex& corr_vtx = *cur_vtx.next_corr;
				IndexType corr_label = corr_vtx.label_parent->label_id;
				auto find_if_exist = mapper.find( corr_label );
				if ( find_if_exist==mapper.end() )
				{
					mapper.insert( make_pair(corr_label, set<IndexType>()) );
				}
				mapper[corr_label].insert( corr_vtx.vtx_id );
			}

			//check whether split two part
			bool to_spilt = false;
			map<IndexType, set<IndexType> >::iterator component_to_spilt;
			if (mapper.size()==1)
			{
				component_to_spilt = mapper.begin();
				to_spilt = true;
			}
			else
			{
				bool first_encounter_no_new_label = true;
				for ( auto miter = mapper.begin(); miter != mapper.end(); miter++ )
				{
					IndexType l = miter->first;
					if( new_label_set.find(l)!=new_label_set.end() )
						continue;
					if ( first_encounter_no_new_label )
					{
						first_encounter_no_new_label = !first_encounter_no_new_label;
						component_to_spilt = miter;
						to_spilt = true;
					}
					else
					{
						// contain two old components
						to_spilt = false;
						break;
					}
				}
			}


			if( to_spilt )
			{
				 auto only = component_to_spilt;
				 IndexType map_label = only->first;
				 set<IndexType>& map_bucket = only->second;
				 map<IndexType, CVertex*>&  real_bucket = next_frame.label_bucket[map_label]->vertex_bucket;
				 if( map_bucket.size() *2 < real_bucket.size() )
				 {
					 //now split

					 //now create new component
					 IndexType new_label = (IndexType)next_frame.label_bucket.size();
					 new_label_set.insert(new_label);
					 next_frame.label_bucket.push_back( (CLabel*)0 );
					 CLabel* new_label_space = allocator_.allocate<CLabel>();
					 CLabel* new_label_obj = new (new_label_space)CLabel;
					 next_frame.label_bucket[new_label]	= new_label_obj;			
					 next_frame.label_bucket[new_label]->label_id = new_label;
					 next_frame.label_bucket[new_label]->frame_parent = &next_frame;

					 //delete elem from real_bucket that appear in map_bucket
					 auto to_remove = map_bucket.begin();
					 for (auto vv = real_bucket.begin(); vv!=real_bucket.end() && to_remove!=map_bucket.end(); )
					 {
						 IndexType vtx_idx = vv->first;
						 if( *to_remove == vtx_idx )
						 {
							 next_frame.label_bucket[new_label]->vertex_bucket.insert( *vv );
							 vv = real_bucket.erase( vv );
							 next_frame.label_of_vtx[ vtx_idx ] = new_label;
						 }
						 else if( *to_remove > vv->first )
							vv++;
						 else
							 to_remove++;
					 }

					 for (auto vv = next_frame.label_bucket[new_label]->vertex_bucket.begin();
						 vv !=next_frame.label_bucket[new_label]->vertex_bucket.end(); vv++ )
					 {
						 vv->second->label_parent = next_frame.label_bucket[new_label];
					 }
				 }


			}
		}
		smooth_label(f+1);
	}

	void propagate_front(IndexType f)
	{
		if ( components_.find(f-1)==components_.end() )
		{
			return;
		}
		CFrame& cur_frame = components_[f];
		CFrame& prev_frame = components_[f-1];

		set<IndexType> new_label_set;
		for ( IndexType l=0; l<cur_frame.label_bucket.size(); ++l )
		{
			CLabel& cur_label = *(cur_frame.label_bucket[l]);
			map<IndexType, set<IndexType> > mapper;
			for (auto v = cur_label.vertex_bucket.begin();
				v != cur_label.vertex_bucket.end();
				v++)
			{
				CVertex& cur_vtx = *(v->second);
				CVertex& corr_vtx = *cur_vtx.prev_corr;
				IndexType corr_label = corr_vtx.label_parent->label_id;
				auto find_if_exist = mapper.find( corr_label );
				if ( find_if_exist==mapper.end() )
				{
					mapper.insert( make_pair(corr_label, set<IndexType>()) );
				}
				mapper[corr_label].insert( corr_vtx.vtx_id );
			}

			//check whether split two part
			bool to_spilt = false;
			map<IndexType, set<IndexType> >::iterator component_to_spilt;
			if (mapper.size()==1)
			{
				component_to_spilt = mapper.begin();
				to_spilt = true;
			}
			else
			{
				bool first_encounter_no_new_label = true;
				for ( auto miter = mapper.begin(); miter != mapper.end(); miter++ )
				{
					IndexType l = miter->first;
					if( new_label_set.find(l)!=new_label_set.end() )
						continue;
					if ( first_encounter_no_new_label )
					{
						first_encounter_no_new_label = !first_encounter_no_new_label;
						component_to_spilt = miter;
						to_spilt = true;
					}
					else
					{
						// contain two old components
						to_spilt = false;
						break;
					}
				}
			}


			if( to_spilt )
			{
				auto only = component_to_spilt;
				IndexType map_label = only->first;
				set<IndexType>& map_bucket = only->second;
				map<IndexType, CVertex*>&  real_bucket = prev_frame.label_bucket[map_label]->vertex_bucket;
				if( map_bucket.size() *2 < real_bucket.size() )
				{
					//now split

					//now create new component
					IndexType new_label = (IndexType)prev_frame.label_bucket.size();
					new_label_set.insert(new_label);
					prev_frame.label_bucket.push_back( (CLabel*)0 );
					CLabel* new_label_space = allocator_.allocate<CLabel>();
					CLabel* new_label_obj = new (new_label_space)CLabel;
					prev_frame.label_bucket[new_label]	= new_label_obj;			
					prev_frame.label_bucket[new_label]->label_id = new_label;
					prev_frame.label_bucket[new_label]->frame_parent = &prev_frame;

					//delete elem from real_bucket that appear in map_bucket
					auto to_remove = map_bucket.begin();
					for (auto vv = real_bucket.begin(); vv!=real_bucket.end() && to_remove!=map_bucket.end(); )
					{
						IndexType vtx_idx = vv->first;
						if( *to_remove == vtx_idx )
						{
							prev_frame.label_bucket[new_label]->vertex_bucket.insert( *vv );
							vv = real_bucket.erase( vv );
							prev_frame.label_of_vtx[ vtx_idx ] = new_label;
						}
						else if( *to_remove > vv->first )
							vv++;
						else
							to_remove++;
					}
					
					for (auto vv = prev_frame.label_bucket[new_label]->vertex_bucket.begin();
						vv !=prev_frame.label_bucket[new_label]->vertex_bucket.end(); vv++ )
					{
						vv->second->label_parent = prev_frame.label_bucket[new_label];
					}
				}


			}
		}
		smooth_label(f-1);
	}

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
	void smooth_label(IndexType frame_idx)
	{
		Sample& orig_smp = SampleSet::get_instance()[frame_idx];
		Sample* downsmp_ptr = new Sample;
		map<IndexType, IndexType> idx_mapper;
		IndexType i=0;
		for ( auto viter = components_[frame_idx].label_of_vtx.begin();
				viter != components_[frame_idx].label_of_vtx.end();
				viter++,i++)
		{
			IndexType vtx_idx = viter->first;
			Vertex& vtx = orig_smp[vtx_idx];
			
			PointType v( vtx.x(), vtx.y(), vtx.z() );
			ColorType cv(vtx.r(), vtx.g(), vtx.b(), vtx.alpha());
			NormalType nv(vtx.nx(), vtx.ny(), vtx.nz());

			downsmp_ptr->add_vertex(v,nv,cv);

			idx_mapper.insert(make_pair(i, vtx_idx));
		}

		downsmp_ptr->build_kdtree();

		const IndexType k =60;
		IndexType neighbours[k];
		ScalarType dist[k];
		i = 0;
		map<IndexType, IndexType> old_label_map = components_[frame_idx].label_of_vtx;
		for ( i=0; i<idx_mapper.size(); i++)
		{
			vector<int> label_count( (int)components_[frame_idx].label_bucket.size(), 0 );
			IndexType real_vtx_idx = idx_mapper[i];
			downsmp_ptr->neighbours(i, k, neighbours, dist );
			for ( IndexType neig_id = 0; neig_id<k; neig_id++ )
			{
				IndexType real_neig_id = idx_mapper[neighbours[neig_id]];
				IndexType neig_label = old_label_map[real_neig_id];
				label_count[neig_label]++;
			}
			IndexType max_freq_label = max_element(label_count.begin(), label_count.end()) - label_count.begin();
			IndexType cur_vtx_label = old_label_map[real_vtx_idx];

			if ( max_freq_label!=cur_vtx_label )
			{
				//change label
				auto vv = components_[frame_idx].label_bucket[cur_vtx_label]->vertex_bucket.find(real_vtx_idx);
				assert(vv!=components_[frame_idx].label_bucket[cur_vtx_label]->vertex_bucket.end());
				components_[frame_idx].label_bucket[max_freq_label]->vertex_bucket.insert(*vv);
				components_[frame_idx].label_bucket[cur_vtx_label]->vertex_bucket.erase(vv);
				components_[frame_idx].label_of_vtx[real_vtx_idx] = max_freq_label;
			}
		}

		delete downsmp_ptr;

	}

	void write_label_file(std::string filename)
	{

		FILE* outfile = fopen(filename.c_str(),"w");
		for ( auto frame_iter = components_.begin();
				frame_iter != components_.end();
				frame_iter++ )
		{
			CFrame& frame = frame_iter->second;
			IndexType frame_idx = frame_iter->first;
			for ( auto label_iter = frame.label_bucket.begin(); 
				label_iter!=frame.label_bucket.end(); label_iter++  )
			{
				CLabel& label = **label_iter;
				IndexType label_idx = label.label_id;
				for ( auto vtx_iter = label.vertex_bucket.begin();
					vtx_iter!=label.vertex_bucket.end();
					vtx_iter++)
				{
					CVertex& vtx = *(vtx_iter->second);
					fprintf(outfile, "%d %d %d\n", frame_idx, label_idx, vtx.vtx_id);
				}
			}
		}

		fclose(outfile);
	}

	void compute()
	{
		for (auto citer = components_.begin(); citer!=components_.end(); citer++)
		{
			propagate_back_with_link(citer->first);
			//propagate_back(citer->first);
			
		}

		//show_corresponding(14);


		for (auto citer = components_.rbegin(); citer!=components_.rend(); citer++)
		{
			//propagate_front(citer->first);
		}


 		for (auto citer = components_.begin(); citer!=components_.end(); citer++)
 		{
 			//propagate_back(citer->first);
 		}

		//for (auto citer = components_.rbegin(); citer!=components_.rend(); citer++)
		//{
		//	propagate_front(citer->first);
		//}

		return;
	}

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
	DualwayPropagation()
	{
		downSample = NULL;
		frameTimer = NULL;
		currentVisframe = 1;
		startframe = 1;
		endframe = 9;
	}
	~DualwayPropagation()
	{
		if( downSample){
			delete downSample;
		}
	}
	static DualwayPropagation& get_instance()
	{
		static DualwayPropagation	instance;
		return instance;
	}

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