#ifndef _GRAPH_MATCHING_H
#define _GRAPH_MATCHING_H

#include "basic_types.h"
#include "sample.h"
#include "sample_set.h"
#include "multiway_propagation.h"
#include "queue"
#define  INF 1000000
#define  COUT_DEBUG 0

#include <set>

using namespace std;

struct HVertex;
struct HLabel;
struct HFrame;
struct GraphVertexProperty;
struct GraphEdgeProperty;
struct GraphVertexProperty;
struct GraphEdgeProperty;


typedef boost::adjacency_list<boost::listS, boost::vecS,boost::undirectedS,GraphVertexProperty,GraphEdgeProperty> LabelsGraph;//�ڵ�������

typedef boost::graph_traits<LabelsGraph>::vertex_descriptor   VertexDescriptor; 
// ��������
typedef boost::graph_traits<LabelsGraph>::edge_descriptor     EdgeDescriptor;
// �������ڽ������һЩ������
typedef boost::graph_traits<LabelsGraph>::vertex_iterator   VertexIterator;
typedef boost::graph_traits<LabelsGraph>::edge_iterator   EdgeIterator;
typedef boost::graph_traits<LabelsGraph>::adjacency_iterator   AdjacencyIterator;
typedef boost::graph_traits<LabelsGraph>::out_edge_iterator   OutEdgeIterator;

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

class FrameWithNodeCaculation
{

public: 
	FrameWithNodeCaculation(SampleSet& _smpSet, map<IndexType,HFrame>& _componet,IndexType srFrame,IndexType tgFrame): sample_set_(_smpSet), components_(_componet)
	{
		CFrameId_ = srFrame;
		NFrameId_ = tgFrame;
	}

	std::set< set<IndexType> >  getLatenteSet(IndexType CFrameId , const std::set<IndexType>& slabel_set , IndexType NFrameId){ 
		std::set< set<IndexType> > L;
		return L;}


	void calculateSimilar2Frame(IndexType CFrameId, IndexType NFrameId)
	{
		if( components_.find(CFrameId+1) == components_.end())return;

		HFrame& cur_frame = components_[CFrameId];

		IndexType srLevel = components_[CFrameId].hier_graph.size();
		IndexType tgLevel = components_[NFrameId].hier_graph.size();

		srLevel--;
		tgLevel--;

		LabelsGraph* labelGraphOfCFrame = cur_frame.hier_graph[srLevel];

		//���������
		LabelsGraph::vertex_iterator CFvertex_biter ,CFvertex_eiter;

		LabelsGraph::adjacency_iterator CFadj_bitr , CFadj_eitr;

		tie( CFvertex_biter ,CFvertex_eiter) = boost::vertices(* labelGraphOfCFrame);

		for(; CFvertex_biter != CFvertex_eiter ; ++CFvertex_biter)
		{
			//�Ӷ���������ж�

			if (!equeue_.empty())
			{
				while (!equeue_.empty())
				{
					equeue_.pop();
				}
			}

			IndexType vtx_id = *CFvertex_biter;


			IndexType vtxSize = components_[CFrameId].hier_label_bucket[srLevel][vtx_id]->vertex_bucket.size();

			if ( vtxSize < 5) //�����̫����ȥ�Ҷ�Ӧ��
			{
				Logger<<"  �鱾ʡ̫С,��ʱ���Ҷ�Ӧ!.\n";
				continue;
			}

			boost::tie(CFadj_bitr , CFadj_eitr) = boost::adjacent_vertices( *CFvertex_biter ,*labelGraphOfCFrame ); // ����ڽӵ�ĵ�����

			std::set<set<IndexType> > unique_pair_set;

			//�����ڵ� ƥ��
			std::set<IndexType> CFcmp_node ,NFcmp_node;  //�ӿڶ��� �����ڴ������ڼ����������Ͻڵ�

			std::set<set<IndexType> > comp_set;

			CFcmp_node.clear();

			CFcmp_node.insert(vtx_id);

			matchNextFrameOnePt(CFrameId ,NFrameId,CFcmp_node,srLevel,tgLevel);

			//�ҵ�����ƥ��ֱ�Ӷ�HLabel�Ķ�Ӧ��ֵ

			NFcmp_node = equeue_.top().tgPatches;

			ScalarType val = equeue_.top().Er;

			if (val > 10)
			{
				Logger<<" û���ҵ����ʵĿ�.\n";
				continue;
			}

			if (NFcmp_node.size() > 1)
			{
				Logger<<"Merge this patches to match label"<<vtx_id<<endl;

			}

			//build patch correspondece		

			IndexType corLabelId = (*NFcmp_node.begin());

			Logger<<CFrameId<<"֡�Ŀ�"<<vtx_id<<"��֡"<<NFrameId<<"�Ŀ�"<<corLabelId<<"��Ӧ.\n";

			HLabel* nextPtPtr = components_[NFrameId].hier_label_bucket[tgLevel][corLabelId]; 

			components_[CFrameId].hier_label_bucket[srLevel][vtx_id]->next_corr = nextPtPtr;

			// 			comp_set.insert(CFcmp_node);
			// 
			// 			unique_pair_set.insert(CFcmp_node);
			// 
			// 			CFrameSetSet_.insert(comp_set.begin() , comp_set.end() ); 
			// 			sadj_map_.insert( std::make_pair(* CFvertex_biter ,CFcmp_node));
			// 			stotal_map_.insert( std::make_pair( * CFvertex_biter , comp_set));
		}

	}

	void matchLevelGraphs(IndexType CFrameId , IndexType NFrameId,IndexType graLevel)
	{
		if( components_.find(CFrameId+1) == components_.end())return;

		HFrame& cur_frame = components_[CFrameId];
		HFrame& next_frame = components_[NFrameId];

		LabelsGraph* labelGraphOfCFrame = cur_frame.hier_graph[graLevel];
		LabelsGraph* labelGraphOfNFrame = next_frame.hier_graph[graLevel];

		//���������
		LabelsGraph::vertex_iterator CFvertex_biter ,CFvertex_eiter, NFvertex_biter,NFvertex_eiter;
		LabelsGraph::adjacency_iterator CFadj_bitr , CFadj_eitr ,NFadj_bitr , NFadj_eitr;

		tie( CFvertex_biter ,CFvertex_eiter) = boost::vertices(* labelGraphOfCFrame);

		for(  ; CFvertex_biter != CFvertex_eiter ; ++CFvertex_biter){
			if(COUT_DEBUG)std::cout<<"�㣺"<<*CFvertex_biter<<std::endl;
			boost::tie(CFadj_bitr , CFadj_eitr) = boost::adjacent_vertices( *CFvertex_biter ,*labelGraphOfCFrame ); // ����ڽӵ�ĵ�����
			std::set<set<IndexType> > unique_pair_set;

			//�����ڵ� ƥ��
			std::set<IndexType> CFcmp_node ,NFcmp_node;  //�ӿڶ��� �����ڴ������ڼ����������Ͻڵ�
			std::set<set<IndexType> > comp_set;

			CFcmp_node.clear();
			CFcmp_node.insert(*CFvertex_biter);

			// ������һ֡��ѭ��
			process_CFcmp_node(CFrameId ,NFrameId ,CFcmp_node ,NFcmp_node ,labelGraphOfNFrame,graLevel);

			comp_set.insert(CFcmp_node);
			unique_pair_set.insert(CFcmp_node);
			CFrameSetSet_.insert(comp_set.begin() , comp_set.end() ); 
			sadj_map_.insert( std::make_pair(* CFvertex_biter ,CFcmp_node));
			stotal_map_.insert( std::make_pair( * CFvertex_biter , comp_set));

			//�����

			// 			if( CFadj_bitr == CFadj_eitr /** ˵���ýڵ�û���ڽӽڵ㣬�����ɿ�**/){
			// 				//ʹ�õ����˽ڵ��Ӧ�ĵ����ݣ���Ѱ�� ��һ֡�е�ƥ��ڵ�
			// 				if(COUT_DEBUG)std::cout<< "�����ڵ�"<<std::endl;
			// 				std::set<set<IndexType> > comp_set;
			// 
			// 				CFcmp_node.clear();
			// 				CFcmp_node.insert(*CFvertex_biter);
			// 
			// 				// ������һ֡��ѭ��
			// 				process_CFcmp_node(CFrameId ,NFrameId ,CFcmp_node ,NFcmp_node ,labelGraphOfNFrame,graLevel);
			// 
			// 				comp_set.insert(CFcmp_node);
			// 				unique_pair_set.insert(CFcmp_node);
			// 				CFrameSetSet_.insert(comp_set.begin() , comp_set.end() ); 
			// 				sadj_map_.insert( std::make_pair(* CFvertex_biter ,CFcmp_node));
			// 				stotal_map_.insert( std::make_pair( * CFvertex_biter , comp_set));
			// 
			// 			}else{ /*�˽ڵ�����ڽӽڵ�*/
			// 				//ʹ�õ����˽ڵ��Ӧ�ĵ����ݣ���Ѱ�� ��һ֡�е�ƥ��ڵ�
			// 				if(COUT_DEBUG)std::cout<< "���������ڵ�"<<std::endl;
			// 
			// 				std::set<set<IndexType> > comp_set;
			// 				std::set<IndexType> cmps_node;
			// 				comp_set.clear();
			// 				cmps_node.clear();
			// 				cmps_node.insert(*CFvertex_biter);   //�����ڵ�
			// 				//if(  comp_set.end() != comp_set.find(cmps_node) ){ /* �����˸��ڵ�*/
			// 				comp_set.insert(cmps_node);
			// 				cmps_node.clear();
			// 
			// 				//}
			// 				//std::stack<set<IndexType> > cmps_node_stack;  //����ջ�����н���ʽ���
			// 				//�˴������ظ��½ڵ�
			// 				std::set<set<IndexType> > ::iterator CFcomsetbitr ,CFcomseteitr;
			// 				for( ; CFadj_bitr != CFadj_eitr ; ++CFadj_bitr ){
			// 					cmps_node.insert( *CFadj_bitr);  // ���ڴ洢�ڵ�
			// 
			// 					if(COUT_DEBUG)std::cout<< "���ڽڵ㣺"<< *CFadj_bitr <<std::endl;
			// 					CFcomsetbitr = comp_set.begin();
			// 					CFcomseteitr = comp_set.end();
			// 					if( CFcomsetbitr == CFcomseteitr /*��ʼΪ��*/ ){
			// 						//������ڵ�
			// 						std::set<IndexType> cmps_node;
			// 						cmps_node.insert(*CFvertex_biter);   
			// 						comp_set.insert(cmps_node);
			// 					}
			// 					if(COUT_DEBUG)std::cout<<"comp_set test:"<< std::endl;
			// 					if(COUT_DEBUG)coutSetSet( comp_set);
			// 					CFcomsetbitr = comp_set.begin();
			// 					CFcomseteitr = comp_set.end();
			// 
			// 					set< set<IndexType> > tmpsetset;
			// 					tmpsetset.clear();
			// 					tmpsetset = comp_set;
			// 					if(COUT_DEBUG)std::cout<<"first tmpsetset test:"<< std::endl;
			// 					if(COUT_DEBUG)coutSetSet( tmpsetset);
			// 					for( ; CFcomsetbitr != CFcomseteitr ; ++ CFcomsetbitr){
			// 
			// 						set<IndexType> new_set = *CFcomsetbitr; 
			// 						new_set.insert( *CFadj_bitr );
			// 						tmpsetset.insert( new_set); 
			// 
			// 						if(COUT_DEBUG)std::cout<<"after tmpsetset test:"<< std::endl;
			// 						if(COUT_DEBUG)coutSetSet( tmpsetset);
			// 					}
			// 					comp_set = tmpsetset;
			// 					if(COUT_DEBUG)std::cout<<"comp_set test:"<< std::endl;
			// 					if(COUT_DEBUG)coutSetSet( comp_set);
			// 				}
			// 				CFrameSetSet_.insert(comp_set.begin() , comp_set.end() );
			// 				sadj_map_.insert( std::make_pair(* CFvertex_biter ,cmps_node));
			// 				stotal_map_.insert( std::make_pair( * CFvertex_biter , comp_set));
			// 				CFcomsetbitr = comp_set.begin();
			// 				CFcomseteitr = comp_set.end();
			// 				if(COUT_DEBUG)std::cout<<"��:"<< *CFvertex_biter <<"���������:" <<std::endl;
			// 				for( ; CFcomsetbitr != CFcomseteitr ; ++ CFcomsetbitr){
			// 					//test
			// 					set<IndexType> testset = * CFcomsetbitr;
			// 					if(COUT_DEBUG)std::cout<< "��ǰ֡�ڵ���� ,��������һ֡�ڵ㴦��";
			// 					if(COUT_DEBUG)coutSet(* CFcomsetbitr);
			// 
			// 					// ������һ֡��ѭ��
			// 					CFcmp_node = *CFcomsetbitr;
			// 					process_CFcmp_node(CFrameId ,NFrameId ,CFcmp_node ,NFcmp_node,labelGraphOfNFrame,graLevel);
			// 
			// 					unique_pair_set.insert(*CFcomsetbitr);
			// 				}
			// 
			// 
			// 			}
		}


	}

	void matchNextFrameOnePt(IndexType CFrameId, IndexType NFrameId, set<IndexType>& CFcmp_node,
		IndexType srGraLevel,IndexType tgGraLevel)
	{

		LabelsGraph::vertex_iterator NFvertex_biter ;
		LabelsGraph::vertex_iterator NFvertex_eiter;
		LabelsGraph::adjacency_iterator  NFadj_bitr ,NFadj_eitr;

		IndexType gLevel = components_[NFrameId].hier_graph.size();

		LabelsGraph* labelGraphOfNFrame = components_[NFrameId].hier_graph[tgGraLevel];

		set<IndexType> NFcmp_node;

		tie( NFvertex_biter ,NFvertex_eiter) = boost::vertices(* labelGraphOfNFrame);

		for(; NFvertex_biter != NFvertex_eiter; ++NFvertex_biter)
		{

			boost::tie( NFadj_bitr , NFadj_eitr) = boost::adjacent_vertices( *NFvertex_biter ,*labelGraphOfNFrame ); // ����ڽӵ�ĵ�����

			std::set<set<IndexType> > comp_set;	
			NFcmp_node.clear();
			NFcmp_node.insert(*NFvertex_biter);
			comp_set.insert(NFcmp_node);
			NFrameSetSet_.insert(comp_set.begin() , comp_set.end() );
			tadj_map_.insert( std::make_pair(* NFvertex_biter ,NFcmp_node));
			ttotal_map_.insert( std::make_pair( * NFvertex_biter , comp_set));

			//ScalarType  error = distance2PatchesLevel( CFrameId, CFcmp_node, NFrameId, NFcmp_node, srGraLevel, tgGraLevel);

			ScalarType error = distBetween2Frame4Items(CFrameId, CFcmp_node, NFrameId, NFcmp_node, srGraLevel, tgGraLevel);

			equeue_.push( PatchMatch( CFcmp_node ,NFcmp_node, error ) );
		}
	}

	void process_CFcmp_node( IndexType CFrameId ,IndexType NFrameId, set<IndexType>& CFcmp_node /* ��ǰ֡����Ͻڵ�*/ ,set<IndexType>& NFcmp_node ,
		LabelsGraph* labelGraphOfNFrame,IndexType graLevel)
	{
		if(COUT_DEBUG)std::cout<<"������һ֡�ڵ㣬�������뵱ǰ�ڵ�"<<std::endl;

		LabelsGraph::vertex_iterator NFvertex_biter ;
		LabelsGraph::vertex_iterator NFvertex_eiter;
		LabelsGraph::adjacency_iterator  NFadj_bitr ,NFadj_eitr;

		tie( NFvertex_biter ,NFvertex_eiter) = boost::vertices(* labelGraphOfNFrame);
		boost::tie( NFadj_bitr , NFadj_eitr) = boost::adjacent_vertices( *NFvertex_biter ,*labelGraphOfNFrame ); // ����ڽӵ�ĵ�����

		for(  ; NFvertex_biter != NFvertex_eiter ; ++NFvertex_biter)
		{
			boost::tie( NFadj_bitr , NFadj_eitr) = boost::adjacent_vertices( *NFvertex_biter ,*labelGraphOfNFrame ); // ����ڽӵ�ĵ�����
			if(COUT_DEBUG)std::cout<<"��һ֡�ڵ㣺"<<*NFvertex_biter<<std::endl;


			//�������ڵ�-----------------------------------------



			//�������ڵ�-----------------------------------------

			if( NFadj_bitr == NFadj_eitr /* ˵���ýڵ�û���ڽӽڵ㣬�����ɿ�*/){

				//ʹ�õ����˽ڵ��Ӧ�ĵ����ݣ���Ѱ�� ��һ֡�е�ƥ��ڵ�
				if(COUT_DEBUG)std::cout<< "�����ڵ�"<<std::endl;
				std::set<set<IndexType> > comp_set;	
				NFcmp_node.clear();
				NFcmp_node.insert(*NFvertex_biter);
				comp_set.insert(NFcmp_node);
				NFrameSetSet_.insert(comp_set.begin() , comp_set.end() );
				tadj_map_.insert( std::make_pair(* NFvertex_biter ,NFcmp_node));
				ttotal_map_.insert( std::make_pair( * NFvertex_biter , comp_set));

				/*   �˴����� ��Ͻڼ� error*/
				ScalarType  error = distance2PatchesLevel( CFrameId  , CFcmp_node,NFrameId, NFcmp_node,graLevel,graLevel);

				if(COUT_DEBUG)std::cout<< "��ǰ֡�ڵ����";
				if(COUT_DEBUG)coutSet(CFcmp_node);
				if(COUT_DEBUG)std::cout<< "��֡�ڵ���ϣ�";
				if(COUT_DEBUG)coutSet(NFcmp_node);
				if(COUT_DEBUG)std::cout<<"֮���errorΪ "<< error<<std::endl;

				equeue_.push( PatchMatch( CFcmp_node ,NFcmp_node, error ) );

				// ������������

			}else{
				/*�˽ڵ�����ڽӽڵ�*/
				//ʹ�õ����˽ڵ��Ӧ�ĵ����ݣ���Ѱ�� ��һ֡�е�ƥ��ڵ�
				if(COUT_DEBUG)std::cout<< "���������ڵ�"<<std::endl;
				std::set<set<IndexType> > comp_set;
				std::set<IndexType> cmps_node;
				comp_set.clear();
				cmps_node.clear();
				cmps_node.insert(*NFvertex_biter);   //
				comp_set.insert( cmps_node);
				cmps_node.clear();
				//std::stack<set<IndexType> > cmps_node_stack;  //����ջ�����н���ʽ���
				//�˴������ظ��½ڵ�
				std::set<set<IndexType> > ::iterator NFcomsetbitr ,NFcomseteitr;
				for( ; NFadj_bitr != NFadj_eitr ; NFadj_bitr++ ){

					if(COUT_DEBUG)std::cout<< "���ڽڵ㣺"<< *NFadj_bitr <<std::endl;

					cmps_node.insert(*NFadj_bitr);
					NFcomsetbitr = comp_set.begin();
					NFcomseteitr = comp_set.end();
					if( NFcomsetbitr == NFcomseteitr /*��ʼΪ��*/ ){
						//������ڵ�
						std::set<IndexType> cmps_node;
						cmps_node.insert(*NFvertex_biter);   
						comp_set.insert(cmps_node);
					}

					NFcomsetbitr = comp_set.begin();
					NFcomseteitr = comp_set.end();
					set< set<IndexType> > tmpsetset;
					tmpsetset.clear();
					tmpsetset = comp_set;


					for( ; NFcomsetbitr != NFcomseteitr ; ++ NFcomsetbitr){
						set<IndexType> new_set = *NFcomsetbitr; 
						new_set.insert( *NFadj_bitr );
						tmpsetset.insert( new_set); 

						if(COUT_DEBUG)std::cout<<"after tmpsetset test:"<< std::endl;
						if(COUT_DEBUG)coutSetSet( tmpsetset);
					}
					comp_set = tmpsetset;  //�ؼ�����

					if(COUT_DEBUG)std::cout<<"�ڵ����:"<< std::endl;
					if(COUT_DEBUG)coutSetSet( comp_set);
				}

				NFrameSetSet_.insert(comp_set.begin() , comp_set.end() );
				tadj_map_.insert( std::make_pair(* NFvertex_biter , cmps_node));
				ttotal_map_.insert( std::make_pair( * NFvertex_biter , comp_set));
				NFcomsetbitr = comp_set.begin();
				NFcomseteitr = comp_set.end();

				if(COUT_DEBUG)std::cout<<"��һ֡�ڵ�:"<< *NFvertex_biter <<"���������:" <<std::endl;
				if(COUT_DEBUG)coutSetSet(comp_set);

				for( ; NFcomsetbitr != NFcomseteitr ; ++ NFcomsetbitr){

					NFcmp_node.clear();
					NFcmp_node = *NFcomsetbitr;


					ScalarType  error = distance2PatchesLevel( CFrameId  , CFcmp_node,NFrameId, NFcmp_node,graLevel,graLevel);

					if(COUT_DEBUG)std::cout<< "��ǰ֡�ڵ����";
					if(COUT_DEBUG)coutSet(CFcmp_node);
					if(COUT_DEBUG)std::cout<< "��֡�ڵ���ϣ�";
					if(COUT_DEBUG)coutSet(NFcmp_node);
					if(COUT_DEBUG)std::cout<<"֮���errorΪ "<< error<<std::endl;

					equeue_.push( PatchMatch( CFcmp_node ,NFcmp_node, error ) );

				}
			}
		}


	}

	void coutFrameNodeLink()
	{
		std::cout<<"��ǰ֡��"<<CFrameId_<<std::endl;
		std::cout<<"���ӹ�ϵ"<<std::endl;
		coutSetSet( CFrameSetSet_);
		std::cout<<"��һ֡��"<<NFrameId_<<std::endl;
		std::cout<<"���ӹ�ϵ"<<std::endl;
		coutSetSet( NFrameSetSet_);

	}


	void coutFrameNodeAdj()
	{
		std::map< IndexType , set<IndexType> >::iterator smap_bitr ,smap_eitr ,tmap_bitr ,tmap_eitr;
		smap_bitr = sadj_map_.begin();
		smap_eitr = sadj_map_.end();
		tmap_bitr = tadj_map_.begin();
		tmap_eitr = tadj_map_.end();
		std::cout<<"��ǰ֡�ţ� "<< CFrameId_<<std::endl;
		for( ; smap_bitr != smap_eitr ;++ smap_bitr){
			std::cout<<"��ǰ֡�� "<< smap_bitr->first<<std::endl;
			std::cout<<" ���ڽӵ�"<<std::endl;
			coutSet(smap_bitr->second);
		}
		std::cout<<"��һ֡�ţ� "<< NFrameId_<<std::endl;
		for( ; tmap_bitr != tmap_eitr; ++ tmap_bitr){
			std::cout<<"��һ֡�� "<< tmap_bitr->first<<std::endl;
			std::cout<<" ���ڽӵ�"<<std::endl;
			coutSet( tmap_bitr->second);

		} 

	}


	void coutFrameNodeAdjCMP()
	{
		std::map< IndexType , set< set<IndexType> > >::iterator smap_bitr ,smap_eitr ,tmap_bitr ,tmap_eitr;
		smap_bitr = stotal_map_.begin();
		smap_eitr = stotal_map_.end();
		tmap_bitr = ttotal_map_.begin();
		tmap_eitr = ttotal_map_.end();
		for( ; smap_bitr != smap_eitr ;++ smap_bitr){
			std::cout<<"��ǰ֡�� "<< smap_bitr->first<<std::endl;
			std::cout<<" ���ڽӵ����"<<std::endl;
			coutSetSet( smap_bitr->second);
		}
		for( ; tmap_bitr != tmap_eitr; ++ tmap_bitr){
			std::cout<<"��һ֡�� "<< tmap_bitr->first<<std::endl;
			std::cout<<" ���ڽӵ����"<<std::endl;
			coutSetSet( tmap_bitr->second);

		};


	}

	void coutPatchMatch( IndexType _prioNum)
	{
		std::cout<<"��ǰ֡�ţ� "<< CFrameId_<<std::endl;
		for( IndexType prioNum = _prioNum ; prioNum > 0 ; --prioNum){
			PatchMatch  pm = equeue_.top();
			equeue_.pop();
			std::cout<<"���ȼ�"<<prioNum<<"-->";
			std::cout<<"Դ��";
			coutSet( pm.srPatches);
			std::cout<<"Ŀ�꣺";
			coutSet( pm.tgPatches);
			std::cout<<"error��";
			std::cout<< pm.Er<<std::endl;	 		 
		}

	}

	ScalarType distBetween2Frame4Items(IndexType sFrame, const set<IndexType>& sPatches,
		IndexType tFrame, const set<IndexType>& tPatches,IndexType srLevel, IndexType tgLevel)
	{
		assert(sPatches.size() > 0 && tPatches.size() > 0);

		map<IndexType,HVertex*> srVertex;

		set<IndexType>::iterator sIter = sPatches.begin();

		for (; sIter != sPatches.end(); sIter ++)
		{
			HLabel* temp = components_[sFrame].hier_label_bucket[srLevel][*sIter];

			srVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
		}

		map<IndexType,HVertex*> tgVertex;

		set<IndexType>::iterator tIter = tPatches.begin();

		for (; tIter != tPatches.end(); tIter++)
		{
			HLabel* temp = components_[tFrame].hier_label_bucket[tgLevel][*tIter];
			tgVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
		}

		map<IndexType,HVertex*> toComVtx; // ��¼��Щ�����ǵĶ�Ӧ�����ڶ�Ӧ����

		map<IndexType,HVertex*> backComVtx; // �໥��¼

		bool isTo = true;
		computer_common(srVertex,tgVertex,toComVtx,isTo);

		isTo = false;
		computer_common(srVertex, tgVertex,backComVtx,isTo);


		ScalarType srV_size = srVertex.size();
		ScalarType tgV_size = tgVertex.size();

		IndexType toCom_size = toComVtx.size();
		IndexType backCom_size = backComVtx.size();

		ScalarType corValue =  0.5 * (toCom_size/srV_size + backCom_size/tgV_size );

		ScalarType scaleShapeValue =  abs( srV_size - tgV_size) / max(srV_size, tgV_size );

		ScalarType deformationValue  = deformationDis(toComVtx,backComVtx,sFrame,tFrame);

		if ( scaleShapeValue > 0.5 || corValue < 0.5)
		{
			return INF;
		}else
		{
			return  1. - corValue;
		}
	}

	ScalarType deformationDis(map<IndexType,HVertex*>& toComVtx, map<IndexType,HVertex*>& backComVtx, IndexType sFrame, IndexType tFrame)
	{
		//
		Sample& srFrame = sample_set_[sFrame];
		Sample& tgFrame = sample_set_[tFrame];

		IndexType toPsSize = toComVtx.size();
		IndexType backPsSize = backComVtx.size();

		Matrix3X s_coord, t_coord;
		s_coord.setZero(3, toPsSize);
		t_coord.setZero(3, backPsSize);

		IndexType i = 0;
		for (auto toIter = toComVtx.begin(); toIter != toComVtx.end(); ++toIter,++i)
		{

			IndexType sPsId = toIter->first;
			IndexType corPsId = toIter->second->next_corr->vtx_id;

			s_coord.col(i) = srFrame.vertices_matrix().col(sPsId);
			t_coord.col(i) = tgFrame.vertices_matrix().col(corPsId);

		}

		Matrix33 rot_mat;
		MatrixXX tran_vec;

		point2point(s_coord, t_coord, rot_mat, tran_vec);

		//����ÿ���㵽��Ӧ�����
		MatrixXX traSCoor = rot_mat * s_coord;

		traSCoor.colwise() += tran_vec.col(0);

		MatrixXX errMat = t_coord - traSCoor;

		ScalarType totDis = errMat.colwise().norm().sum();


		return totDis/i;

	}

	void point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat,MatrixXX & transVec)
	{
		Eigen::Vector3f X_mean, Y_mean;

		for(int i=0; i<3; ++i) //���������Ƶľ�ֵ
		{
			X_mean(i) = srCloud.row(i).sum()/srCloud.cols();
			Y_mean(i) = tgCloud.row(i).sum()/tgCloud.cols();
		}

		srCloud.colwise() -= X_mean;
		tgCloud.colwise() -= Y_mean;

		/// Compute transformation
		Eigen::Affine3f transformation;
		Eigen::Matrix3f sigma = srCloud * tgCloud.transpose();
		Eigen::JacobiSVD<Eigen::Matrix3f> svd(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
		if(svd.matrixU().determinant()*svd.matrixV().determinant() < 0.0)//contains reflection
		{
			Eigen::Vector3f S = Eigen::Vector3f::Ones(); S(2) = -1.0;
			transformation.linear().noalias() = svd.matrixV()*S.asDiagonal()*svd.matrixU().transpose();
		} else 
		{
			transformation.linear().noalias() = svd.matrixV()*svd.matrixU().transpose();//������ת����
		}

		transVec = Y_mean - transformation.linear()*X_mean;
		rotMat = transformation.linear() ;

		srCloud.colwise() += X_mean;
		tgCloud.colwise() += Y_mean;

	}

	ScalarType distance2PatchesLevel(IndexType sFrame, const set<IndexType>& sPatches,
		IndexType tFrame, const set<IndexType>& tPatches,IndexType srLevel, IndexType tgLevel)
	{

		assert(sPatches.size() > 0 && tPatches.size() > 0);

		map<IndexType,HVertex*> srVertex;

		set<IndexType>::iterator sIter = sPatches.begin();

		for (; sIter != sPatches.end(); sIter ++)
		{
			HLabel* temp = components_[sFrame].hier_label_bucket[srLevel][*sIter];

			srVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
		}

		map<IndexType,HVertex*> tgVertex;

		set<IndexType>::iterator tIter = tPatches.begin();

		for (; tIter != tPatches.end(); tIter++)
		{
			HLabel* temp = components_[tFrame].hier_label_bucket[tgLevel][*tIter];
			tgVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
		}

		map<IndexType,HVertex*> toComVtx; // ��¼��Щ�����ǵĶ�Ӧ�����ڶ�Ӧ����

		map<IndexType,HVertex*> backComVtx; // �໥��¼

		bool isTo = true;
		computer_common(srVertex,tgVertex,toComVtx,isTo);

		isTo = false;
		computer_common(srVertex, tgVertex,backComVtx,isTo);


		//ScalarType corValue =  0.5 * (toComVtx.size()/srVertex.size() + backComVtx.size()/tgVertex.size() );

		//ScalarType scaleShape =  abs((int) (srVertex.size() - tgVertex.size() )) / std::max(srVertex.size(), tgVertex.size() );

		ScalarType srV_size = srVertex.size();
		ScalarType tgV_size = tgVertex.size();

		IndexType toCom_size = toComVtx.size();
		IndexType backCom_size = backComVtx.size();

		ScalarType corValue =  0.5 * (toCom_size/srV_size + backCom_size/tgV_size );

		ScalarType scaleShape =  abs( srV_size - tgV_size) / max(srV_size, tgV_size );

		if ( scaleShape > 0.5 || corValue < 0.5)
		{
			return INF;
		}else
		{
			return  1. - corValue;
		}

	}

	// 	ScalarType distance_between_patch(IndexType sFrame, const set<IndexType>& sPatches,
	// 		IndexType tFrame, const set<IndexType>& tPatches)
	// 	{
	// 		if(COUT_DEBUG)std::cout<< "�������"<<std::endl;
	// 		if(COUT_DEBUG)std::cout<<"Spatches:";
	// 		if(COUT_DEBUG)coutSet(sPatches);
	// 		if(COUT_DEBUG)std::cout<<"Tpatches:";
	// 		if(COUT_DEBUG)coutSet(tPatches);
	// 
	// 		assert(sPatches.size() > 0 && tPatches.size() > 0);
	// 
	// 		map<IndexType,CVertex*> srVertex;
	// 
	// 		set<IndexType>::iterator sIter = sPatches.begin();
	// 
	// 		for (; sIter != sPatches.end(); sIter ++)
	// 		{
	// 			CLabel* temp = components_[sFrame].label_bucket[*sIter];
	// 			srVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
	// 		}
	// 
	// 		map<IndexType,CVertex*> tgVertex;
	// 
	// 		set<IndexType>::iterator tIter = tPatches.begin();
	// 
	// 		for (; tIter != tPatches.end(); tIter++)
	// 		{
	// 			CLabel* temp = components_[tFrame].label_bucket[*tIter];
	// 			tgVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
	// 		}
	// 
	// 		map<IndexType,CVertex*> toComVtx; // ��¼��Щ�����ǵĶ�Ӧ�����ڶ�Ӧ����
	// 
	// 		map<IndexType,CVertex*> backComVtx; // �໥��¼
	// 
	// 		bool isTo = true;
	// 		computer_common(srVertex,tgVertex,toComVtx,isTo);
	// 
	// 		isTo = false;
	// 		computer_common(srVertex, tgVertex,backComVtx,isTo);
	// 
	// 
	// 		//ScalarType corValue =  0.5 * (toComVtx.size()/srVertex.size() + backComVtx.size()/tgVertex.size() );
	// 
	// 		//ScalarType scaleShape =  abs((int) (srVertex.size() - tgVertex.size() )) / std::max(srVertex.size(), tgVertex.size() );
	// 		ScalarType srV_size = srVertex.size();
	// 		ScalarType tgV_size = tgVertex.size();
	// 
	// 		IndexType toCom_size = toComVtx.size();
	// 		IndexType backCom_size = backComVtx.size();
	// 
	// 		ScalarType corValue =  0.5 * (toCom_size/srV_size + backCom_size/tgV_size );
	// 
	// 		ScalarType scaleShape =  abs( srV_size - tgV_size) / max(srV_size, tgV_size );
	// 
	// 		if ( scaleShape > 0.5 || corValue < 0.5)
	// 		{
	// 			return INF;
	// 		}else
	// 		{
	// 			return  1. - corValue;
	// 		}
	// 
	// 		// the distances should includes two items Jarcard and deformation distortion
	// 
	// 
	// 
	// 	}

	void computer_common(map<IndexType,HVertex*>& srPatchesVtx, map<IndexType,HVertex*>& tgPatchesVtx, 
		map<IndexType,HVertex*>& comVtx,bool isTo)
	{


		if (isTo) // -->
		{
			map<IndexType,HVertex*>::iterator iter = srPatchesVtx.begin();

			for (; iter != srPatchesVtx.end(); iter ++)
			{
				if( tgPatchesVtx.find( iter->second->next_corr->vtx_id) != tgPatchesVtx.end())
				{
					comVtx.insert(*iter);
				}

			}

		}else// <--
		{
			map<IndexType,HVertex*>::iterator iter = tgPatchesVtx.begin();

			for (; iter != tgPatchesVtx.end(); iter++)
			{
				if ( srPatchesVtx.find(iter->second->prev_corr->vtx_id) != srPatchesVtx.end())
				{
					comVtx.insert(*iter);
				}
			}
		}

		return;
	}

public:
	void coutSet ( const std::set<IndexType>& _in) const{
		set<IndexType>::iterator bitr, eitr;
		bitr = _in.begin();
		eitr = _in.end();
		std::cout<<"set:";
		for(  ; bitr != eitr ; ++bitr){
			std::cout<<"-"<<* bitr;
		}
		std::cout<<std::endl;
	}
	void coutSetSet( const std::set< set<IndexType> >& _in) const{
		std::set< set<IndexType> >::iterator bitr , eitr;
		bitr = _in.begin();eitr = _in.end();
		for(  ; bitr != eitr ; ++bitr){
			coutSet( *bitr);
		}


	}
private:

	SampleSet& sample_set_;
	map<IndexType ,HFrame>& components_;


	IndexType CFrameId_;
	IndexType NFrameId_;
	set< set<IndexType> > CFrameSetSet_;
	set< set<IndexType> > NFrameSetSet_;

	std::map< IndexType , set< set<IndexType> > >  stotal_map_;
	std::map< IndexType , set< set<IndexType> > >  ttotal_map_;
	std::map< IndexType , set< IndexType > >  sadj_map_;
	std::map< IndexType , set< IndexType>  >  tadj_map_;

	typedef priority_queue< PatchMatch, vector<PatchMatch>, patchmatch_error_greater_than> ERQueue;
	ERQueue equeue_;
	//PoolAllocator allocator_;


};





#endif