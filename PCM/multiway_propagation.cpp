#include"multiway_propagation.h"
#include "InOutputDefinition.h"
#include <QTimer>
#include <QObject>
#include <stack>
#define  INF 1000000
#define  COUT_DEBUG 0

#define frame_index_to_key(f,i) ((f<<16)|i)
#define frame_label_to_key(f,l) ((f<<16)|l)
#define get_index_from_key(k) (k&0xffff)
#define get_frame_from_key(k) (k>>16)


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
	FrameWithNodeCaculation(SampleSet& _sample_set , map<IndexType ,CFrame>& _components ,
		map<IndexType ,IndexType>& _frame_index_map_label  ,IndexType _CframeId ,IndexType _NframeId)
		:sample_set_(_sample_set),components_ (  _components),
		frame_index_map_label_(_frame_index_map_label) ,CFrameId_(_CframeId),NFrameId_(_NframeId)
	{
		
		 
	}

	std::set< set<IndexType> >  getLatenteSet(IndexType CFrameId , const std::set<IndexType>& slabel_set , IndexType NFrameId){ 
		std::set< set<IndexType> > L;
		return L;}
	//ScalarType distance_between_patch(IndexType sFrame , std::set<IndexType>& sPatches,
		//IndexType tFrame , std::set<IndexType>& tPatches);

	void compute(IndexType CFrameId , IndexType NFrameId)
	{
		if( components_.find(CFrameId+1) == components_.end())return;
		CFrame& cur_frame = components_[CFrameId];
		CFrame& next_frame = components_[CFrameId+1];
		LabelsGraph* labelGraphOfCFrame = cur_frame.labelGraph;
		LabelsGraph* labelGraphOfNFrame = next_frame.labelGraph;
		//点遍历方法
		LabelsGraph::vertex_iterator CFvertex_biter ,CFvertex_eiter, NFvertex_biter,NFvertex_eiter;
		LabelsGraph::adjacency_iterator CFadj_bitr , CFadj_eitr ,NFadj_bitr , NFadj_eitr;
		//typedef boost::graph_traits< LabelsGraph>::vertex_iterator vertex_iter;
		//typedef boost::graph_traits< LabelsGraph>::vertex_iterator NFvertex_iter;
		tie( CFvertex_biter ,CFvertex_eiter) = boost::vertices(* labelGraphOfCFrame);
		
		//for( vertex_iter itr = vrange.first; itr != vrange.second ; ++itr){}
		//std::cout<<*itr<<std::endl;
		//边遍历方法
		//typedef boost::graph_traits<LabelsGraph>::edge_iterator edge_iter;
		//std::pair<edge_iter ,edge_iter> erange = boost::edges(*labelGraphOfCFrame);
		//for( edge_iter itr = erange.first; itr!= erange.second ;++itr){}
		//std::cout<< boost::source(*itr ,*labelGraphOfCFrame)<<"-->"<<target(*itr ,*labelGraphOfCFrame)<<std::endl;
		//求点的邻接点		  
		for(  ; CFvertex_biter != CFvertex_eiter ; ++CFvertex_biter){
			if(COUT_DEBUG)std::cout<<"点："<<*CFvertex_biter<<std::endl;
			//std::cout<<"adjacent vertices"<<std::endl;

			
			
			//boost::graph_traits<LabelsGraph>::adjacency_iterator ai ;
			//boost::graph_traits<LabelsGraph>::adjacency_iterator ai_end;
			boost::tie(CFadj_bitr , CFadj_eitr) = boost::adjacent_vertices( *CFvertex_biter ,*labelGraphOfCFrame ); // 获得邻接点的迭代器
			std::set<set<IndexType> > unique_pair_set;
			
			std::set<IndexType> CFcmp_node ,NFcmp_node;  //接口对象 ，用于存入用于计算的两个组合节点
			if( CFadj_bitr == CFadj_eitr /** 说明该节点没有邻接节点，独立成块**/){
				//使用单独此节点对应的点数据，并寻找 下一帧中的匹配节点
				if(COUT_DEBUG)std::cout<< "单独节点"<<std::endl;
				std::set<set<IndexType> > comp_set;
				//std::set<IndexType> cmps_node;
				CFcmp_node.clear();
				CFcmp_node.insert(*CFvertex_biter);
				 
				// 插入下一帧的循环
			    process_CFcmp_node(CFrameId ,NFrameId ,CFcmp_node ,NFcmp_node ,NFvertex_biter ,NFvertex_eiter,labelGraphOfNFrame);

				//std::set< set<IndexType> > latente_set = getLatenteSet(CFrameId , cmps_node ,NFrameId);
				//std::set< set<IndexType> >::iterator bitr ,eitr;
				/*
				for( bitr = latente_set.begin() ; bitr != eitr ;++ bitr){

					ScalarType  error = distance_between_patch( CFrameId  , cmps_node,
						NFrameId, *bitr);

				}
				*/
				comp_set.insert(CFcmp_node);
				unique_pair_set.insert(CFcmp_node);
				CFrameSetSet_.insert(comp_set.begin() , comp_set.end() ); 
				sadj_map_.insert( std::make_pair(* CFvertex_biter ,CFcmp_node));
				stotal_map_.insert( std::make_pair( * CFvertex_biter , comp_set));

			}else{ /*此节点存在邻接节点*/
				//使用单独此节点对应的点数据，并寻找 下一帧中的匹配节点
				if(COUT_DEBUG)std::cout<< "存在其他节点"<<std::endl;

				std::set<set<IndexType> > comp_set;
				std::set<IndexType> cmps_node;
				comp_set.clear();
			    cmps_node.clear();
				cmps_node.insert(*CFvertex_biter);   //单独节点
				//if(  comp_set.end() != comp_set.find(cmps_node) ){ /* 插入了根节点*/
			    comp_set.insert(cmps_node);
				cmps_node.clear();


				//}
				//std::stack<set<IndexType> > cmps_node_stack;  //引入栈来进行渐进式组合
				//此处迭代地更新节点
				std::set<set<IndexType> > ::iterator CFcomsetbitr ,CFcomseteitr;
			    for( ; CFadj_bitr != CFadj_eitr ; ++CFadj_bitr ){
					cmps_node.insert( *CFadj_bitr);  // 用于存储邻点

					if(COUT_DEBUG)std::cout<< "相邻节点："<< *CFadj_bitr <<std::endl;
			 		CFcomsetbitr = comp_set.begin();
			 		CFcomseteitr = comp_set.end();
					if( CFcomsetbitr == CFcomseteitr /*初始为空*/ ){
						//插入根节点
						std::set<IndexType> cmps_node;
						cmps_node.insert(*CFvertex_biter);   
						comp_set.insert(cmps_node);
					}
					if(COUT_DEBUG)std::cout<<"comp_set test:"<< std::endl;
					if(COUT_DEBUG)coutSetSet( comp_set);
					CFcomsetbitr = comp_set.begin();
					CFcomseteitr = comp_set.end();

					set< set<IndexType> > tmpsetset;
					tmpsetset.clear();
					tmpsetset = comp_set;
					if(COUT_DEBUG)std::cout<<"first tmpsetset test:"<< std::endl;
					if(COUT_DEBUG)coutSetSet( tmpsetset);
			 		for( ; CFcomsetbitr != CFcomseteitr ; ++ CFcomsetbitr){
						
						
						
			 			set<IndexType> new_set = *CFcomsetbitr; 
			 			new_set.insert( *CFadj_bitr );
						tmpsetset.insert( new_set); 
		 				
						if(COUT_DEBUG)std::cout<<"after tmpsetset test:"<< std::endl;
						if(COUT_DEBUG)coutSetSet( tmpsetset);
		 			}
					comp_set = tmpsetset;
					if(COUT_DEBUG)std::cout<<"comp_set test:"<< std::endl;
					if(COUT_DEBUG)coutSetSet( comp_set);
     			}
				CFrameSetSet_.insert(comp_set.begin() , comp_set.end() );
				sadj_map_.insert( std::make_pair(* CFvertex_biter ,cmps_node));
				stotal_map_.insert( std::make_pair( * CFvertex_biter , comp_set));
				CFcomsetbitr = comp_set.begin();
				CFcomseteitr = comp_set.end();
				if(COUT_DEBUG)std::cout<<"点:"<< *CFvertex_biter <<"的所有组合:" <<std::endl;
				for( ; CFcomsetbitr != CFcomseteitr ; ++ CFcomsetbitr){
					//test
					set<IndexType> testset = * CFcomsetbitr;
					if(COUT_DEBUG)std::cout<< "当前帧节点组合 ,并进入下一帧节点处理：";
					if(COUT_DEBUG)coutSet(* CFcomsetbitr);
					
					// 插入下一帧的循环
					CFcmp_node = *CFcomsetbitr;
				    process_CFcmp_node(CFrameId ,NFrameId ,CFcmp_node ,NFcmp_node ,NFvertex_biter ,NFvertex_eiter,labelGraphOfNFrame);


					//std::set< set<IndexType> > latente_set = getLatenteSet(CFrameId , *bitr ,NFrameId);
					//std::set< set<IndexType> >::iterator Tbitr ,Teitr;
					//for( Tbitr = latente_set.begin() ; Tbitr !=Teitr ;++ Tbitr){

						//ScalarType  error = distance_between_patch( CFrameId  , *bitr,
						//	NFrameId, *Tbitr);

					//}
					unique_pair_set.insert(*CFcomsetbitr);
				}

				
			}


			/*

					//双节点
					if(0 == cmps_node_stack.size() ){
						cmps_node.insert(*itr);   

					}else{
						std::stack<set<IndexType> > tmp_stack;
						while(0 != cmps_node_stack.size() ){

							cmps_node.insert(*itr);   
							tmp_stack.push(cmps_node);

							std::set<IndexType> cmps_node_to_process = cmps_node_stack.top();
							cmps_node_to_process.insert( *ai);
							cmps_node_stack.pop();
							tmp_stack.push(cmps_node_to_process);
							//std::set<IndexType> new_cmps_node = cmps_node_to_process;

						}
						while( 0 != tmp_stack.size() ){ /* 更新cmps_node_stack */  
			/*				cmps_node_stack.push( tmp_stack.top());
							tmp_stack.pop();
						}





					}
					//cmps_node.insert(*itr); 
					//cmps_node.insert(*ai); 
					//if( unique_pair_set.end() != unique_pair_set.find(cmps_node) ){
					//	unique_pair_set.insert(cmps_node);
					//}
					cmps_node.clear();

					//构造组合节点，并寻找下一帧中的匹配节点
				

			}
			for( boost::tie(ai , ai_end) = boost::adjacent_vertices( *itr ,*labelGraphOfCFrame ) ; ai != ai_end;++ai){
				std::cout<< *ai <<std::endl;
			}
			*/

	   }
	}

	void process_CFcmp_node( IndexType CFrameId ,IndexType NFrameId, set<IndexType>& CFcmp_node /* 当前帧的组合节点*/ ,set<IndexType>& NFcmp_node ,LabelsGraph::vertex_iterator& NFvertex_biter ,LabelsGraph::vertex_iterator& NFvertex_eiter , LabelsGraph* labelGraphOfNFrame)
	{
		if(COUT_DEBUG)std::cout<<"处理下一帧节点，并计算与当前节点"<<std::endl;
		LabelsGraph::adjacency_iterator  NFadj_bitr ,NFadj_eitr;
		tie( NFvertex_biter ,NFvertex_eiter) = boost::vertices(* labelGraphOfNFrame);
		boost::tie( NFadj_bitr , NFadj_eitr) = boost::adjacent_vertices( *NFvertex_biter ,*labelGraphOfNFrame ); // 获得邻接点的迭代器
		
		for(  ; NFvertex_biter != NFvertex_eiter ; ++NFvertex_biter){
			boost::tie( NFadj_bitr , NFadj_eitr) = boost::adjacent_vertices( *NFvertex_biter ,*labelGraphOfNFrame ); // 获得邻接点的迭代器
			if(COUT_DEBUG)std::cout<<"下一帧节点："<<*NFvertex_biter<<std::endl;

			if( NFadj_bitr == NFadj_eitr /* 说明该节点没有邻接节点，独立成块*/){
				 
				//使用单独此节点对应的点数据，并寻找 下一帧中的匹配节点
				if(COUT_DEBUG)std::cout<< "单独节点"<<std::endl;
				std::set<set<IndexType> > comp_set;	
				NFcmp_node.clear();
				NFcmp_node.insert(*NFvertex_biter);
				comp_set.insert(NFcmp_node);
				NFrameSetSet_.insert(comp_set.begin() , comp_set.end() );
				tadj_map_.insert( std::make_pair(* NFvertex_biter ,NFcmp_node));
				ttotal_map_.insert( std::make_pair( * NFvertex_biter , comp_set));
				/*   此处计算 组合节间 error*/
				ScalarType  error = distance_between_patch( CFrameId  , CFcmp_node,
					NFrameId, NFcmp_node);
				if(COUT_DEBUG)std::cout<< "当前帧节点组合";
				if(COUT_DEBUG)coutSet(CFcmp_node);
				if(COUT_DEBUG)std::cout<< "下帧节点组合：";
				if(COUT_DEBUG)coutSet(NFcmp_node);
				if(COUT_DEBUG)std::cout<<"之间的error为 "<< error<<std::endl;

				equeue_.push( PatchMatch( CFcmp_node ,NFcmp_node, error ) );
				// 其他容器操作

			}else{
				/*此节点存在邻接节点*/
				//使用单独此节点对应的点数据，并寻找 下一帧中的匹配节点
				if(COUT_DEBUG)std::cout<< "存在其他节点"<<std::endl;
				std::set<set<IndexType> > comp_set;
				std::set<IndexType> cmps_node;
				comp_set.clear();
				cmps_node.clear();
				cmps_node.insert(*NFvertex_biter);   //
				comp_set.insert( cmps_node);
				cmps_node.clear();
				//std::stack<set<IndexType> > cmps_node_stack;  //引入栈来进行渐进式组合
				//此处迭代地更新节点
				std::set<set<IndexType> > ::iterator NFcomsetbitr ,NFcomseteitr;
				for( ; NFadj_bitr != NFadj_eitr ; NFadj_bitr++ ){
					if(COUT_DEBUG)std::cout<< "相邻节点："<< *NFadj_bitr <<std::endl;
					cmps_node.insert(*NFadj_bitr);
					NFcomsetbitr = comp_set.begin();
					NFcomseteitr = comp_set.end();
					if( NFcomsetbitr == NFcomseteitr /*初始为空*/ ){
						//插入根节点
						std::set<IndexType> cmps_node;
						cmps_node.insert(*NFvertex_biter);   
						comp_set.insert(cmps_node);
					}
					//std::cout<<"comp_set test:"<< std::endl;
					//coutSetSet( comp_set);
					NFcomsetbitr = comp_set.begin();
					NFcomseteitr = comp_set.end();
					set< set<IndexType> > tmpsetset;
					tmpsetset.clear();
					tmpsetset = comp_set;
					//std::cout<<"first tmpsetset test:"<< std::endl;
					//coutSetSet( tmpsetset); 

					for( ; NFcomsetbitr != NFcomseteitr ; ++ NFcomsetbitr){
						//cmps_node.insert(*NFcomsetbitr);
						set<IndexType> new_set = *NFcomsetbitr; 
						new_set.insert( *NFadj_bitr );
						tmpsetset.insert( new_set); 

						if(COUT_DEBUG)std::cout<<"after tmpsetset test:"<< std::endl;
						if(COUT_DEBUG)coutSetSet( tmpsetset);
					}
					comp_set = tmpsetset;  //关键步骤
					if(COUT_DEBUG)std::cout<<"节点组合:"<< std::endl;
					if(COUT_DEBUG)coutSetSet( comp_set);
				}
				NFrameSetSet_.insert(comp_set.begin() , comp_set.end() );
				tadj_map_.insert( std::make_pair(* NFvertex_biter , cmps_node));
				ttotal_map_.insert( std::make_pair( * NFvertex_biter , comp_set));
				NFcomsetbitr = comp_set.begin();
				NFcomseteitr = comp_set.end();
				if(COUT_DEBUG)std::cout<<"下一帧节点:"<< *NFvertex_biter <<"的所有组合:" <<std::endl;
				if(COUT_DEBUG)coutSetSet(comp_set);
				for( ; NFcomsetbitr != NFcomseteitr ; ++ NFcomsetbitr){
					//std::cout<< "组合：";
					//coutSet( *NFcomsetbitr);
					NFcmp_node.clear();
					NFcmp_node = *NFcomsetbitr;
					//test
	 
					//std::cout<< "组合：";
					//coutSet( *NFcomsetbitr);
				 
					/*   此处计算 组合节间 error*/
					
					ScalarType  error = distance_between_patch( CFrameId  , CFcmp_node,
						NFrameId, NFcmp_node);
					if(COUT_DEBUG)std::cout<< "当前帧节点组合";
					if(COUT_DEBUG)coutSet(CFcmp_node);
					if(COUT_DEBUG)std::cout<< "下帧节点组合：";
					if(COUT_DEBUG)coutSet(NFcmp_node);
					if(COUT_DEBUG)std::cout<<"之间的error为 "<< error<<std::endl;
					
					equeue_.push( PatchMatch( CFcmp_node ,NFcmp_node, error ) );
					
				}
			}
		}


	}

	void coutFrameNodeLink()
	{
		std::cout<<"当前帧："<<CFrameId_<<std::endl;
		std::cout<<"连接关系"<<std::endl;
		coutSetSet( CFrameSetSet_);
		std::cout<<"下一帧："<<NFrameId_<<std::endl;
		std::cout<<"连接关系"<<std::endl;
		coutSetSet( NFrameSetSet_);

	}


	void coutFrameNodeAdj()
	{
		std::map< IndexType , set<IndexType> >::iterator smap_bitr ,smap_eitr ,tmap_bitr ,tmap_eitr;
		smap_bitr = sadj_map_.begin();
		smap_eitr = sadj_map_.end();
		tmap_bitr = tadj_map_.begin();
		tmap_eitr = tadj_map_.end();
		std::cout<<"当前帧号： "<< CFrameId_<<std::endl;
		for( ; smap_bitr != smap_eitr ;++ smap_bitr){
			std::cout<<"当前帧点 "<< smap_bitr->first<<std::endl;
			std::cout<<" 的邻接点"<<std::endl;
			coutSet(smap_bitr->second);
		}
		std::cout<<"下一帧号： "<< NFrameId_<<std::endl;
		for( ; tmap_bitr != tmap_eitr; ++ tmap_bitr){
			std::cout<<"下一帧点 "<< tmap_bitr->first<<std::endl;
			std::cout<<" 的邻接点"<<std::endl;
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
			std::cout<<"当前帧点 "<< smap_bitr->first<<std::endl;
			std::cout<<" 的邻接点组合"<<std::endl;
			coutSetSet( smap_bitr->second);
		}
		for( ; tmap_bitr != tmap_eitr; ++ tmap_bitr){
			std::cout<<"下一帧点 "<< tmap_bitr->first<<std::endl;
			std::cout<<" 的邻接点组合"<<std::endl;
			coutSetSet( tmap_bitr->second);

		};
		 

	}
	
	void coutPatchMatch( IndexType _prioNum)
	{
		std::cout<<"当前帧号： "<< CFrameId_<<std::endl;
		for( IndexType prioNum = _prioNum ; prioNum > 0 ; --prioNum){
			PatchMatch  pm = equeue_.top();
			equeue_.pop();
			std::cout<<"优先级"<<prioNum<<"-->";
			std::cout<<"源：";
			coutSet( pm.srPatches);
			std::cout<<"目标：";
			coutSet( pm.tgPatches);
			std::cout<<"error：";
			std::cout<< pm.Er<<std::endl;	 		 
		}

	}
	ScalarType distance_between_patch(IndexType sFrame, const set<IndexType>& sPatches,
		IndexType tFrame, const set<IndexType>& tPatches)
	{
		if(COUT_DEBUG)std::cout<< "计算距离"<<std::endl;
		if(COUT_DEBUG)std::cout<<"Spatches:";
		if(COUT_DEBUG)coutSet(sPatches);
		if(COUT_DEBUG)std::cout<<"Tpatches:";
		if(COUT_DEBUG)coutSet(tPatches);

		assert(sPatches.size() > 0 && tPatches.size() > 0);

		map<IndexType,CVertex*> srVertex;

		set<IndexType>::iterator sIter = sPatches.begin();

		for (; sIter != sPatches.end(); sIter ++)
		{
			CLabel* temp = components_[sFrame].label_bucket[*sIter];
			srVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
		}

		map<IndexType,CVertex*> tgVertex;

		set<IndexType>::iterator tIter = tPatches.begin();

		for (; tIter != tPatches.end(); tIter++)
		{
			CLabel* temp = components_[tFrame].label_bucket[*tIter];
			tgVertex.insert(temp->vertex_bucket.begin(), temp->vertex_bucket.end() );
		}

		map<IndexType,CVertex*> toComVtx; // 记录那些点它们的对应会落在对应块上

		map<IndexType,CVertex*> backComVtx; // 相互记录

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

		// the distances should includes two items Jarcard and deformation distortion



	}
	void computer_common(map<IndexType,CVertex*>& srPatchesVtx, map<IndexType,CVertex*>& tgPatchesVtx, 
		map<IndexType,CVertex*>& comVtx,bool isTo)
	{


		if (isTo) // -->
		{
			map<IndexType,CVertex*>::iterator iter = srPatchesVtx.begin();

			for (; iter != srPatchesVtx.end(); iter ++)
			{
				if( tgPatchesVtx.find( iter->second->next_corr->vtx_id) != tgPatchesVtx.end())
				{
					comVtx.insert(*iter);
				}

			}

		}else// <--
		{
			map<IndexType,CVertex*>::iterator iter = tgPatchesVtx.begin();

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

private:
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
	map<IndexType ,CFrame>&components_;
	map<IndexType ,IndexType>& frame_index_map_label_;
	IndexType CFrameId_;
	IndexType NFrameId_;
	set< set<IndexType> > CFrameSetSet_;
	set< set<IndexType> > NFrameSetSet_;
	set< set<IndexType> > unique_setset;
	
	std::map< IndexType , set< set<IndexType> > >  stotal_map_;
	std::map< IndexType , set< set<IndexType> > >  ttotal_map_;
	std::map< IndexType , set< IndexType > >  sadj_map_;
	std::map< IndexType , set< IndexType>  >  tadj_map_;
	
	typedef priority_queue< PatchMatch, vector<PatchMatch>, patchmatch_error_greater_than> ERQueue;
	ERQueue equeue_;
	//PoolAllocator allocator_;
 

};

class UtilityClass
{
public:

	UtilityClass( DualwayPropagation& _duawayObj )
	: duawayObj_( _duawayObj) ,SMOOTH_AREA_(20) ,duawayComponent_(_duawayObj.hier_componets_){}

	set<IndexType> mergePatches(IndexType _frameId , set<IndexType>& patches)
	{
		depth_ = 0;
		HFrame& cframe = duawayComponent_[_frameId];
		vector<LabelsGraph*>& chierGraph = cframe.hier_graph;
		map<IndexType ,IndexType>& labelofvtx = cframe.label_of_vtx;   //初始 labelofvtx
		LabelsGraph* p =new LabelsGraph( *( chierGraph[depth_]) );  //这里拷贝一个新的         //初始连接图
		LabelsGraph& oriGra = *p;

		vector< vector<HLabel*> >& chierlabucket =  cframe.hier_label_bucket;   //
		assert( chierlabucket .size() > depth_);
		vector<HLabel*>* p_t;
		p_t = DeepCopyHLableVec( &chierlabucket[depth_] );    //用这个深拷贝
		vector<HLabel*>& old_label_bucket = *p_t;

		Sample& csmp = SampleSet::get_instance()[_frameId];

		LabelsGraph* new_graph  ;  //新图 指针
		IndexType	oriNodeNum  = boost::num_vertices( oriGra);     
		IndexType  targetLabel;
		set<IndexType> tagetLabelSet;
		//new_graph = mergeIter(oriGra , old_label_bucket ,patches ,tagetLabelSet);
		new_graph = preprocessGraphAndPushBack( oriGra ,old_label_bucket ,oriNodeNum+1 ,oriNodeNum);
		duawayComponent_[frameId_].hier_graph.push_back(new_graph);   //在再上一层保存新结果

		chierlabucket.push_back(old_label_bucket);
		Logger<<" label bucket 大小："<< chierlabucket.size()<<std::endl;
		return tagetLabelSet;

	}
	LabelsGraph* mergeIter( LabelsGraph& _orIG ,vector<HLabel*>& old_label_bucket , set<IndexType>& patches ,set<IndexType>& new_label )
	{
		/*if( patches.size() < 2){
			set<IndexType>::iterator itr = patches.begin();
			new_label = *itr ;
			return &_orIG;
			
		}*/
		set<IndexType>* new_patches ;
		new_patches = &patches;
		LabelsGraph& oriGra = _orIG;   //初始连接图

		//先遍历每个节点 ，检查其中点的数量
		IndexType	oriNodeNum ;
		IndexType	newNodeNum;
		newNodeNum = oriNodeNum  = boost::num_vertices( oriGra);   //初始赋值
		assert( newNodeNum > patches.size());

		LabelsGraph*	newGraph = &_orIG;  // 初始赋值
		VertexIterator vtxbitr ,vtxeitr;
		boost::tie(vtxbitr , vtxeitr) = boost::vertices( oriGra);

		VertexDescriptor cvd;
		VertexDescriptor tgsd(10000);  // 出初始值 ，只有找到邻边才对其赋值
		for(  ; vtxbitr != vtxeitr ;++ vtxbitr){
			HLabel& cLable = *(old_label_bucket[ *vtxbitr]);

			cvd = *vtxbitr;
			IndexType clabelSize = cLable.vertex_bucket.size();
			Logger<<"label 号： "<< cLable.label_id <<"点的个数 :"<< clabelSize<<std::endl;
			if( patches.find( cvd) != patches.end() ){
				newGraph = new LabelsGraph();
				new_patches = new set<int>();
				new_patches->clear();
				OutEdgeIterator outegbitr , outegeitr ;
				boost::tie( outegbitr , outegeitr) = boost::out_edges( *vtxbitr , oriGra);
				map<IndexType , map<IndexType , HVertex*> > recordColapseEdges;
				vector<HLabel*>  recordNeibNode;
				vector< VertexDescriptor> recordEdgeVD;
				//VertexDescriptor	tgsd;
				set<GraphEdgeProperty> collapseEdgesProperty;
				OutEdgeIterator nextoutegbitr;  //用于保存下一个迭代器地址，避免删边之后迭代器失效
				for(  nextoutegbitr = outegbitr ; outegbitr != outegeitr ; outegbitr = nextoutegbitr){
					++nextoutegbitr;
					VertexDescriptor sd = boost::target( *outegbitr ,oriGra);
					if( patches.find( sd) != patches.end()){
						tgsd = sd;
					}
					recordNeibNode.push_back( old_label_bucket[ sd] );
					recordEdgeVD.push_back( sd);
					EdgeDescriptor& tobeRemovedEdge = *outegbitr;
					GraphEdgeProperty& nextEP = ( oriGra)[tobeRemovedEdge];
					collapseEdgesProperty.insert( nextEP );
					boost::remove_edge( tobeRemovedEdge , oriGra);   //删除这条边

					

				}
				coutGraph( oriGra);
			// 需找最近的 邻节点
				IndexType bestNeibIndexInVector = 0 ;
				//ScalarType  mindistance =100000;
				//ScalarType  distance = 0;
				//for( IndexType i = 0 ; i <recordNeibNode.size() ; ++ i){
				//	HLabel& neibLabel = *(recordNeibNode[i]);
				//	//neibLabel.vertex_bucket
				//	duawayObj_.minDistBeTwoParts( frameId_ ,cLable.vertex_bucket ,neibLabel.vertex_bucket,distance );
				//	if(distance < mindistance){
				//	mindistance = distance;
				//	bestNeibIndexInVector = i;
				//	}
				//}

				bestNeibIndexInVector = 0;
			//重新建边
				for( IndexType i = 0 ; i <recordNeibNode.size() ; ++ i){
					if( i == bestNeibIndexInVector){ continue;}
				//还应该加入property
					boost::add_edge( recordNeibNode[bestNeibIndexInVector]->label_id , recordNeibNode[i]->label_id ,oriGra );

				}

				HLabel& bestNeiLabel = *(recordNeibNode[ bestNeibIndexInVector]);
				mergeTwoLabelAndPushBack(  cLable , bestNeiLabel);
				processAndCreateNewGraphAndPushBack(oriGra , cvd ,old_label_bucket , duawayComponent_[frameId_].label_of_vtx ,*newGraph);
				if( newGraph !=&oriGra){
					delete &oriGra;
				}              
				newNodeNum = oriNodeNum -1;
				break;
			}
			

		}//点迭代结束
		if( new_patches != &patches ){   // new patches 重新生成

			set<IndexType>::iterator setbitr ,seteitr;
			setbitr = patches.begin();
			seteitr = patches.end();
			for( ; setbitr != seteitr ; ++setbitr){
				if(*setbitr == cvd) continue;
				if(*setbitr< cvd){
					(*new_patches).insert( *setbitr);
				}
				else{
					(*new_patches).insert( (*setbitr) -1);
				}
			}

		}else{
		// 说明迭代可以结束了
			new_label = (*new_patches);
			return  &_orIG;
				
		}
		

		
		return mergeIter( *newGraph ,old_label_bucket, *new_patches ,new_label);

	}
	void  smoothHierLabelGraphAndPushBack(IndexType _frameId ,IndexType _whichDepth ,IndexType _smootharea = 20)
	{
		SMOOTH_AREA_ = _smootharea;
		frameId_ = _frameId ;
		depth_ = _whichDepth;

		//map<IndexType, HFrame>& duawayComponent_ =   duawayObj_.hier_componets_ ;
		HFrame& cframe = duawayComponent_[_frameId];
		vector<LabelsGraph*>& chierGraph = cframe.hier_graph;
		map<IndexType ,IndexType>& labelofvtx = cframe.label_of_vtx;   //初始 labelofvtx
		assert( chierGraph .size() > depth_ );

		LabelsGraph* p =new LabelsGraph( *( chierGraph[depth_]) );  //这里拷贝一个新的         //初始连接图
		LabelsGraph& oriGra = *p;
		//LabelsGraph oriGra = *( chierGraph[depth_]);     // 这里有问题，是在栈上分配的，删掉会有问题
		vector< vector<HLabel*> >& chierlabucket =  cframe.hier_label_bucket;   //
		assert( chierlabucket .size() > depth_);
		//vector<HLabel*>& old_label_bucket = chierlabucket[depth_];   //初始label bucket
//		vector<HLabel*>* p_t = new vector<HLabel*>(chierlabucket[depth_]);   // 拷贝一个新的
		vector<HLabel*>* p_t;
		p_t = DeepCopyHLableVec( &chierlabucket[depth_] );    //用这个深拷贝
		vector<HLabel*>& old_label_bucket = *p_t;

		coutHLabelVec(old_label_bucket);
		Sample& csmp = SampleSet::get_instance()[_frameId];

		LabelsGraph* new_graph  ;  //新图 指针
		IndexType	oriNodeNum  = boost::num_vertices( oriGra);     
		IndexType   highestLabel = oriNodeNum - 1;
		new_graph = preprocessGraphAndPushBack( oriGra ,old_label_bucket ,oriNodeNum+1 ,oriNodeNum);
		duawayComponent_[frameId_].hier_graph.push_back(new_graph);   //在再上一层保存新结果

		chierlabucket.push_back(old_label_bucket);
		Logger<<" label bucket 大小："<< chierlabucket.size()<<std::endl;
		coutHLabelVec(old_label_bucket);
		coutGraph( *new_graph);
	}

	void mergeTwoLabelAndPushBack(HLabel& _cLabel , HLabel& _bestNeiLabel_  ){
#define mergeTwoLabel_tset 0
		//测试
		//if( mergeTwoLabel_tset){
		//	Logger<<"_cLabel_.label_id: "<<_cLabel.label_id<< "_bestNeiLabel.label_id: "<<_bestNeiLabel_.label_id<<std::endl;
		//	map<IndexType , HVertex*>::iterator l_vtxbkbitr ,l_vtxbkeitr;
		//	map<IndexType , HVertex*>::iterator r_vtxbkbitr ,r_vtxbkeitr;
		//	l_vtxbkbitr = _cLabel.vertex_bucket.begin();
		//	l_vtxbkeitr = _cLabel.vertex_bucket.end();
		//	r_vtxbkbitr = _bestNeiLabel_.vertex_bucket.begin();
		//	r_vtxbkeitr = _bestNeiLabel_.vertex_bucket.end();
		//	Logger<<"label"<<_cLabel.label_id<<std::endl;
		//	for(  ; l_vtxbkbitr != l_vtxbkeitr ;++ l_vtxbkbitr){
		//		HVertex&  vtx = *(l_vtxbkbitr->second) ;
		//		vtx.label_parent = &_cLabel;
		//		Logger<< "vtxid"<< vtx.vtx_id<<"label_id"<< vtx.label_parent->label_id<<std::endl;
		//	}
		//	Logger<<"label"<<_bestNeiLabel_.label_id<<std::endl;
		//	for(  ; r_vtxbkbitr != r_vtxbkeitr ;++ r_vtxbkbitr){
		//		HVertex&  vtx = *(r_vtxbkbitr->second) ;
		//		vtx.label_parent = &_cLabel;
		//		Logger<< "vtxid"<< vtx.vtx_id<<"label_id"<< vtx.label_parent->label_id<<std::endl;
		//	}

		//}

		// 对 _cLabel_ 和 ——bestNeiLabel 合并，并把合并的结果放于 ——clabel中
		map<IndexType , HVertex*>& lvtxbck = _cLabel.vertex_bucket;
		map<IndexType , HVertex*>& rvtxbck = _bestNeiLabel_.vertex_bucket;
		//_cLabel_.label_id  =  10000;  //表示其需要被重新洗号
		IndexType	mergedLabel = _bestNeiLabel_.label_id;   //合并后使用的label号
		map<IndexType , HVertex*>::iterator l_vtxbkbitr ,l_vtxbkeitr;
		map<IndexType , HVertex*>::iterator r_vtxbkbitr ,r_vtxbkeitr;
		l_vtxbkbitr = lvtxbck.begin();
		l_vtxbkeitr = lvtxbck.end();
		for( l_vtxbkbitr = lvtxbck.begin() ; l_vtxbkbitr != l_vtxbkeitr ;++ l_vtxbkbitr){
			HVertex&  vtx = *(l_vtxbkbitr->second) ;
			vtx.label_parent[0] = &_bestNeiLabel_;
		}
		rvtxbck.insert( lvtxbck.begin() , lvtxbck.end());
//      free_labelId_ = _bestNeiLabel_.label_id;
		// 对 frame 中的 label_of_vtx  进行处理
		map<IndexType ,IndexType>& labelofvtx= _bestNeiLabel_.frame_parent->label_of_vtx;

		r_vtxbkbitr = rvtxbck.begin();
		r_vtxbkeitr = rvtxbck.end();
		for( ; r_vtxbkbitr!= r_vtxbkeitr ;++r_vtxbkbitr){
			labelofvtx[ r_vtxbkbitr->first] = mergedLabel;
		}

		//测试
		//if( mergeTwoLabel_tset){
		//	Logger<<"_cLabel_.label_id: "<<_cLabel.label_id<< "_bestNeiLabel.label_id: "<<_bestNeiLabel_.label_id<<std::endl;
		//	map<IndexType , HVertex*>::iterator l_vtxbkbitr ,l_vtxbkeitr;
		//	map<IndexType , HVertex*>::iterator r_vtxbkbitr ,r_vtxbkeitr;
		//	l_vtxbkbitr = _cLabel.vertex_bucket.begin();
		//	l_vtxbkeitr = _cLabel.vertex_bucket.end();
		//	r_vtxbkbitr = _bestNeiLabel_.vertex_bucket.begin();
		//	r_vtxbkeitr = _bestNeiLabel_.vertex_bucket.end();
		//	Logger<<"label"<<_cLabel.label_id<<std::endl;
		//	for(  ; l_vtxbkbitr != l_vtxbkeitr ;++ l_vtxbkbitr){
		//		HVertex&  vtx = *(l_vtxbkbitr->second) ;
		//		vtx.label_parent = &_cLabel;
		//		Logger<< "vtxid"<< vtx.vtx_id<<"label_id"<< vtx.label_parent->label_id<<std::endl;
		//	}
		//	Logger<<"label"<<_bestNeiLabel_.label_id<<std::endl;
		//	for(  ; r_vtxbkbitr != r_vtxbkeitr ;++ r_vtxbkbitr){
		//		HVertex&  vtx = *(r_vtxbkbitr->second) ;
		//		vtx.label_parent = &_cLabel;
		//		Logger<< "vtxid"<< vtx.vtx_id<<"label_id"<< vtx.label_parent->label_id<<std::endl;
		//	}

		//}


	}
	LabelsGraph* preprocessGraphAndPushBack( LabelsGraph& _orIG ,vector<HLabel*>& old_label_bucket, IndexType _oriNodeNum ,IndexType _newNodeNum)
	{
		LabelsGraph& oriGra = _orIG;   //初始连接图


//		vector<HLabel*>& old_label_bucket = chierlabucket[depth_];   //初始label bucket
		if( _oriNodeNum == _newNodeNum){

			return &_orIG;
		}

		//先遍历每个节点 ，检查其中点的数量
		IndexType	oriNodeNum ;
		IndexType	newNodeNum;
		newNodeNum = oriNodeNum  = boost::num_vertices( oriGra);   //初始赋值
		LabelsGraph*	newGraph = &_orIG;  // 初始赋值

		VertexIterator vtxbitr ,vtxeitr;
		boost::tie(vtxbitr , vtxeitr) = boost::vertices( oriGra);
		
		for(  ; vtxbitr != vtxeitr ;++ vtxbitr){
			HLabel& cLable = *(old_label_bucket[ *vtxbitr]);
			VertexDescriptor cvd = *vtxbitr;
			IndexType clabelSize = cLable.vertex_bucket.size();
			Logger<<"label 号： "<< cLable.label_id <<"点的个数 :"<< clabelSize<<std::endl;
			//小于阈值，需要对该节点进行处理
			if( clabelSize < SMOOTH_AREA_ ){
				//处理函数
				newGraph = new LabelsGraph();
				OutEdgeIterator outegbitr , outegeitr ;
				boost::tie( outegbitr , outegeitr) = boost::out_edges( *vtxbitr , oriGra);
				map<IndexType , map<IndexType , HVertex*> > recordColapseEdges;
				vector<HLabel*>  recordNeibNode;
				vector< VertexDescriptor> recordEdgeVD;
				set<GraphEdgeProperty> collapseEdgesProperty;
				OutEdgeIterator nextoutegbitr;  //用于保存下一个迭代器地址，避免删边之后迭代器失效
				for(  nextoutegbitr = outegbitr ; outegbitr != outegeitr ; outegbitr = nextoutegbitr){
					++nextoutegbitr;
					VertexDescriptor sd = boost::target( *outegbitr ,oriGra);
					
					recordNeibNode.push_back( old_label_bucket[ sd] );
					recordEdgeVD.push_back( sd);
					EdgeDescriptor& tobeRemovedEdge = *outegbitr;
					GraphEdgeProperty& nextEP = ( oriGra)[tobeRemovedEdge];
					collapseEdgesProperty.insert( nextEP );
					boost::remove_edge( tobeRemovedEdge , oriGra);   //删除这条边

				}
//测试图
				coutGraph( oriGra);
				// 需找最近的 邻节点
				IndexType bestNeibIndexInVector = 0 ;
				ScalarType	mindistance =100000;
				ScalarType	distance = 0;
				for( IndexType i = 0 ; i <recordNeibNode.size() ; ++ i){
					HLabel& neibLabel = *(recordNeibNode[i]);
					//neibLabel.vertex_bucket
					duawayObj_.minDistBeTwoParts( frameId_ ,cLable.vertex_bucket ,neibLabel.vertex_bucket,distance );
					if(distance < mindistance){
						mindistance = distance;
						bestNeibIndexInVector = i;
					}
				}
				//重新建边
				for( IndexType i = 0 ; i <recordNeibNode.size() ; ++ i){
					if( i == bestNeibIndexInVector){ continue;}
					//还应该加入property
					boost::add_edge( recordNeibNode[bestNeibIndexInVector]->label_id , recordNeibNode[i]->label_id ,oriGra );

				}
//测试图
				coutGraph( oriGra);

				HLabel& bestNeiLabel = *(recordNeibNode[ bestNeibIndexInVector]);
				IndexType free_id;
				mergeTwoLabelAndPushBack(  cLable , bestNeiLabel);
				processAndCreateNewGraphAndPushBack(oriGra , cvd ,old_label_bucket , duawayComponent_[frameId_].label_of_vtx ,*newGraph);
				

				coutGraph( oriGra);
				coutGraph( *newGraph);
				if( newGraph !=&oriGra){
					delete &oriGra;
				}				
				newNodeNum = oriNodeNum -1;
				break;
			}
			
		}//点迭代结束
		
		return preprocessGraphAndPushBack(*newGraph ,old_label_bucket ,oriNodeNum, newNodeNum );

	}

	void processAndCreateNewGraphAndPushBack(LabelsGraph& _preGraph , VertexDescriptor& _targetVd ,vector<HLabel*>& _labelvec_ ,
		map<IndexType ,IndexType>& labelofvtx_, LabelsGraph& newGraph_)
	{
#define processAndCreateNewGraph_test 0

		//assert(_targetVd > =0 );
		assert( &_preGraph != &newGraph_);
		VertexIterator vtxbitr ,vtxeitr;
		vector<HLabel*> newLabelvec;
		boost::tie( vtxbitr ,vtxeitr) = boost::vertices( _preGraph);
		for( ; vtxbitr != vtxeitr ; ++vtxbitr){
			if( *vtxbitr < _targetVd){
				GraphVertexProperty newvp = _preGraph[*vtxbitr];
				add_vertex( newvp ,newGraph_);
				newLabelvec.push_back( _labelvec_[ *vtxbitr]);
			}
			else if( *vtxbitr == _targetVd){


			}else if( *vtxbitr >_targetVd){
				GraphVertexProperty newvp = _preGraph[*vtxbitr];
				newvp.index =  *vtxbitr-1;
				add_vertex( newvp ,newGraph_);
				//还要对label 对应的实体处理
				_labelvec_[ *vtxbitr]->label_id = *vtxbitr-1;
				map<IndexType , HVertex*>::iterator vtxbckbitr = _labelvec_[ *vtxbitr]->vertex_bucket.begin();
				map<IndexType , HVertex*>::iterator vtxbckeitr = _labelvec_[ *vtxbitr]->vertex_bucket.end();
				for( ;vtxbckbitr != vtxbckeitr ;++vtxbckbitr){
					//Logger<<"vtxbucket->first"<<vtxbckbitr->first<<"*vtxbitr"<<(*vtxbitr)-1<<std::endl;
					labelofvtx_[ vtxbckbitr->first] = (*vtxbitr) -1;
				}
				//Logger<<"labelvec_ size"<<_labelvec_.size()<<std::endl;
				newLabelvec.push_back( _labelvec_[ *vtxbitr]);

			}else{


			}

		}//接着遍历边
		_labelvec_.clear();
		_labelvec_ = newLabelvec;

		EdgeIterator	egbitr ,egeitr;
		boost::tie( egbitr ,egeitr) = boost::edges( _preGraph);
		for( ; egbitr != egeitr ; ++egbitr){
			EdgeDescriptor		ed = *egbitr;
			VertexDescriptor	sd = boost::source( ed ,_preGraph);
			VertexDescriptor	td = boost::target( ed ,_preGraph);
			if( _targetVd == sd || _targetVd == td ){
				continue;
			}
			if( (sd<_targetVd) &&( td< _targetVd)){
				boost::add_edge( sd , td , newGraph_);

			}else if( (sd>_targetVd) &&( td< _targetVd)){
				boost::add_edge( sd-1 ,td ,newGraph_);
			}else if( (sd<_targetVd) &&( td> _targetVd)){
				boost::add_edge( sd , td-1 ,newGraph_);
			}else if((sd>_targetVd) &&( td> _targetVd)){
				boost::add_edge( sd -1 ,td -1 ,newGraph_);
			}

		}


		if(processAndCreateNewGraph_test){
			coutGraph(newGraph_);
		}
	}

	void coutGraph( LabelsGraph& _g)
	{
		VertexIterator vtxbitr ,vtxeitr;
		boost::tie( vtxbitr ,vtxeitr) = boost::vertices(_g);
		AdjacencyIterator adjbitr ,adjeitr;
		for( ; vtxbitr != vtxeitr ; ++vtxbitr){
			Logger<<" 顶点 "<<*vtxbitr<<std::endl;
			boost::tie( adjbitr ,adjeitr) = boost::adjacent_vertices( *vtxbitr , _g);
			Logger<<"它的邻点有 "<<std::endl;
			for( ; adjbitr != adjeitr ;++adjbitr){
				Logger<< *adjbitr<<std::endl;
			}
		}
		//for( ; ; ++ v)

	}

	void coutHLabelVec(vector<HLabel*>& hblvec)
	{
		vector<HLabel*>::iterator Hlabelbitr ,Hlabeleitr;
		Hlabelbitr = hblvec.begin();
		Hlabeleitr = hblvec.end();
		for( ; Hlabelbitr!= Hlabeleitr;++Hlabelbitr){
			HLabel* phlal = *Hlabelbitr;
			
			Logger<< "label 号 " <<phlal->label_id<<" 点数 "<< phlal->vertex_bucket.size()<< std::endl;
		}

	}
	void setLayerLabelDisplay()
	{
		Sample& smp = SampleSet::get_instance()[frameId_];
		Logger<<"帧号： "<< frameId_ <<std::endl;
		//DualwayPropagation.hier_componets_[frameId_].
		map< IndexType , IndexType>& labofvtx = duawayObj_.hier_componets_[frameId_].label_of_vtx;
		map<IndexType ,IndexType>::iterator lbofvtx_bitr = duawayObj_.hier_componets_[frameId_].label_of_vtx.begin();
		map<IndexType ,IndexType>::iterator lbofvtx_eitr = duawayObj_.hier_componets_[frameId_].label_of_vtx.end();
		for( ; lbofvtx_bitr != lbofvtx_eitr; ++lbofvtx_bitr ){
			Logger<<"点号 "<< lbofvtx_bitr->first<<" label 号： "<< lbofvtx_bitr->second <<std::endl;
			smp[lbofvtx_bitr->first].set_label(lbofvtx_bitr->second);

		}
	}

	 vector<HLabel*>* DeepCopyHLableVec( const vector<HLabel*>* _p_oriHLabelvec )
	{
		 vector<HLabel*>* _p_newHLabelvec;
		_p_newHLabelvec = new vector<HLabel*>(*_p_oriHLabelvec);
		vector<HLabel*>::iterator bitr ,eitr;
		bitr = _p_newHLabelvec->begin();
		eitr = _p_newHLabelvec->end();
		for( ; bitr!= eitr ; ++bitr){
			HLabel* p_orihlabel = * bitr;
			HLabel* p_newhlabel;
			 p_newhlabel = DeepCopyHLabel(p_orihlabel);
			*bitr = p_newhlabel;
		}
		return _p_newHLabelvec;

	}
	HLabel* DeepCopyHLabel( const HLabel* _p_oriHlabel  )
	{
		 HLabel* _p_newHlabel = new HLabel( *_p_oriHlabel);
		//这个 newHlabel 中的 label_id ， frame_parent  不需要变， prev_corr ，next_corr 也不需要变 ， 但vertex_bucket 中的Hvertex需被处理
		//map<IndexType , HVertex*> new_vertex_bucket;
		map<IndexType ,HVertex*>::iterator  hvtxbitr ,hvtxeitr;
		hvtxbitr = (*_p_newHlabel).vertex_bucket.begin();
		hvtxeitr = (*_p_newHlabel).vertex_bucket.end();
		for( ; hvtxbitr != hvtxeitr ;++hvtxbitr){
			HVertex* p_orivtx = hvtxbitr->second;
			HVertex* p_newvtx;
			p_newvtx = DeepCopyVertex(p_orivtx  ,_p_newHlabel);
			hvtxbitr->second = p_newvtx;
		}
		return _p_newHlabel;

	}
	HVertex*  DeepCopyVertex( const HVertex* _p_orivtx ,   HLabel* new_label_parent )
	{
		HVertex* _p_newvtx = new HVertex( *_p_orivtx);
		//这个新的 HVertex 中的 label_parent 需重新赋值；
		_p_newvtx->label_parent[0] = new_label_parent;
		return _p_newvtx;

	}

public:
	DualwayPropagation& duawayObj_;
	map<IndexType, HFrame>& duawayComponent_;
	IndexType	frameId_;
	IndexType	depth_;
	IndexType	SMOOTH_AREA_;

};
 

 

void DualwayPropagation::propagate_back_with_link(IndexType f)
{
	FrameWithNodeCaculation fwnc(SampleSet::get_instance() ,components_ ,frame_index_map_label_ ,f, f+1);
	fwnc.compute( f , f+1 );
	//fwnc.coutFrameNodeLink();
	//fwnc.coutFrameNodeAdjCMP();
	fwnc.coutFrameNodeAdj();
	fwnc.coutPatchMatch(10);
}



void DualwayPropagation::read_label_file_hier(char *filename)
{
	FILE* in_file = fopen(filename, "r");
	if (in_file==NULL)
	{
		return;
	}
	IndexType frame, label, vtx_idx;
	while ( true )
	{
		int stat =  fscanf(in_file, "%d %d %d\n",&frame, &label, &vtx_idx);
		if (stat == EOF)
			break;


		if ( hier_componets_.find(frame)== hier_componets_.end() )
		{
			hier_componets_.insert(make_pair(frame, HFrame()));
			hier_componets_[frame].frame_id = frame;	
			hier_componets_[frame].hier_label_bucket.resize(1);
		}
		if ( label >= hier_componets_[frame].hier_label_bucket[0].size() )
		{
			hier_componets_[frame].hier_label_bucket[0].resize( label+1 );
		}
		if (   nullptr==hier_componets_[frame].hier_label_bucket[0][label] )
		{
			HLabel* new_label_space = allocator_.allocate<HLabel>();
			HLabel* new_label = new (new_label_space)HLabel;
			hier_componets_[frame].hier_label_bucket[0][label] = new_label;
			hier_componets_[frame].hier_label_bucket[0][label]->frame_parent = &hier_componets_[frame];
			hier_componets_[frame].hier_label_bucket[0][label]->label_id = label;
		}
		HVertex* new_space = allocator_.allocate<HVertex>();
		HVertex* new_vtx = new (new_space)HVertex(vtx_idx, hier_componets_[frame].hier_label_bucket[0][label]);
		hier_componets_[frame].hier_label_bucket[0][label]->vertex_bucket.insert( make_pair(vtx_idx,new_vtx) );
		hier_componets_[frame].label_of_vtx.insert( make_pair(vtx_idx, label) );


	}
}

void DualwayPropagation::getEdgeVertexs( IndexType _CFrameId ,IndexType lLabelId ,IndexType rLabelId , map<IndexType, map<IndexType ,HVertex*> >& _edgepoints )
{
	map<IndexType,HVertex*> twoLabelsVtx;


	twoLabelsVtx.insert(hier_componets_[_CFrameId].hier_label_bucket[0][lLabelId]->vertex_bucket.begin(),
		                hier_componets_[_CFrameId].hier_label_bucket[0][lLabelId]->vertex_bucket.end() );

	twoLabelsVtx.insert(hier_componets_[_CFrameId].hier_label_bucket[0][rLabelId]->vertex_bucket.begin(),
		                hier_componets_[_CFrameId].hier_label_bucket[0][rLabelId]->vertex_bucket.end() );

	const IndexType k = 3;

	IndexType neighbours[k];

	ScalarType dist[k];

	IndexType i = 0;
	map<IndexType,HVertex*> epoints;
	for (auto iter = twoLabelsVtx.begin(); iter != twoLabelsVtx.end(); iter ++)
	{
		IndexType ori_idx = iter->first;
		IndexType map_id = o2d_map[ori_idx];

		downSample->neighbours(map_id,k,neighbours,dist);
		set<IndexType> neilabel_size;
		neilabel_size.clear();
		for (IndexType kk = 0; kk < k; kk++)
		{
			IndexType neig_downId = neighbours[kk];
			IndexType oriNeig_id = d2o_map[neig_downId];
			IndexType neig_label = hier_componets_[_CFrameId].label_of_vtx[oriNeig_id];
			neilabel_size.insert(neig_label);
		}
		
		if( neilabel_size.size() > 1){
			//map<IndexType, map<IndexType ,HVertex*> >& _edgepoints
			epoints[ori_idx] = (*iter).second;
			IndexType seKey = frame_label_to_key(lLabelId,rLabelId);
			_edgepoints[seKey] = epoints;
			
		} ;//它就是边界点
	}

	return;
}



void DualwayPropagation::getEdgeVertexsByMinDIstance( IndexType _CFrameId , distanPriQueue& _PriQuemap, map<IndexType, map<IndexType ,HVertex*> >& _edgepoints )
{
	Sample& smp = SampleSet::get_instance()[_CFrameId];
	map<IndexType,HVertex*> edgedps;
	pointdistance pd;
	for( IndexType i  = 10 ; i >0 ; --i){
		pd = _PriQuemap.top();
		_PriQuemap.pop();
//		std::cout<<"vtx1Id: "<<pd.vtx1Id_ <<"    labelofvtx1:   "<<pd.labelofvtx1_<<"    vtx2:  "<<pd.vtx2Id_<<"   labelofvtx2:  "<<pd.labelofvtx2_<<std::endl;
		HVertex* tmpVtx;

		IndexType Id1 = hier_componets_[_CFrameId].hier_label_vtxBucket_index[0][pd.labelofvtx1_];   //new added , to solve label not continuous

		tmpVtx = hier_componets_[_CFrameId].hier_label_bucket[0][Id1]->vertex_bucket[pd.vtx1Id_];
		Vertex& vtx1 = smp[tmpVtx->vtx_id];
		vtx1.set_edge_point( true);          //用于绘制边界点
		vtx1.set_wrapbox(false);
		edgedps[ pd.vtx1Id_] = tmpVtx;

		IndexType Id2 = hier_componets_[_CFrameId].hier_label_vtxBucket_index[0][pd.labelofvtx2_];   //new added , to solve label not continuous

		tmpVtx = hier_componets_[_CFrameId].hier_label_bucket[0][Id2]->vertex_bucket[pd.vtx2Id_];
		Vertex& vtx2 = smp[tmpVtx->vtx_id];
		vtx2.set_edge_point( true);
		vtx2.set_wrapbox(false);
		edgedps[ pd.vtx2Id_] = tmpVtx;
		if( pd.labelofvtx1_ < pd.labelofvtx2_){
			vtx1.set_edgePointWithSmallLabel(true);
			vtx2.set_edgePointWithSmallLabel(false);
		}else{
			vtx1.set_edgePointWithSmallLabel(false);
			vtx2.set_edgePointWithSmallLabel(true);
		}

			
	}

	IndexType seKey = frame_label_to_key( pd.labelofvtx1_, pd.labelofvtx2_);
	_edgepoints[ seKey] = edgedps;
	
}

void DualwayPropagation::buildKdTree( IndexType _cframeId)
{
	downSample =new Sample();
	Sample& smp = SampleSet::get_instance()[_cframeId];
	auto iter = hier_componets_[_cframeId].label_of_vtx.begin();
	IndexType i = 0;
	for(; iter != hier_componets_[_cframeId].label_of_vtx.end(); ++iter,i++ )
	{
		IndexType vtx_id = (*iter).first;
		Vertex& vtx = smp[vtx_id];

		PointType v( vtx.x(), vtx.y(), vtx.z() );
		ColorType cv(vtx.r(), vtx.g(), vtx.b(), vtx.alpha());
		NormalType nv(vtx.nx(), vtx.ny(), vtx.nz());

		downSample->add_vertex(v,nv,cv);
		o2d_map[vtx_id] = i;
		d2o_map[i] = vtx_id;

	}

	downSample->build_kdtree();

}



void DualwayPropagation:: init_labeles_graph_hier()
{
	for (auto citer = hier_componets_.begin(); citer!=hier_componets_.end(); citer++)
	{
		std::cout<<"第"<<citer->first<<"帧:"<<std::endl;
		

		IndexType lbsize = citer->second.hier_label_bucket.size();
		assert( lbsize > 0);
		IndexType nodeSize = citer->second.hier_label_bucket[lbsize -1].size();//.size();

		//LabelsGraph* new_labelGraph_space = allocator_.allocate<LabelsGraph>();

		//LabelsGraph* new_labelGraph = new (new_labelGraph_space)LabelsGraph;
		LabelsGraph* new_labelGraph = new LabelsGraph();
		GraphVertexProperty gvp;

		IndexType gep_count  = 0;
		for( IndexType i = 0 ; i< nodeSize ; ++i){
			gvp.index = i ;
			gvp.prev =  -1;
			gvp.next = -1;
			add_vertex( gvp , *new_labelGraph);

		}
		//new_labelGraph->added_vertex(nodeSize);
		//if( downSample){
		//	delete downSample;
		//	o2d_map.clear();
		//	d2o_map.clear();
		//}

		//buildKdTree(citer->first);

	

		for (IndexType i = 0; i < nodeSize-1; i ++)
		{
			for (IndexType j = i + 1; j < nodeSize; j++)
			{
				GraphEdgeProperty   gep;
				distanPriQueue PriQue;   //
				while ( ! PriQue.empty() ) PriQue.pop(); 

				HLabel* fir = citer->second.hier_label_bucket[lbsize -1][i];

				HLabel* sec = citer->second.hier_label_bucket[lbsize -1][j];

				map<IndexType,HVertex*> fir_vtx = fir->vertex_bucket;

				map<IndexType,HVertex*> sec_vtx = sec->vertex_bucket;

				//计算两个块之间的最短距离
				ScalarType minDis = 1000000;
				SampleSet& sample_set = SampleSet::get_instance();
				map<IndexType,HVertex*>::iterator biter1 , eiter1 ,biter2,eiter2;
				eiter1 = fir_vtx.end();
				eiter2 = sec_vtx.end();
				ScalarType dia ;
//				IndexType ti = 0;
//				IndexType tj = 0;
				for( biter1 = fir_vtx.begin() ;biter1 != eiter1 ;++biter1 /*++ti*/){
					for( biter2 = sec_vtx.begin() ;biter2 != eiter2 ;++biter2 /*++tj*/){
						IndexType index1 = biter1->first;
						Sample& s = sample_set[citer->first];
						dia = s.getBox().diag();

						PointType point1 =  s.vertices_matrix().col(index1);

						IndexType index2 = biter2->first;

						PointType point2 =  s.vertices_matrix().col(index2);

						ScalarType distance = (point1 - point2).norm();
						if(distance < minDis)minDis = distance;
						IndexType label1 = hier_componets_[citer->first].label_of_vtx[ index1];
						IndexType label2 = hier_componets_[citer->first].label_of_vtx[ index2];
						if( distance < dia*0.1){
//							pointdistance tpd( index1 ,  label1 , index2 ,label2 ,distance );
//							PriQue.push( tpd);
//							std::cout<< "ti:"<< ti<<"tj:"<<tj<<"distance:"<<distance<<std::endl;
						}
						

					}
//					tj = 0;

				}

				if (minDis <  dia * 0.04 /*0.0588*/ )
				{
					//遍历
					//gep.edgePoints = 
//					getEdgeVertexsByMinDIstance( citer->first ,PriQue ,gep.edgePoints);
			 

					//components_( *citer)->second.label_of_vtx[biter1->first] ;

				
					// 加边
					// 得到边界点的集合

					gep.index = gep_count;
					if( i < j){
						gep.start_ = i;
						gep.end_ = j;
						boost::add_edge(i,j,gep ,*new_labelGraph);
						++gep_count;
					}else if( i> j){
						gep.start_ = j;
						gep.end_ = i;
						boost::add_edge( j, i ,gep ,*new_labelGraph);
						++gep_count;
					}else{
						// i == j 时不处理
					}
					
				
				}

			}

		}

		citer->second.hier_graph.clear();

		citer->second.hier_graph.push_back( new_labelGraph);
			//labelGraph = new_labelGraph;
		//citer->second.labelGraph = new_labelGraph;


		//assert(citer->second.hier_graph.size() > 0);
		////测试图
		//LabelsGraph& mg = *(citer->second.hier_graph[0]);
		//LabelsGraph::vertex_iterator CFvertex_biter ,CFvertex_eiter;
		//LabelsGraph::adjacency_iterator CFadj_bitr , CFadj_eitr ;
		//typedef LabelsGraph::edge_iterator EdgeIterator ;
		//typedef LabelsGraph::vertex_descriptor VertexDescriptor;
		//typedef LabelsGraph::edge_descriptor   EdgeDescriptor;
		////boost::tie( CFvertex_biter ,CFvertex_eiter) = boost::vertices( mg );
		//std::cout<<"当前帧"<<citer->first<<std::endl;
		////for( ; CFvertex_biter!=CFvertex_eiter ;++CFvertex_biter){
		////	
		////	std::cout<< "点："<< * CFvertex_biter<<std::endl;
		////	VertexDescriptor vd;
		////	vd = *CFvertex_biter;
		////	GraphVertexProperty& gvp = mg[vd];
		////	boost::tie( CFadj_bitr ,CFadj_eitr) = boost::adjacent_vertices( *CFvertex_biter , mg);
		////	for( ; CFadj_bitr != CFadj_eitr;++CFadj_bitr){
		////		std::cout<< "邻点："<< *CFadj_bitr<<std::endl;

		////	}
		////}
		//std::pair<EdgeIterator, EdgeIterator> ei = edges( mg);
		//std::cout<<"获得其的邻边"<<std::endl;
		//for( EdgeIterator eit = ei.first ;eit != ei.second ;++eit){
		//	std::cout<<"邻边 "<<*eit<<":"<<std::endl;
		//	EdgeDescriptor ed = *eit;
		//	GraphEdgeProperty& gep2 = mg[ed];
		//	std::cout<<"该边 "<<gep2.index<<"的属性:"<<std::endl;
		//	std::cout<<"index"<<gep2.index<<":"<<std::endl;
		//	//std::cout<<"edgePoints"<<gep2.edgePoints<<":"<<std::endl;
		//	std::cout<<"start"<<gep2.start_<<":"<<std::endl;
		//	std::cout<<"end"<<gep2.end_<<":"<<std::endl;
		//	//
		//	auto gep_bitr = gep2.edgePoints.begin();
		//	auto gep_mbitr = (*gep_bitr).second.begin();
		//	auto gep_meitr = (*gep_bitr).second.end();
		//	for( ; gep_mbitr != gep_meitr ; ++gep_mbitr){
		//		std::cout<<"边界顶点号"<< gep_mbitr->first<<std::endl;
		//		
		//		std::cout<<"其对应的顶点label号"<< gep_mbitr->second->label_parent->label_id<<std::endl;
		//	}
		//	 
		//
		//	VertexDescriptor sd = boost::source( ed , mg);
		//	VertexDescriptor td = boost::target( ed , mg);

		//	GraphVertexProperty& s_vp = mg[sd];
		//	GraphVertexProperty& t_vp = mg[td];
		//	//std::cout<<"起点： "<<sd<<":"<<std::endl;
		//	std::cout<<"起点的属性 "<<":"<<std::endl;
		//	std::cout<<"index"<<s_vp.index<<":"<<std::endl;
		//	std::cout<<"start"<<s_vp.prev<<":"<<std::endl;
		//	std::cout<<"end"<<s_vp.next<<":"<<std::endl;
		//	//std::cout<<"终点： "<<td<<":"<<std::endl;
		//	std::cout<<"终点的属性 "<<":"<<std::endl;
		//	std::cout<<"index"<<t_vp.index<<":"<<std::endl;
		//	std::cout<<"start"<<t_vp.prev<<":"<<std::endl;
		//	std::cout<<"end"<<t_vp.next<<":"<<std::endl;


		//}//边遍历结束



	}//帧遍历结束


}

void DualwayPropagation::init_labeles_graph_hier2()
{
	Logger<<"  Begin initialize graphs.\n";

	for (auto citer = hier_componets_.begin(); citer!=hier_componets_.end(); citer++)
	{

		IndexType lbsize = citer->second.hier_label_bucket.size();
		assert( lbsize > 0);
		IndexType nodeSize = citer->second.hier_label_bucket[lbsize -1].size();//.size();

		LabelsGraph* new_labelGraph_space = allocator_.allocate<LabelsGraph>();

		LabelsGraph* new_labelGraph = new (new_labelGraph_space)LabelsGraph;

		GraphVertexProperty gvp;

		map<IndexType,IndexType> labelLevel = citer->second.hier_label_vtxBucket_index[0];

		auto labIndex = labelLevel.begin();

		IndexType gep_count  = 0;

		for( IndexType i = 0 ; i< nodeSize && labIndex != labelLevel.end(); ++i,++ labIndex)
		{
			gvp.index = i ;
			gvp.prev =  -1;
			gvp.next = -1;
			gvp.label_id = labIndex->first;
			add_vertex( gvp , *new_labelGraph);
		}

		for (IndexType i = 0; i < nodeSize-1; i ++)
		{
			for (IndexType j = i + 1; j < nodeSize; j++)
			{
				GraphEdgeProperty   gep;

				distanPriQueue PriQue;   //
				while ( ! PriQue.empty() ) PriQue.pop(); 

				HLabel* fir = citer->second.hier_label_bucket[lbsize -1][i];

				HLabel* sec = citer->second.hier_label_bucket[lbsize -1][j];

				map<IndexType,HVertex*> fir_vtx = fir->vertex_bucket;

				map<IndexType,HVertex*> sec_vtx = sec->vertex_bucket;

				//计算两个块之间的最短距离
				ScalarType minDis = 1e5;
				SampleSet& sample_set = SampleSet::get_instance();
				map<IndexType,HVertex*>::iterator biter1 , eiter1 ,biter2,eiter2;
				eiter1 = fir_vtx.end();
				eiter2 = sec_vtx.end();
				ScalarType dia ;

				for( biter1 = fir_vtx.begin() ;biter1 != eiter1 ;++biter1)
				{
					for( biter2 = sec_vtx.begin() ;biter2 != eiter2 ;++biter2)
					{
						IndexType index1 = biter1->first;
						Sample& s = sample_set[citer->first];
						dia = s.getBox().diag();

						PointType point1 =  s.vertices_matrix().col(index1);

						IndexType index2 = biter2->first;

						PointType point2 =  s.vertices_matrix().col(index2);

						ScalarType distance = (point1 - point2).norm();
						if(distance < minDis)minDis = distance;
						IndexType label1 = hier_componets_[citer->first].label_of_vtx[ index1];
						IndexType label2 = hier_componets_[citer->first].label_of_vtx[ index2];
						if( distance < dia*0.1){
							pointdistance tpd( index1 ,  label1 , index2 ,label2 ,distance );
							PriQue.push( tpd);
						}

					}

				}

				if (minDis <  dia * 0.04 )//0.0588
				{
					//getEdgeVertexs2( citer->first ,PriQue ,gep.edgePoints);
					getEdgeVertexsByMinDIstance( citer->first ,PriQue ,gep.edgePoints);
					//边界点集个数太少也不需要添加,可能是噪声点.

					gep.index = gep_count;
					if( i < j)
					{
						gep.start_ = i;
						gep.end_ = j;
						boost::add_edge(i,j,gep ,*new_labelGraph);
						++gep_count;
					}else if( i> j)
					{
						gep.start_ = j;
						gep.end_ = i;
						boost::add_edge( j, i ,gep ,*new_labelGraph);
						++gep_count;
					}else
					{
						// i == j 时不处理
					}

				}

			}

		}

		citer->second.hier_graph.clear();

		citer->second.hier_graph.push_back( new_labelGraph);

	}//帧遍历结束

	Logger<<"  End initialize graphs.\n";


}
//过期方法
void DualwayPropagation::getEdgeVertexs2( IndexType _CFrameId , distanPriQueue& _PriQuemap, map<IndexType, map<IndexType ,HVertex*> >& _edgepoints )
{
	map<IndexType,HVertex*> edgedps;
	pointdistance pd;

	IndexType sr_labelLevel,tg_labelLevel;
	for( IndexType i  = 10 ; i >0 ; --i)
	{
		pd = _PriQuemap.top();
		_PriQuemap.pop();

		sr_labelLevel = hier_componets_[_CFrameId].hier_label_vtxBucket_index[0][pd.labelofvtx1_];
		tg_labelLevel = hier_componets_[_CFrameId].hier_label_vtxBucket_index[0][pd.labelofvtx2_];

		HVertex* tmpVtx;


		tmpVtx = hier_componets_[_CFrameId].hier_label_bucket[0][sr_labelLevel]->vertex_bucket[pd.vtx1Id_];
		edgedps[ pd.vtx1Id_] = tmpVtx;

		// 		tmpVtx = hier_componets_[_CFrameId].hier_label_bucket[0][pd.labelofvtx2_]->vertex_bucket[pd.vtx2Id_];
		// 		edgedps[ pd.vtx2Id_] = tmpVtx;



	}

	IndexType seKey = frame_label_to_key(sr_labelLevel, tg_labelLevel);
	//IndexType seKey = frame_label_to_key( pd.labelofvtx1_, pd.labelofvtx2_);

	_edgepoints[ seKey] = edgedps;

}


void DualwayPropagation::init_node_link()
{
	for (auto citer = hier_componets_.begin(); citer!=hier_componets_.end(); citer++){
		std::cout<<"第"<<citer->first<<"帧"<<std::endl;
		init_node_link( citer->first , 0);

	}
}
void DualwayPropagation::init_node_link(IndexType _frameId ,IndexType _depth)
{
	HFrame& cframe = hier_componets_[_frameId];
	Sample& csmp = SampleSet::get_instance()[_frameId];

	LabelsGraph& lbgf = *(cframe.hier_graph[_depth]);
	LabelsGraph::vertex_iterator vbitr , veitr;
	LabelsGraph::adjacency_iterator adjbitr, adjeitr ;
	boost::tie( vbitr , veitr) = boost::vertices( lbgf);
	assert( _depth < cframe.hier_label_bucket.size() );
	/*vector<HLabel*>::iterator hlable_bitr = cframe.hier_label_bucket[_depth].begin();
	vector<HLabel*>::iterator hlable_eitr = cframe.hier_label_bucket[_depth].end();*/
	for( ; vbitr != veitr; ++ vbitr){
		HLabel& hlb1  = *(cframe.hier_label_bucket[_depth][*vbitr]);
		std::cout<<"vbitr:"<<*vbitr<<" hlabel:"<<hlb1.label_id<<std::endl;
		map<IndexType ,HVertex*>::iterator hvtxbk_bitr1 = hlb1.vertex_bucket.begin();
		map<IndexType ,HVertex*>::iterator hvtxbk_eitr1 = hlb1.vertex_bucket.end();
		PointType mean1( 0 ,0 ,0 );
		PointType sum1( 0 ,0 ,0 );
		IndexType  count1 = 0;
		for( ; hvtxbk_bitr1 != hvtxbk_eitr1 ;++ hvtxbk_bitr1, ++count1){
			Matrix3X& pointArray = csmp.vertices_matrix();  
			PointType point = csmp.vertices_matrix().col(hvtxbk_bitr1->first);
			sum1 += point;
		}
		mean1 = sum1/count1;

		std::cout<<"x:"<<mean1.x()<<"y:"<<mean1.y()<<"z:"<<mean1.z()<<std::endl;
		boost::tie( adjbitr ,adjeitr) = boost::adjacent_vertices( *vbitr , lbgf);
		if( adjbitr == adjeitr ){  //没有邻点

			IndexType index1 = *vbitr;
			LinkNode* linknode;
			linknode = new LinkNode( index1 , index1 ,mean1 , mean1);
			//这里再想一下
			if( (_depth +1) > csmp.wrap_box_link_.size() ){
				csmp.wrap_box_link_.resize(_depth+1);
			}
			csmp.wrap_box_link_[_depth].push_back(*linknode); 
			delete linknode;
			continue;
		}


		for( ; adjbitr != adjeitr; ++adjbitr){
			//
			HLabel&  hlb2 = *(cframe.hier_label_bucket[_depth][*adjbitr]);
			std::cout<<"adjbitr:"<<*adjbitr<<" llabel:"<<hlb2.label_id<<std::endl;
			map<IndexType ,HVertex*>::iterator hvtxbk_bitr2 = hlb2.vertex_bucket.begin();
			map<IndexType ,HVertex*>::iterator hvtxbk_eitr2 = hlb2.vertex_bucket.end();
			//
			PointType mean2( 0 ,0 ,0 );
			PointType sum2( 0 ,0 ,0);
			IndexType  count2 = 0;
			for( ; hvtxbk_bitr2 != hvtxbk_eitr2 ;++ hvtxbk_bitr2, ++count2){
				Matrix3X& pointArray = csmp.vertices_matrix();  
				PointType point = csmp.vertices_matrix().col(hvtxbk_bitr2->first);
				sum2 += point;
			}
			mean2 = sum2/count2;
			std::cout<<"x:"<<mean2.x()<<"y:"<<mean2.y()<<"z:"<<mean2.z()<<std::endl;
			IndexType index1 = *vbitr;
			IndexType index2 = *adjbitr;
			LinkNode* linknode;
			if( index1 > index2){
				linknode = new LinkNode( index1 , index2 ,mean1 , mean2);
			}else{
				linknode = new LinkNode( index2 , index1 ,mean2 , mean1);
			}
			
			//这里再想一下
			if( (_depth +1) > csmp.wrap_box_link_.size() ){
				csmp.wrap_box_link_.resize(_depth+1);
			}
			csmp.wrap_box_link_[_depth].push_back(*linknode); 
			delete linknode;
			
		}//后节点遍历结束


	}//前节点遍历结束

	//test
	vector<LinkNode>::iterator lnbitr = csmp.wrap_box_link_[_depth].begin();
	vector<LinkNode>::iterator lneitr = csmp.wrap_box_link_[_depth].end();
	for( ; lnbitr != lneitr ;++lnbitr){
		LinkNode& ln =*lnbitr;
		std::cout<<"labelh:" <<ln.labelH_<<" x:"<<ln.pointH_.x()<<" y:"<<ln.pointH_.y()<<" z:"<<ln.pointH_.z()<<std::endl;
		std::cout<<"labell:" <<ln.labelL_<<" x:"<<ln.pointL_.x()<<" y:"<<ln.pointL_.y()<<" z:"<<ln.pointL_.z()<<std::endl;

	}


}

void DualwayPropagation::split_twoAjacent_graph_next(IndexType srFrame, IndexType tgFrame)
{
	//向前分裂

	//get the new graph of tgGrame--需要深拷贝

	IndexType tgGraSize = hier_componets_[tgFrame].hier_graph.size();
	LabelsGraph* oriGra = hier_componets_[tgFrame].hier_graph[tgGraSize - 1];
	LabelsGraph* new_graph = new LabelsGraph(*oriGra);

	//new_graph = oriGra;

	//vector<HLabel* > new_label_bucket =  hier_componets_[tgFrame].hier_label_bucket[tgGraSize - 1];
	vector<HLabel* > new_label_bucket;
	copyLabelBucket(new_label_bucket,hier_componets_[tgFrame].hier_label_bucket[tgGraSize - 1] );


	//
	IndexType srGraSize = hier_componets_[srFrame].hier_graph.size();

	LabelsGraph* srGraLat = hier_componets_[srFrame].hier_graph[srGraSize - 1];


	pair<EdgeIterator,EdgeIterator> ei = boost::edges(*srGraLat);

	for (EdgeIterator eit = ei.first; eit != ei.second; ++eit)
	{
		EdgeDescriptor ed = *eit;

		GraphEdgeProperty& ep = (*srGraLat)[ed];

		map<IndexType,HVertex*> edgePoints;

		auto ePsIt = ep.edgePoints.begin();

		edgePoints.insert(ePsIt->second.begin(),ePsIt->second.end() );  

		if (edgePoints.size() < 3)
		{
			Logger<<"边上的顶点数太少,无法分裂.\n";
			continue;
		}

		map<IndexType,HVertex*> edgeCorrNextVtx;

		IndexType newGraphEdgeSize = new_graph->m_edges.size();

		IndexType nodeId = checkNextLabelBucket(edgePoints,edgeCorrNextVtx);//获得边界点在下一帧对应的块和对应点
		//IndexType nodeId = edgeCorrNextVtx.size();


		//对应回去,标签是起始点,则保持不变
		HLabel* splitedLabel = new_label_bucket[nodeId];

		IndexType eS = ep.start_;
		IndexType eE = ep.end_;

		IndexType recordS = 0;      
		IndexType recordE = 0;

		for (auto iter = splitedLabel->vertex_bucket.begin(); iter != splitedLabel->vertex_bucket.end(); iter ++)
		{
			IndexType prev_id = iter->second->prev_corr->label_parent[0]->label_id;

			if (prev_id == eS)
			{
				recordS ++;

			}else if(prev_id == eE)
			{
				recordE ++;
			}
		}


		//若分裂出来的点个数有一个数据很少,则该边不做裂变

		if ( recordE < 5 || recordS < 5)
		{
			Logger<<"边界点太靠近,不需要分裂.\n";
			continue;
		}



		//遍历nodeId 上相连接的边

		pair<VertexIterator, VertexIterator> vi = boost::vertices(*new_graph);

		VertexIterator nodeIter = (vi.first + nodeId);

		VertexDescriptor nodeDesc = *nodeIter;

		//节点对应的所有出边
		pair<OutEdgeIterator,OutEdgeIterator> nodeEiter = boost::out_edges(nodeDesc,*new_graph);

		map<IndexType,map<IndexType,HVertex*> > recordColapseEdges;

		set<GraphEdgeProperty> collapseEdges;

		OutEdgeIterator oit,nextIt;

		oit = nodeEiter.first;

		for (nextIt = oit; oit != nodeEiter.second; oit = nextIt )
		{
			++nextIt;

			EdgeDescriptor nextEdgeD = *oit;

			GraphEdgeProperty& nextEP = (*new_graph)[nextEdgeD];

			collapseEdges.insert(nextEP);

			boost::remove_edge(*oit,*new_graph);//删除这条边
		}


		//增加一个节点

		IndexType nSize = boost::num_vertices(*new_graph);

		GraphVertexProperty vp(nSize,-1,-1);

		boost::add_vertex(vp,*new_graph);


		//更新分割块信息,新增加的Label标号为nSize. 被分裂的点为nodeId

		IndexType new_label = nSize;

		new_label_bucket.push_back((HLabel*)0 );
		HLabel* new_label_space = allocator_.allocate<HLabel>();
		HLabel* new_label_obj = new (new_label_space)HLabel;
		new_label_bucket[new_label] = new_label_obj;
		new_label_bucket[new_label]->label_id = new_label;
		new_label_bucket[new_label]->frame_parent = &hier_componets_[tgFrame];

		// //对应回去,标签是起始点,则保持不变

		map<IndexType,HVertex*> unMakePs;

		for (auto iter = splitedLabel->vertex_bucket.begin(); iter != splitedLabel->vertex_bucket.end(); )
		{
			IndexType prev_id = iter->second->prev_corr->label_parent[0]->label_id; //得不到最新的label_parent地址.
			IndexType vtx_id = iter->first;
			if (prev_id == eS)
			{
				iter->second->label_parent[0] = new_label_bucket[nodeId];
				++iter;

			}else if(prev_id == eE)
			{
				iter->second->label_parent[0] = new_label_bucket[new_label];

				new_label_bucket[new_label]->vertex_bucket.insert(*iter);

				hier_componets_[tgFrame].label_of_vtx[vtx_id ] = new_label;

				iter = splitedLabel->vertex_bucket.erase(iter);

			}else//一些待确定label的点
			{
				unMakePs.insert(*iter);
				iter = splitedLabel->vertex_bucket.erase(iter);
			}

		}

		//用随机取点产生的最小距离来判断不确定点属于哪个类.unmark 要么属于nodeid 要么属于new_label
		determinateUnmarkPoints(tgFrame,unMakePs,new_label_bucket,nodeId,new_label);


		//对这两个点进行加边操作,
		//node<--->new_node
		GraphEdgeProperty newEP;
		newEP.start_ = nodeId;
		newEP.end_ = nSize;
		newEP.index = newGraphEdgeSize;

		IndexType edgeKey = frame_index_to_key(newEP.start_,newEP.end_);
		//还没对index赋值

		newEP.edgePoints[edgeKey].insert(edgeCorrNextVtx.begin(),edgeCorrNextVtx.end());


		boost::add_edge(nodeId,nSize,newEP,*new_graph);

		//断定查找两个节点与其它节点进行连边操作只会出现 recordColapseEdges.size()次数.
		for (auto iter = collapseEdges.begin(); iter != collapseEdges.end(); iter ++)
		{

			GraphEdgeProperty glueEdge;

			glueEdge = (*iter);
			IndexType  sEdgeId = glueEdge.start_;
			IndexType  eEdgeId = glueEdge.end_;

			map<IndexType,HVertex*>  edgePoints;

			auto bIter = glueEdge.edgePoints.begin();
			edgePoints.insert(bIter->second.begin(),bIter->second.end() );

			map<IndexType,HVertex*> startVtx = new_label_bucket[sEdgeId]->vertex_bucket;
			map<IndexType,HVertex*> endVtx = new_label_bucket[eEdgeId]->vertex_bucket;
			map<IndexType,HVertex*> nodeVtx = new_label_bucket[nodeId]->vertex_bucket;
			map<IndexType,HVertex*> nSizeVtx = new_label_bucket[nSize]->vertex_bucket;

			ScalarType minNode = 0.0;
			ScalarType minSize = 0.0;


			if (sEdgeId < nodeId) //nodeid为终点
			{
				minDistBeTwoParts(tgFrame,startVtx,nodeVtx,minNode);
				minDistBeTwoParts(tgFrame,startVtx,nSizeVtx,minSize);

				if (minNode < minSize)
				{
					glueEdge.end_ = nodeId;
				}else
				{
					glueEdge.end_ = nSize;
				}

			}else//nodeid为起点
			{
				minDistBeTwoParts(tgFrame,endVtx,nodeVtx,minNode);
				minDistBeTwoParts(tgFrame,endVtx,nSizeVtx,minSize);

				if (minNode < minSize)
				{
					glueEdge.start_ = nodeId;
					glueEdge.end_ = eEdgeId;

				}else
				{
					glueEdge.start_ = eEdgeId;
					glueEdge.end_ = nSize;
				}

			}

			IndexType eKey = frame_index_to_key(glueEdge.start_,glueEdge.end_);

			glueEdge.edgePoints[eKey] = edgePoints;

			boost::add_edge(glueEdge.start_,glueEdge.end_,glueEdge,*new_graph);

		}//遍历collapse的边


		//一条边指导分裂之后,查看点的编号情况

		// char* out_label_file = "D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_multiview0804\\refine(8_18)\\0824ViewoutputLabels.txt";
		// wirteSplitGraphLables(out_label_file);
		//hier_componets_[tgFrame].hier_label_bucket[tgGraSize - 1] = new_label_bucket;

	}//遍历每条边,每条边都会使得new_graph增加一个新的节点


	hier_componets_[tgFrame].hier_label_bucket.push_back(new_label_bucket);

	hier_componets_[tgFrame].hier_graph.push_back(new_graph);//保存最新的graph



}

void DualwayPropagation::split_twoAjacent_graph_prev(IndexType srFrame, IndexType tgFrame)
{

	//首先更新srFrame的grpah.

}

void DualwayPropagation::split_nest_graph_prev(IndexType srFrame, IndexType tgFrame)
{
	if(srFrame == 1)
	{
		split_twoAjacent_graph_prev(srFrame,tgFrame);
		return;
	}

	while (srFrame > 0)
	{
		split_twoAjacent_graph_prev(srFrame,tgFrame);

		srFrame --;

		tgFrame --;
	}

	return;
}

void DualwayPropagation::splitAllSquenceGraph()
{
	for (auto cIter = components_.begin(); cIter != components_.end(); cIter++)
	{
		//最后一个节点不可以运算
		IndexType srFrame = cIter->first;
		IndexType tgFrame = srFrame + 1;

		split_twoAjacent_graph_next(srFrame,tgFrame );

		//split_nest_graph_prev(srFrame,tgFrame);
	}

}

IndexType DualwayPropagation::checkNextLabelBucket(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor)
{
	assert(edgePs.size() > 0);

	set<IndexType > labelS;
	labelS.clear();

	for (auto iter = edgePs.begin(); iter != edgePs.end(); iter ++)
	{
		IndexType corPId = iter->second->next_corr->vtx_id;
		//

		//IndexType TestnextVtxLabid = hier_components_[2].label_of_vtx[corPId];

		IndexType nextVtxLabid = iter->second->next_corr->label_parent[0]->label_id;

		labelS.insert(nextVtxLabid);

		HVertex* nextP = iter->second->next_corr;

		edgePsCor[corPId] = nextP;

	}

	if (labelS.size() > 1)
	{
		Logger<<"边界点指向了下个块的两个Label!.\n";

	}

	return (*labelS.begin() );
}

void DualwayPropagation::getNextCorVtx(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor)
{
	// 	for (auto iter = edgePs.begin(); iter != edgePs.end(); iter ++ )
	// 	{
	// 		HVertex& nextP = iter->sec
	// 	}
}
class UtilityClass;
void DualwayPropagation::read_data(char *label_name,char *corr_name)
{
	//read_label_file(label_name);
	read_label_file_hier(label_name);
	read_corres_file_hier(corr_name);
	init_labeles_graph_hier2();
	init_node_link();
	//UtilityClass utclass( *this);
	//utclass.smoothHierLabelGraphAndPushBack(1 ,0 ,5);
	initialLabelDisplay();
	wirteGraphLables(OUT_LABEL_FILE_GCOP , 1 );
 
	//UtilityClass utclass( *this);
	//set<IndexType> testset;
	//set<IndexType> nlabel;
	//testset.insert(1);
	//testset.insert(8);
	//testset.insert(5);
	//testset.insert(7);
	//testset.insert(4);
	//testset.insert(6);


	//nlabel= utclass.mergePatches( 1 ,testset);
	//Logger<<"new label "<< nlabel.size()<<std::endl;
	///*testset.clear();
	/*testset.insert(1);
	testset.insert(8);

	nlabel = utclass.mergePatches( 1 ,testset);
	Logger<<"new label "<< nlabel<<std::endl;
	testset.clear();
	testset.insert(4);
	testset.insert(6);

	nlabel = utclass.mergePatches( 1 ,testset);
	Logger<<"new label "<< nlabel<<std::endl;

	testset.clear();
	testset.insert(2);
	testset.insert(0);
	testset.insert(9);
	nlabel = utclass.mergePatches( 1 ,testset);
	Logger<<"new label "<< nlabel<<std::endl;
	testset.clear();
	testset.insert(4);
	testset.insert(6);
	testset.insert(9);
	nlabel = utclass.mergePatches( 1 ,testset);
	Logger<<"new label "<< nlabel<<std::endl;*/
		/*utclass.smoothHierLabelGraphAndPushBack(1 ,0 ,3);
		utclass.smoothHierLabelGraphAndPushBack(1 ,1 ,20);
		utclass.smoothHierLabelGraphAndPushBack(1 ,2,30);
		utclass.smoothHierLabelGraphAndPushBack(1 ,3 ,60);*/
	//read_corres_file_hier(corr_name);
	//split_twoAjacent_graph_next(1,2);

}

void DualwayPropagation::wirteSplitGraphLables(std::string filename)
{
	FILE* outfile = fopen(filename.c_str(),"w");
	for ( auto frame_iter = hier_componets_.begin();
		frame_iter != hier_componets_.end();
		frame_iter++ )
	{
		HFrame& frame = frame_iter->second;
		IndexType frame_idx = frame_iter->first;
		for ( auto label_iter = frame.label_of_vtx.begin(); 
			label_iter!=frame.label_of_vtx.end(); label_iter++  )
		{
			IndexType vtx_id = label_iter->first;
			IndexType label_idx = label_iter->second;
			fprintf(outfile, "%d %d %d\n", frame_idx, label_idx, vtx_id);

		}
	}

	fclose(outfile);
}

//************************************
// Method:    wirteGraphLablesAtTop
// Parameter: std::string filename   指定输出的文件地址
// 用于输出最顶层的图结构
//************************************
void DualwayPropagation::wirteGraphLablesAtTop(std::string filename )
{
	FILE* outfile = fopen(filename.c_str(),"w");
	SampleSet& smpset = SampleSet::get_instance();
	std::map<IndexType, HFrame>::iterator fbitr ,feitr;
	fbitr = hier_componets_.begin();
	feitr = hier_componets_.end();
	for( ; fbitr != feitr ; ++fbitr){
		IndexType frame_idx = fbitr->first;
		HFrame& cframe = hier_componets_[ fbitr->first]; 

		IndexType graph_size =  cframe.hier_label_bucket.size();

		std::vector<HLabel*>& cLabelbck = cframe.hier_label_bucket[graph_size-1];

		std::vector<HLabel*>::iterator hlabelbitr , hlabeleitr ;

		hlabelbitr = cLabelbck.begin();
		hlabeleitr = cLabelbck.end();
		vector< std::map<IndexType ,HVertex*>::iterator> vtxbeginitrs, vtxenditrs;
		vector<IndexType> labelVec;
		for( ; hlabelbitr!= hlabeleitr ;++ hlabelbitr){
			HLabel* pclabel = *hlabelbitr;
			IndexType label_idx = pclabel->label_id;
	/*		std::map<IndexType ,HVertex*>::iterator vtxvecbitr ,vtxveceitr;

			vtxvecbitr = pclabel->vertex_bucket.begin();
			vtxveceitr = pclabel->vertex_bucket.end();
			for( ; vtxvecbitr!= vtxveceitr ;++vtxvecbitr){

				IndexType vtx_id = vtxvecbitr->first;
				fprintf(outfile, "%d %d %d\n", frame_idx, label_idx, vtx_id);
			}*/
			labelVec.push_back( label_idx);
			vtxbeginitrs.push_back( pclabel->vertex_bucket.begin() );
			vtxenditrs.push_back( pclabel->vertex_bucket.end());

		}

		
		IndexType labelNum = labelVec.size();
		while(true){
			IndexType minVtxId = 10000000;
			IndexType corrLabel ;
			std::map<IndexType ,HVertex*>::iterator* corrbitr;

			IndexType  endCount = 0 ; //用于判断点是不是都已经输出完了
			for( IndexType labelid = 0 ; labelid < labelNum ;++ labelid){
				std::map<IndexType ,HVertex*>::iterator& vtxbitr = vtxbeginitrs[labelid];
				std::map<IndexType ,HVertex*>::iterator& vtxeitr = vtxenditrs[labelid];
				if(vtxbitr != vtxeitr){
					IndexType vtx_id = vtxbitr->first;
					if(vtx_id < minVtxId) {minVtxId = vtx_id;
					corrLabel = labelVec[labelid];
					corrbitr = &vtxbitr;}
					

				}else{
					++endCount;
				}

			}
			if( endCount == labelNum)break;  // 全部点都已经输出完
			++(*corrbitr);
			fprintf(outfile, "%d %d %d\n", frame_idx, corrLabel, minVtxId);

		}
		


	}

	fclose(outfile);

}

/*

*/
//************************************
// Method:    wirteGraphLables
// FullName:  DualwayPropagation::wirteGraphLables
// Parameter: std::string filename  指定输出的文件地址
// Parameter: IndexType _depth     指定要输出的图的层次，如果超过图的层次会跳过
//************************************
void DualwayPropagation::wirteGraphLables(std::string filename ,IndexType _depth)
{
	FILE* outfile = fopen(filename.c_str(),"w");
	SampleSet& smpset = SampleSet::get_instance();
	std::map<IndexType, HFrame>::iterator fbitr ,feitr;
	fbitr = hier_componets_.begin();
	feitr = hier_componets_.end();
	for( ; fbitr != feitr ; ++fbitr){
		IndexType frame_idx = fbitr->first;
			HFrame& cframe = hier_componets_[ fbitr->first]; 

			if ((_depth + 1) > cframe.hier_label_bucket.size())break;

			std::vector<HLabel*>& cLabelbck = cframe.hier_label_bucket[_depth];

			std::vector<HLabel*>::iterator hlabelbitr , hlabeleitr ;

			hlabelbitr = cLabelbck.begin();
			hlabeleitr = cLabelbck.end();
			for( ; hlabelbitr!= hlabeleitr ;++ hlabelbitr){
				HLabel* pclabel = *hlabelbitr;
				IndexType label_idx = pclabel->label_id;
				std::map<IndexType ,HVertex*>::iterator vtxvecbitr ,vtxveceitr;

				vtxvecbitr = pclabel->vertex_bucket.begin();
				vtxveceitr = pclabel->vertex_bucket.end();
				for( ; vtxvecbitr!= vtxveceitr ;++vtxvecbitr){

					IndexType vtx_id = vtxvecbitr->first;
					fprintf(outfile, "%d %d %d\n", frame_idx, label_idx, vtx_id);
				}

			}


	}

	fclose(outfile);

}


//************************************
// Method:    writeOrigCloudWithLabel
// Parameter: const std::string _filename  指定输出的文件地址
// 用于 把sampleset 中所有的数据按照label 格式输出 frameid- labelid - vtxid
// 主要用在 采样点的label 号传递到原点云的label号 后
//************************************
void DualwayPropagation::writeOrigCloudWithLabel( const std::string _filename)
{
	FILE* outfile = fopen(_filename.c_str(),"w");
	SampleSet& smpset = SampleSet::get_instance();
	IndexType smpsetSize = smpset.size();
	for( IndexType frame_idx = 0 ; frame_idx< smpsetSize ;++frame_idx){
		Sample& smp = smpset[frame_idx];
		IndexType vtxnums = smp.num_vertices();
		for( IndexType vtx_id = 0 ; vtx_id < vtxnums ;++vtx_id){
			IndexType label_idx = smp[vtx_id].label();

			fprintf(outfile, "%d %d %d\n", frame_idx, label_idx, vtx_id);

		}

	}

	fclose(outfile);
}
void DualwayPropagation::minDistBeTwoParts(IndexType cFrame, map<IndexType,HVertex*>& fPart,map<IndexType,HVertex*>& sPart, ScalarType& misDis)
{

	Sample& smp = SampleSet::get_instance()[cFrame];

	ScalarType diag = smp.getBox().diag();

	vector<ScalarType> totDis;

	for (auto fIter = fPart.begin(); fIter != fPart.end(); ++fIter)
	{
		for (auto sIter = sPart.begin(); sIter != sPart.end(); ++sIter)
		{
			PointType fVtx = smp.vertices_matrix().col(fIter->first);
			PointType sVtx = smp.vertices_matrix().col(sIter->first);

			ScalarType dis = (fVtx - sVtx).norm();

			if (dis < 0.5 * diag)
			{
				totDis.push_back(dis);
			}
		}
	}

	if (totDis.size() == 0)
	{
		misDis =  0.5*diag;
	}else
	{
		misDis = * min_element(totDis.begin(),totDis.end() );
	}


}

void DualwayPropagation::determinateUnmarkPoints(IndexType cFrame,map<IndexType,HVertex*>& unMarkPs,vector<HLabel*> oriLabelBucket,IndexType nodeId,IndexType newLabe)
{
	if (unMarkPs.size() < 1)
	{
		Logger<<"没有Unmark 点.\n";
		return;
	}

	Sample& smp = SampleSet::get_instance()[cFrame];

	for (auto iter = unMarkPs.begin(); iter != unMarkPs.end();)
	{
		PointType pCoor = smp.vertices_matrix().col(iter->first);

		ScalarType d2node = p2PatchAvgDis(cFrame,pCoor,oriLabelBucket[nodeId]->vertex_bucket);
		ScalarType d2size = p2PatchAvgDis(cFrame,pCoor,oriLabelBucket[newLabe]->vertex_bucket );

		//Logger<<"to Label "<<nodeId<<" distance  = "<<d2node<<endl;
		//Logger<<"to Label "<<newLabe<<" distance  = "<<d2size<<endl;

		if (d2node <= d2size)
		{

			oriLabelBucket[nodeId]->vertex_bucket.insert(*iter);

			hier_componets_[cFrame].label_of_vtx[iter->first] = nodeId;

			iter->second->label_parent[0] = oriLabelBucket[nodeId];

		}else
		{

			oriLabelBucket[newLabe]->vertex_bucket.insert(*iter);

			hier_componets_[cFrame].label_of_vtx[iter->first] = newLabe;

			iter->second->label_parent[0] = oriLabelBucket[newLabe];

		}

		iter = unMarkPs.erase(iter); 
	}

}
ScalarType DualwayPropagation::p2PatchAvgDis(IndexType cFrame, PointType& pCoor,map<IndexType,HVertex*> parthPs)
{
	ScalarType avgDis = 1e5;

	ScalarType randDis = 0.0;

	IndexType pSize = parthPs.size();

	assert(pSize > 0);

	IndexType randPsSize = 100;

	Sample& smp = SampleSet::get_instance()[cFrame];


	auto eIter = parthPs.end();

	while(randPsSize -- > 0 )
	{
		auto bIter = parthPs.begin();

		IndexType randPId = rand()%pSize;

		std::advance(bIter,randPId);

		if ( bIter !=  eIter)
		{
			IndexType tgPId = bIter->first;

			PointType tgCoor = smp.vertices_matrix().col(tgPId);

			randDis = (pCoor - tgCoor).norm();

			//avgDis += (pCoor - tgCoor).norm(); //平均值不合理

			if(randDis < avgDis)
			{
				avgDis = randDis;
			}
		}
	}


	return  1e2* avgDis;

}
void DualwayPropagation::copyLabelBucket(vector<HLabel*>& leftLabels, const vector<HLabel*>& oriLabels)
{
	assert(oriLabels.size() > 0);

	for (auto iter = oriLabels.begin(); iter != oriLabels.end(); iter ++)
	{
		IndexType LId = (*iter)->label_id;

		HLabel* new_label_space = allocator_.allocate<HLabel>();
		HLabel* new_label = new (new_label_space)HLabel((*iter)->label_id,(*iter)->frame_parent,(*iter)->vertex_bucket,(*iter)->prev_corr,(*iter)->next_corr);

		for (auto cvIter = new_label->vertex_bucket.begin(); cvIter!= new_label->vertex_bucket.end(); cvIter++)
		{
			cvIter->second->label_parent[0] = new_label;
		}

		leftLabels.push_back(new_label);
	}
}

void DualwayPropagation::initialLabelDisplay()
{
	IndexType  dep = 0;
	SampleSet& smpset = SampleSet::get_instance();
	std::map<IndexType, HFrame>::iterator fbitr ,feitr;
	fbitr = hier_componets_.begin();
	feitr = hier_componets_.end();
	for( ; fbitr != feitr ; ++fbitr){

			HFrame& cframe = hier_componets_[ fbitr->first]; 

			if (( dep + 1) > cframe.hier_label_bucket.size())break;
	
			std::vector<HLabel*>& cLabelbck = cframe.hier_label_bucket[dep];
		
			//std::vector<HLabel*>& cLabelbck = cframe.hier_label_bucket[_depth];
			std::vector<HLabel*>::iterator hlabelbitr , hlabeleitr ;

			hlabelbitr = cLabelbck.begin();
			hlabeleitr = cLabelbck.end();
			for( ; hlabelbitr!= hlabeleitr ;++ hlabelbitr){
				HLabel* pclabel = *hlabelbitr;
				std::map<IndexType ,HVertex*>::iterator vtxvecbitr ,vtxveceitr;
				vtxvecbitr = pclabel->vertex_bucket.begin();
				vtxveceitr = pclabel->vertex_bucket.end();
				for( ; vtxvecbitr!= vtxveceitr ;++vtxvecbitr){
					smpset[ fbitr->first][vtxvecbitr->second->vtx_id].set_label(pclabel->label_id);

				//	Logger<<"帧号 "<< fbitr->first<<"点号 "<<vtxvecbitr->second->vtx_id <<"labelid "<<pclabel->label_id<<std::endl;
				}

			}

	}

}

void DualwayPropagation::changedDepthAndDispaly( int _depth )
{
	
	SampleSet& smpset = SampleSet::get_instance();
	std::map<IndexType, HFrame>::iterator fbitr ,feitr;
	fbitr = hier_componets_.begin();
	feitr = hier_componets_.end();
	for( ; fbitr != feitr ; ++fbitr){
		//if(smpset[ fbitr->first].is_visible()){
			HFrame& cframe = hier_componets_[ fbitr->first]; 
			Logger<<" 层数"<< cframe.hier_label_bucket.size()<<std::endl;
			if ((_depth + 1) > cframe.hier_label_bucket.size()) continue;
Logger<<" 先在在第 "<< _depth <<" 层"<<std::endl;
				std::vector<HLabel*>& cLabelbck = cframe.hier_label_bucket[_depth];
Logger<<" vector<HLabel*> 大小"<< cLabelbck.size()<<std::endl;
			//std::vector<HLabel*>& cLabelbck = cframe.hier_label_bucket[_depth];
				std::vector<HLabel*>::iterator hlabelbitr , hlabeleitr ;

				hlabelbitr = cLabelbck.begin();
				hlabeleitr = cLabelbck.end();
				for( ; hlabelbitr!= hlabeleitr ;++ hlabelbitr){
					HLabel* pclabel = *hlabelbitr;
					std::map<IndexType ,HVertex*>::iterator vtxvecbitr ,vtxveceitr;
Logger<<"label id "<< pclabel->label_id<<" 点数："<< pclabel->vertex_bucket.size()<<std::endl;
					vtxvecbitr = pclabel->vertex_bucket.begin();
					vtxveceitr = pclabel->vertex_bucket.end();
					for( ; vtxvecbitr!= vtxveceitr ;++vtxvecbitr){
						smpset[ fbitr->first][vtxvecbitr->second->vtx_id].set_label(pclabel->label_id);
					}
				
				}


	//	}

	}

}

void DualwayPropagation::startframeAnimation( QTimer*&  p_qtimer)
{

	//创建定时器
	if( NULL == frameTimer ){
		frameTimer = p_qtimer;
		//startframe = hier_componets_.begin()->first;
		//endframe = startframe+ hier_componets_.size()-1;
		startframe  = 0;
		endframe = SampleSet::get_instance().size()-1;
		


	}
	//将定时器超时信号与槽(功能函数)联系起来
	/*if(! frameTimer->isActive()){
		frameTimer->start(50);
	}*/
	//开始运行定时器，定时时间间隔为1000ms
	

}
void DualwayPropagation::pauseframeAnimation()
{
	if( NULL != frameTimer ){
		if ( frameTimer->isActive() )
			frameTimer->stop();
	}

}
void DualwayPropagation::stopframeAnimation()
{
	if( NULL != frameTimer ){
		if ( frameTimer->isActive() )
			frameTimer->stop();
	}
	delete frameTimer;
	frameTimer = NULL;
}

void DualwayPropagation::excFrameAnimation()
{

	if( currentVisframe < (endframe ) ){
		++currentVisframe;
		for( IndexType  i = startframe ; i != (endframe+1) ; ++i){
			if( i == currentVisframe){
				SampleSet::get_instance()[i].set_visble(true);
			}else{
				SampleSet::get_instance()[i].set_visble(false);
			}
			
		}
		
	}else if( currentVisframe == (endframe ) ){
		currentVisframe = startframe;
		for( IndexType  i = startframe ; i != (endframe+1) ; ++i){
			if( i == currentVisframe){
				SampleSet::get_instance()[i].set_visble(true);
			}else{
				SampleSet::get_instance()[i].set_visble(false);
			}

		}
	}
}
void DualwayPropagation::startlayerAnimation()
{


}
void DualwayPropagation::stoplayerAnimation()
{


}

void DualwayPropagation::read_corres_file_hier(char* filename)
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
	//	Logger<<cur_frame<<" "<< cur_vtx_idx<<" "<<next_frame<< " " <<next_vtx_idx<<std::endl;
		if(stat==EOF)break;

		if( hier_componets_.find(cur_frame)==hier_componets_.end() || hier_componets_.find(next_frame)==hier_componets_.end())
			continue;

		IndexType label = hier_componets_[cur_frame].label_of_vtx[cur_vtx_idx];

		IndexType next_label = hier_componets_[next_frame].label_of_vtx[next_vtx_idx];

		HVertex& cur_vtx = *hier_componets_[cur_frame].hier_label_bucket[0][label]->vertex_bucket[cur_vtx_idx];

		if ( cur_frame+1 == next_frame  )
		{
			cur_vtx.next_corr = hier_componets_[next_frame].hier_label_bucket[0][ next_label]->vertex_bucket[next_vtx_idx];
		}
		else if (cur_frame-1 == next_frame)
		{
			cur_vtx.prev_corr = NULL;
			hier_componets_[next_frame].hier_label_bucket[0][ next_label]->vertex_bucket[next_vtx_idx];
			cur_vtx.prev_corr = hier_componets_[next_frame].hier_label_bucket[0][ next_label]->vertex_bucket[next_vtx_idx];
		}
	}

	for (auto fiter = hier_componets_.begin(); fiter != hier_componets_.end(); ++ fiter)
	{
		IndexType frame_id = fiter->first;
		vector<HLabel*> fHierBucket = hier_componets_[frame_id].hier_label_bucket[0];

		vector<HLabel*>::iterator  hlabelIter = fHierBucket.begin();

		map<IndexType,IndexType> vtxIndex;
		IndexType vtxLevel = 0;
		for (; hlabelIter != fHierBucket.end(); )
		{
			if (*hlabelIter == NULL)
			{
				hlabelIter = fHierBucket.erase(hlabelIter);
			}else
			{
				IndexType labelId = (*hlabelIter)->label_id;
				vtxIndex[labelId] = vtxLevel;

				++ vtxLevel;
				++ hlabelIter;
			}
		}

		hier_componets_[frame_id].hier_label_bucket[0] = fHierBucket;
		hier_componets_[frame_id].hier_label_vtxBucket_index.resize(1);

		hier_componets_[frame_id].hier_label_vtxBucket_index[0] = vtxIndex;

	}
}






