#include "propagate_withThread.h"


#include <iostream>
#include <fstream>
#include <QString>
#include <QFileDialog>
#include "main_window.h"


#include "merge_write_utilities.h"
#include "merge_propagate_write.h"
#include "time.h"

void PropagateThread::run()
{
	ProxyVisualLabel qinhua_single(
		"D:\\point_data\\qinghuadata\\standard\\single\\label_corr\\totLabelSmooth(15_24).txt",
		"D:\\point_data\\qinghuadata\\standard\\single\\label_corr\\totCorr(15_24).txt");

	/*ProxyStandaraAjustViewOriColorPly single_standard_ajustview(
	"D:\\point_data\\qinghuadata\\standard\\single\\",
	"single_standard_ajustview"
	);*/
	//ProxyStandaraAjustViewOriColorPly two_standard_ajustview(
	//	"D:\\point_data\\qinghuadata\\standard\\two\\",
	//	"two_standard_ajustview"
	//	);
	//
	//ProxyStandaraGrayColorPly pandaOriGrayply(
	//	"F:\\VisualcamerAndYaobing\\pink-panther2\\plywithnormandstandard\\",
	//	"pandaOriGrayply"
	//	);
	/*ProxyStandaraOriColorPly cabinetOriGrayply(
	"D:\\point_data\\plystandard\\cabinet20150904\\",
	"cabinetGrayply"
	);*/

	/*ProxyStandaraGrayColorPly carDepthOriGrayply(
	"D:\\point_data\\plystandard\\carDepth\\",
	"carDepthGrayply"
	);*/
	//ProxyStandaraAjustViewOriColorPly fansOriGrayply(
	//	"D:\\point_data\\plystandard\\fans_ply\\",
	//	"fansGrayply"
	//	);
	/*ProxyStandaraGrayColorPly fingerOriGrayply(
	"D:\\point_data\\plystandard\\finger\\",
	"fingerGrayply"
	);*/
	/*ProxyStandaraGrayColorPly girldanceOriGrayply(
	"D:\\point_data\\plystandard\\girldance\\",
	"girldanceGrayply"
	);*/
	/*ProxyStandaraGrayColorPly horseOriGrayply(
	"D:\\point_data\\plystandard\\horse\\",
	"horseGrayply"
	);*/
	//ProxyStandaraGrayColorPly mandanceOriGrayply(
	//"D:\\point_data\\plystandard\\mandance\\",
	//"mandanceGrayply"
	//);
	/*ProxyStandaraGrayColorPly robotDepthOriGrayply(
	"D:\\point_data\\plystandard\\robotDepth\\",
	"robotDepthGrayply"
	);*/
	/*ProxyStandaraOriColorPly shiqunOriGrayply(
		"D:\\point_data\\plystandard\\shiqun\\",
		"shiqunGrayply"
		);*/
	//ProxyStandaraGrayColorPly twogirlshakehandOriGrayply(
	//	"D:\\point_data\\plystandard\\twogirlshakehand\\",
	//	"twogirlshakehandGrayply"
	//	);


	 //mergeFile( 
		// "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\resetLabelsandCorr\\corr\\" ,
		// "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\resetLabelsandCorr\\twogirlshakehandcorr_0_173.txt" , 
		// "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\resetLabelsandCorr\\label\\" ,
		// "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\resetLabelsandCorr\\twogirlshakehandlabel_0_173.txt");


	/*IndexType i;
	for( i = 0; i< 9 ; ++i){
	char inpath[250];
	char outpath[250];
	sprintf( inpath , "H:\\povay_pointcloud\\point_data\\horse\\ply\\horse-gallop-ground-truth-ply\\%d.ply" ,i );
	sprintf( outpath , "H:\\povay_pointcloud\\point_data\\horse\\ply\\horse-gallop-ground-truth-ply\\newcolor%d.ply" ,i );
	writePlyFromPly(
	inpath,
	outpath,(ScalarType**)corrColormap);

	}

	return;*/
//	 writeIncFromPly( 
//"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\people_cut\\0.ply",
//"H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\people_cut\\inc\\twogirlshakehand38-part1.inc"
//		 );
//	 writeIncFromPly( 
//		 "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\twogirlDanceProOriMergeply030.ply",
//		 "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\inc\\twogirlshakehand30.inc"
//		 );
//	 writeIncFromPly( 
//		 "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\twogirlDanceProOriMergeply034.ply",
//		 "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\inc\\twogirlshakehand34.inc"
//		 );
//	 writeIncFromPly( 
//		 "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\twogirlDanceProOriMergeply038.ply",
//		 "H:\\povay_pointcloud\\point_data\\twogirlshakehand\\ply\\Merge\\neigbor100\\inc\\twogirlshakehand38.inc"
//		 );
//	 return;
	
	Proxy * mProxy ;
	mProxy = &qinhua_single;

	if(mProxy)mProxy->setContext(this);
	//Logger<< "propagate run "<<std::endl;
	clock_t old_time = clock();

	if(mProxy)mProxy->run();
	
	Logger<< "propagation time: "<<(size_t)((clock()-old_time)/CLOCKS_PER_SEC)<<"s"<<std::endl;


	//mProxy = &twogirlAfter;

	//mProxy->setContext(this);
	Logger<< "propagate run "<<std::endl;
	//mProxy->run();
	//mProxy = &twogirlMerge;

	//mProxy->setContext(this);
	//Logger<< "propagate run "<<std::endl;
	//mProxy->run();
	/*
	mProxy = &horsesplit;
	mProxy->setContext(this);
	mProxy->run();
	mProxy = &horseMerge;
	mProxy->setContext(this);
	mProxy->run();
*/
/*
	getDifferentLabelPoints( "H:\\povay_pointcloud\\point_data\\shiqun\\new_data\\beforepropagate5_10_15center5.txt" , 
		"H:\\povay_pointcloud\\point_data\\shiqun\\Propagation_Info\\Propagation_Info\\labeloutoutafterPropagate5_10_15center5.txt"
		, "H:\\povay_pointcloud\\point_data\\shiqun\\new_data\\differece5_10_15center5.txt");
	writeMutiSamplePovray("H:\\povay_pointcloud\\point_data\\shiqun\\new_data\\inc\\differece5_10_15center5.inc" ,
		"H:\\povay_pointcloud\\point_data\\shiqun\\new_data\\differece5_10_15center5.txt");
	//depthToCloud("H:\\qinghuapointcloud\\Couple-dance-data\\cam0\\raw_depth\\0000.txt", "H:\\qinghuapointcloud\\Couple-dance-data\\point.xyz");
	return;
*/

	//char* in_ = ".\\carbinet0904\\0904LabelsAndCorrCabinet\\0904LabelsAndCorr\\0904CabinetCorr00_0.90.txt";
	//char* out_ = ".\\carbinet0904\\0904LabelsAndCorrCabinet\\0904LabelsAndCorr\\testmerge.txt";
	//mergeFile();
	// writeMutiSamplePovray();
	

//	
//
	//DualwayPropagation::get_instance().read_label_file_hier( label_filename);
	//DualwayPropagation::get_instance().read_corres_file_hier(corr_filename);
//	mergeFile();



	//read_label_file(label_name);

	//dp_.read_label_file_hier(label_filename);
	//dp_.read_corres_file_hier(corr_filename);

	//dp_.init_labeles_graph_hier2();
	//for( int i = 10 ;i <14;++i){
	//	split_twoAjacent_graph_next( dp_,i ,i+1);

	//}
/*
	for( int i = 10 ;i > 0;--i){
		split_twoAjacent_graph_prev( dp_,i-1 ,i);

	}
	*/
	//用于输出 裂变后所有帧的label 
//	dp_.wirteGraphLablesAtTop("H:\\povay_pointcloud\\point_data\\cabinet20150904\\cabinetlabeloutputafterPropagate0_29center10.txt");
//	propagateLabel2Orignal();
//	dp_.writeOrigCloudWithLabel("H:\\povay_pointcloud\\point_data\\shiqun\\Propagation_Info\\Propagation_Info\\OrigCloudWithLabel0_29.txt");


//	writeMutiSamplePovray("H:\\povay_pointcloud\\point_data\\cabinet20150904\\inc\\cabinetlabeloutputafterPropagate0_29center10.inc" ,
//		"H:\\povay_pointcloud\\point_data\\cabinet20150904\\cabinetlabeloutputafterPropagate0_29center10.txt");

//	writeMutiSamplePovray("H:\\povay_pointcloud\\point_data\\shiqun\\new_data\\inc\\OrigCloudWithLabel0_29.inc" ,
//		"H:\\povay_pointcloud\\point_data\\shiqun\\Propagation_Info\\Propagation_Info\\OrigCloudWithLabel0_29.txt");

	/*for( int i = 17 ;i <24;++i){
	split_twoAjacent_graph_next( dp_,i ,i+1);

	}

	for( int i = 17 ;i > 0;--i){
	split_twoAjacent_graph_prev( dp_,i-1 ,i);

	}*/
	

//
//
//	//dp_.init_node_link();
//	//UtilityClass utclass( dp_);
//	//dp_.utclass.smoothHierLabelGraphAndPushBack(1 ,0 ,5);

	//dp_.initialLabelDisplay();
    
//
// 
	//Proxy_Propagate* mpp = dynamic_cast<Proxy_Propagate*>(mProxy);
	//visualCosegmentation( mpp->getLabelFileName());

}

void PropagateThread::visualAllvtx()
{
	SampleSet& smpSet = SampleSet::get_instance();
	IndexType frameNum =  smpSet.size();
	for ( IndexType i=0; i < frameNum;i++ )
	{
		for (IndexType j=0; j<smpSet[i].num_vertices(); j++)
		{
			smpSet[i][j].set_visble(true);
		}
	}

}
void PropagateThread::visualCosegmentation(char *labels_file)
{
	FILE* in_file = fopen(labels_file,"r");
	if (in_file==NULL)
	{
		return;
	}

	IndexType frame, label, vtx_idx;

	SampleSet& smpSet = SampleSet::get_instance();

	IndexType frameNum =  smpSet.size();

	for ( IndexType i=0; i < frameNum;i++ )
	{
		for (IndexType j=0; j<smpSet[i].num_vertices(); j++)
		{
			smpSet[i][j].set_visble(false);
		}
	}

	//IndexType frames[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 ,21,22 ,23 };

	//for ( IndexType i=0; i<(sizeof(frames)/sizeof(IndexType));i++ )
	//{
	//	for (IndexType j=0; j<smpSet[frames[i]].num_vertices(); j++)
	//	{
	//		smpSet[frames[i]][j].set_visble(false);
	//	}
	//}

	while (true)
	{
		int stat = fscanf(in_file, "%d %d %d\n",&frame, &label, &vtx_idx);
		if(stat==EOF)break;
		smpSet[frame][vtx_idx].set_visble(true);
		smpSet[frame][vtx_idx].set_label(label);
	}
	fclose(in_file);

}


void PropagateThread::split_twoAjacent_graph_next( DualwayPropagation& dp1 ,IndexType srFrame, IndexType tgFrame)
{
	//向前分裂

	//get the new graph of tgGrame--需要深拷贝


	Logger<<" .......\n";
	Logger<<"  Start next split.\n";
	IndexType tgGraSize = dp_.hier_componets_[tgFrame].hier_graph.size();
	LabelsGraph* oriGra = dp_.hier_componets_[tgFrame].hier_graph[tgGraSize - 1];
	LabelsGraph* new_graph = new LabelsGraph(*oriGra);

	//new_graph = oriGra;

	//vector<HLabel* > new_label_bucket =  hier_componets_[tgFrame].hier_label_bucket[tgGraSize - 1];
	vector<HLabel* > new_label_bucket;
	copyLabelBucket(new_label_bucket,dp_.hier_componets_[tgFrame].hier_label_bucket[tgGraSize - 1] );


	//
	IndexType gLevel = 0;

	IndexType srGraSize = dp_.hier_componets_[srFrame].hier_graph.size();

	gLevel  = srGraSize - 1;//获取最新的层
	LabelsGraph* srGraLat = dp_.hier_componets_[srFrame].hier_graph[gLevel];

	IndexType labParentsize = tgGraSize + 1; //生成的层数

	Logger<<srFrame<<"帧的第"<<gLevel<<"层边界分割"<<tgFrame<<"的"<<tgGraSize - 1 <<"层"<<endl;

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

		Logger<<"  边的起点为"<<eS<<"终点为"<<eE<<endl;

		IndexType recordS = 0;       
		IndexType recordE = 0;

		IndexType vtxBSzie = splitedLabel->vertex_bucket.size();
		for (auto iter = splitedLabel->vertex_bucket.begin(); iter != splitedLabel->vertex_bucket.end(); iter ++)
		{
			IndexType prev_id = iter->second->prev_corr->label_parent[gLevel]->label_id; //

			if (prev_id == eS)
			{
				recordS ++;

			}else if(prev_id == eE)
			{
				recordE ++;
			}
		}

		ScalarType ration = (ScalarType)(recordS + recordE)/vtxBSzie;

		//若分裂出来的点个数有一个数据很少,则该边不做裂变

		if ( recordE < 5 || recordS < 5 )
		{
			Logger<<"边界点太靠近,不需要分裂.\n";
			continue;
		}

		if (ration < 0.2)
		{
			Logger<<"Unmark点比值太大,暂时不分裂.\n";
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
		HLabel* new_label_space = dp_.allocator_.allocate<HLabel>();
		HLabel* new_label_obj = new (new_label_space)HLabel;
		new_label_bucket[new_label] = new_label_obj;
		new_label_bucket[new_label]->label_id = new_label;
		new_label_bucket[new_label]->frame_parent = &dp_.hier_componets_[tgFrame];

		// 		//对应回去,标签是起始点,则保持不变

		map<IndexType,HVertex*> unMakePs;

		for (auto iter = splitedLabel->vertex_bucket.begin(); iter != splitedLabel->vertex_bucket.end(); )
		{
			IndexType prev_id = iter->second->prev_corr->label_parent[gLevel]->label_id; //得不到最新的label_parent地址.
			IndexType vtx_id = iter->first;
			if (prev_id == eS)
			{
				//iter->second->label_parent.push_back( new_label_bucket[nodeId] );//每次赋了标签之后,都会添加一个地址,这样会导致父节点很多,而我们只需要最终的父节点地址
				iter->second->label_parent.resize(tgGraSize + 1);
				iter->second->label_parent[tgGraSize] =  new_label_bucket[nodeId];
				++iter;

			}else if(prev_id == eE)
			{
				//iter->second->label_parent.push_back(new_label_bucket[new_label] );
				iter->second->label_parent.resize(tgGraSize + 1);
				iter->second->label_parent[tgGraSize] = new_label_bucket[new_label] ;
				new_label_bucket[new_label]->vertex_bucket.insert(*iter);

				dp_.hier_componets_[tgFrame].label_of_vtx[vtx_id ] = new_label;

				iter = splitedLabel->vertex_bucket.erase(iter);

			}else//一些待确定label的点
			{
				unMakePs.insert(*iter);
				iter = splitedLabel->vertex_bucket.erase(iter);
			}

		}

		//用随机取点产生的最小距离来判断不确定点属于哪个类.unmark 要么属于nodeid 要么属于new_label
		determinateUnmarkPoints(tgFrame,unMakePs,new_label_bucket,nodeId,new_label,tgGraSize);


		//对这两个点进行加边操作,
		//node<--->new_node
		GraphEdgeProperty newEP;
		newEP.start_ = nodeId;
		newEP.end_ = nSize;
		newEP.index = newGraphEdgeSize;

		IndexType edgeKey = frame_index_to_key(newEP.start_,newEP.end_);
		//还没对index赋值
		//
	//	newEP.edgePoints[edgeKey].insert(edgeCorrNextVtx.begin(),edgeCorrNextVtx.end());

		map<IndexType, map<IndexType ,HVertex*> > nedgePoints;
		getEdgePoints( new_label_bucket[nodeId] , new_label_bucket[new_label] ,tgFrame,nedgePoints);

		newEP.edgePoints[edgeKey].insert(nedgePoints[edgeKey].begin(), nedgePoints[edgeKey].end());


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


	}//遍历每条边,每条边都会使得new_graph增加一个新的节点




	checkPsNewLabelParentPtr(new_label_bucket,labParentsize);//next dirction

	dp_.hier_componets_[tgFrame].hier_label_bucket.push_back(new_label_bucket);

	dp_.hier_componets_[tgFrame].hier_graph.push_back(new_graph);//保存最新的graph


	Logger<<"  Start next split.\n";
	Logger<<" .......\n";

}

void PropagateThread::getEdgePoints(  HLabel*  p_labell,  HLabel*  p_labelr ,IndexType _frameId ,map<IndexType, map<IndexType ,HVertex*> >& edgePoints )
{


	//	GraphEdgeProperty   gep;

		distanPriQueue PriQue;   //
		while ( ! PriQue.empty() ) PriQue.pop(); 



		map<IndexType,HVertex*> fir_vtx = p_labell->vertex_bucket;

		map<IndexType,HVertex*> sec_vtx = p_labelr->vertex_bucket;

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
				Sample& s = sample_set[_frameId];
				dia = s.getBox().diag();

				PointType point1 =  s.vertices_matrix().col(index1);

				IndexType index2 = biter2->first;

				PointType point2 =  s.vertices_matrix().col(index2);

				ScalarType distance = (point1 - point2).norm();
				if(distance < minDis)minDis = distance;
				IndexType label1 = p_labell->frame_parent->label_of_vtx[ index1];
				IndexType label2 = p_labelr->frame_parent->label_of_vtx[ index2];
				//IndexType label1 = hier_componets_[citer->first].label_of_vtx[ index1];
				//IndexType label2 = hier_componets_[citer->first].label_of_vtx[ index2];
				if( distance < dia*0.1){
					pointdistance tpd( index1 ,  label1 , index2 ,label2 ,distance );
					PriQue.push( tpd);
				}

			}

		}

		//if (minDis <  dia * 0.04 )//0.0588
		//{
		//	dp_.getEdgeVertexs2( _frameId ,PriQue ,gep.edgePoints);

		//	//边界点集个数太少也不需要添加,可能是噪声点.

		//	//gep.index = gep_count;
		//	IndexType i = p_labell->label_id;
		//	IndexType j = p_labelr->label_id;
		//	
		//	if( i < j)
		//	{
		//		gep.start_ = i;
		//		gep.end_ = j;
		//		boost::add_edge(i,j,gep , *new_labelGraph);
	
		//	}else if( i> j)
		//	{
		//		gep.start_ = j;
		//		gep.end_ = i;
		//		boost::add_edge( j, i ,gep ,*new_labelGraph);
	
		//	}else
		//	{
		//		// i == j 时不处理
		//	}

		//}

		
	
		std::map<IndexType,HVertex*> edgedps;
		pointdistance pd;
		for( IndexType i  = 10 ; i >0 ; --i){
			pd = PriQue.top();
			PriQue.pop();
			std::cout<<"vtx1Id: "<<pd.vtx1Id_ <<"    labelofvtx1:   "<<pd.labelofvtx1_<<"    vtx2:  "<<pd.vtx2Id_<<"   labelofvtx2:  "<<pd.labelofvtx2_<<std::endl;
			HVertex* tmpVtx;
			tmpVtx = p_labell->vertex_bucket[pd.vtx1Id_];
			Vertex& vtx1 = sample_set[_frameId][tmpVtx->vtx_id];
			vtx1.set_edge_point( true);          //用于绘制边界点
			vtx1.set_wrapbox(false);
			
			edgedps.insert( std::make_pair( pd.vtx1Id_ , tmpVtx) );
			tmpVtx = p_labelr->vertex_bucket[pd.vtx2Id_];
			Vertex& vtx2 =sample_set[_frameId][tmpVtx->vtx_id];
			vtx2.set_edge_point( true);
			vtx2.set_wrapbox(false);
		//	edgedps[ pd.vtx2Id_] = tmpVtx;
			edgedps.insert( std::make_pair( pd.vtx2Id_ , tmpVtx) );

			if( pd.labelofvtx1_ < pd.labelofvtx2_){
				vtx1.set_edgePointWithSmallLabel(true);
				vtx2.set_edgePointWithSmallLabel(false);
			}else{
				vtx1.set_edgePointWithSmallLabel(false);
				vtx2.set_edgePointWithSmallLabel(true);
			}


		}

		IndexType seKey = frame_label_to_key( pd.labelofvtx1_, pd.labelofvtx2_);
		edgePoints[ seKey] = edgedps;



}


void PropagateThread::split_twoAjacent_graph_prev( DualwayPropagation& dp ,IndexType srFrame, IndexType tgFrame)
{
	//获取需要更新的graph
	Logger<<" .......\n";
	Logger<<"  Start prev split.\n";

	IndexType srGraphSize = dp_.hier_componets_[srFrame].hier_graph.size();
	IndexType tgGraphSize = dp_.hier_componets_[tgFrame].hier_graph.size();

	assert(srGraphSize > 0 && tgGraphSize > 0);

	LabelsGraph* oriSpGra = dp_.hier_componets_[srFrame].hier_graph[srGraphSize - 1];
	LabelsGraph* shouldSplitGraph = new LabelsGraph(*oriSpGra);

	vector<HLabel* > new_label_bucket;
	copyLabelBucket(new_label_bucket,dp_.hier_componets_[srFrame].hier_label_bucket[srGraphSize - 1] );

	//获取指导分割的graph
	LabelsGraph* guideSplitGraph;
	IndexType graphLevel = 0;
	if (tgGraphSize > 1)
	{
		graphLevel = tgGraphSize - 2;
	}
	
	graphLevel = tgGraphSize -1 ;
	guideSplitGraph = dp_.hier_componets_[tgFrame].hier_graph[graphLevel];


	// guideSplitGraph的每条边引导一次分割
	pair<EdgeIterator,EdgeIterator> ei = boost::edges(*guideSplitGraph);

	for (EdgeIterator eit = ei.first; eit != ei.second; ++eit)
	{
		EdgeDescriptor ed = *eit;

		GraphEdgeProperty& ep = (*guideSplitGraph)[ed];

		map<IndexType,HVertex*> edgePoints;

		auto ePsIt = ep.edgePoints.begin();

		edgePoints.insert(ePsIt->second.begin(),ePsIt->second.end() );  

		if (edgePoints.size() < 1)
		{
			Logger<<"边上的顶点数太少,无法分裂.\n";
			continue;
		}

		map<IndexType,HVertex*> edgeCorrPrevVtx;

		IndexType nSize = boost::num_vertices(*shouldSplitGraph);

		IndexType newGraphEdgeSize = shouldSplitGraph->m_edges.size(); //为了给新增加的边添加序号

		bool isSplit = true;

		IndexType edgePsCorNode = 0;

		isSplit = checkPrevLabelBucket(edgePoints,edgeCorrPrevVtx,edgePsCorNode);

		if ( edgePsCorNode < 0 || edgePsCorNode > nSize - 1)
		{
			Logger<<"边界找到的块越界!.\n";
			break;
		}

		if (!isSplit)
		{
			Logger<<"边界点指向了两个块,不分割.\n";
			continue;
		}

		HLabel* splitedLabel = new_label_bucket[edgePsCorNode]; //可能需要分裂的块

		IndexType curEdgeStart = ep.start_;
		IndexType curEdgeEnd   = ep.end_;
		IndexType strCorPsSzie = 0;
		IndexType endCorPsSize = 0;

		//分类前做一个简单的预判断
		for (auto iter = splitedLabel->vertex_bucket.begin(); iter != splitedLabel->vertex_bucket.end(); iter ++)
		{
			if (iter->second->next_corr->label_parent[graphLevel] != NULL)
			{

				IndexType nextVtx_label = iter->second->next_corr->label_parent[graphLevel]->label_id; 

				// 				IndexType nextCor_id = iter->second->next_corr->vtx_id;
				// 
				// 				IndexType nextVtx_label = hier_componets_[tgFrame].label_of_vtx[nextCor_id];

				if (nextVtx_label == curEdgeStart)
				{
					strCorPsSzie ++;

				}else if(nextVtx_label == curEdgeEnd)
				{
					endCorPsSize ++;
				}
			}

		}

		IndexType  vtxBSize = splitedLabel->vertex_bucket.size();

		ScalarType ration = (ScalarType)(strCorPsSzie + endCorPsSize)/vtxBSize;

		if ( strCorPsSzie < 5 || endCorPsSize < 5)
		{
			Logger<<"边界点靠近边界(对应点个数),不需要分裂.\n";
			continue;
		}

		if ( ration < 0.2)
		{
			Logger<<"四两拨千斤?算了吧!.\n";
			continue;
		}

		//用eit 边开始分裂
		pair<VertexIterator, VertexIterator> vi = boost::vertices(*shouldSplitGraph);

		VertexIterator curNodeIter = (vi.first + edgePsCorNode);

		VertexDescriptor curNodeDesc = *curNodeIter;

		//节点对应的所有出边
		pair<OutEdgeIterator,OutEdgeIterator> nodeEiter = boost::out_edges(curNodeDesc,*shouldSplitGraph);

		map<IndexType,map<IndexType,HVertex*> > recordColapseEdges;

		set<GraphEdgeProperty> collapseEdges;

		OutEdgeIterator oit,nextIt;

		oit = nodeEiter.first;

		for (nextIt = oit; oit != nodeEiter.second; oit = nextIt )
		{
			++nextIt;

			EdgeDescriptor nextEdgeD = *oit;

			GraphEdgeProperty& nextEP = (*shouldSplitGraph)[nextEdgeD];

			collapseEdges.insert(nextEP);

			boost::remove_edge(*oit,*shouldSplitGraph);//删除这条边
		}

		//增加一个节点

		GraphVertexProperty vp(nSize,-1,-1);
		boost::add_vertex(vp,*shouldSplitGraph);

		//更新分割块信息,新增加的Label标号为nSize. 被分裂的点为edgePsCorNode
		IndexType new_label = nSize;
		new_label_bucket.push_back((HLabel*)0 );
		HLabel* new_label_space = dp_.allocator_.allocate<HLabel>();
		HLabel* new_label_obj = new (new_label_space)HLabel;
		new_label_bucket[new_label] = new_label_obj;
		new_label_bucket[new_label]->label_id = new_label;
		new_label_bucket[new_label]->frame_parent = &dp_.hier_componets_[tgFrame];

		//开始分裂
		map<IndexType,HVertex*> unMakePs;

		for (auto iter = splitedLabel->vertex_bucket.begin(); iter != splitedLabel->vertex_bucket.end(); )
		{
			if (iter->second->next_corr->label_parent[graphLevel])
			{
				IndexType nextVtx_label = iter->second->next_corr->label_parent[graphLevel]->label_id; //得不到最新的label_parent地址.

				// 				IndexType nextCor_id = iter->second->next_corr->vtx_id; 
				// 				IndexType nextVtx_label = hier_componets_[tgFrame].label_of_vtx[nextCor_id];

				IndexType vtx_id = iter->first;

				if (nextVtx_label == curEdgeStart)
				{
					//iter->second->label_parent.push_back(new_label_bucket[edgePsCorNode] );
					iter->second->label_parent.resize(srGraphSize + 1);
					iter->second->label_parent[srGraphSize] = new_label_bucket[edgePsCorNode];
					++iter;

				}else if(nextVtx_label == curEdgeEnd)
				{
					iter->second->label_parent.resize(srGraphSize + 1);

					iter->second->label_parent[srGraphSize] = new_label_bucket[new_label];

					//iter->second->label_parent.push_back(new_label_bucket[new_label] );

					new_label_bucket[new_label]->vertex_bucket.insert(*iter);

					dp_.hier_componets_[srFrame].label_of_vtx[vtx_id ] = new_label;

					iter = splitedLabel->vertex_bucket.erase(iter);

				}else//一些待确定label的点
				{
					unMakePs.insert(*iter);
					iter = splitedLabel->vertex_bucket.erase(iter);
				}
			}

		} //遍历需要分裂块的每个点

		//用随机取点产生的最小距离来判断不确定点属于哪个类.unmark 要么属于nodeid 要么属于new_label
		determinateUnmarkPoints(srFrame,unMakePs,new_label_bucket,edgePsCorNode,new_label,srGraphSize);

		//对这两个点进行加边操作,
		//node<--->new_node
		GraphEdgeProperty newEP;
		newEP.start_ = edgePsCorNode;
		newEP.end_ = nSize;
		newEP.index = newGraphEdgeSize;
		IndexType edgeKey = frame_index_to_key(newEP.start_,newEP.end_);
		newEP.edgePoints[edgeKey].insert(edgeCorrPrevVtx.begin(),edgeCorrPrevVtx.end());
		boost::add_edge(edgePsCorNode,nSize,newEP,*shouldSplitGraph);


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
			map<IndexType,HVertex*> nodeVtx = new_label_bucket[edgePsCorNode]->vertex_bucket;
			map<IndexType,HVertex*> nSizeVtx = new_label_bucket[nSize]->vertex_bucket;

			ScalarType minNode = 0.0;
			ScalarType minSize = 0.0;


			if (sEdgeId < edgePsCorNode) //nodeid为终点
			{
				minDistBeTwoParts(srFrame,startVtx,nodeVtx,minNode);
				minDistBeTwoParts(srFrame,startVtx,nSizeVtx,minSize);

				if (minNode < minSize)
				{
					glueEdge.end_ = edgePsCorNode;
				}else
				{
					glueEdge.end_ = nSize;
				}

			}else//nodeid为起点
			{
				minDistBeTwoParts(srFrame,endVtx,nodeVtx,minNode);
				minDistBeTwoParts(srFrame,endVtx,nSizeVtx,minSize);

				if (minNode < minSize)
				{
					glueEdge.start_ = edgePsCorNode;
					glueEdge.end_ = eEdgeId;

				}else
				{
					glueEdge.start_ = eEdgeId;
					glueEdge.end_ = nSize;
				}

			}

			IndexType eKey = frame_index_to_key(glueEdge.start_,glueEdge.end_);

			glueEdge.edgePoints[eKey] = edgePoints;

			boost::add_edge(glueEdge.start_,glueEdge.end_,glueEdge,*shouldSplitGraph);

		}//遍历collapse的边

	} //遍历引导分割图的每条边

	checkPsNewLabelParentPtr(new_label_bucket,srGraphSize + 1);

	dp_.hier_componets_[srFrame].hier_label_bucket.push_back(new_label_bucket);

	dp_.hier_componets_[srFrame].hier_graph.push_back(shouldSplitGraph);//保存最新的graph

	Logger<<"  End prev split.\n";
	Logger<<" .......\n";

}


void PropagateThread::copyLabelBucket(vector<HLabel*>& leftLabels, const vector<HLabel*>& oriLabels)
{
	assert(oriLabels.size() > 0);

	for (auto iter = oriLabels.begin(); iter != oriLabels.end(); iter ++)
	{
		IndexType LId = (*iter)->label_id;

		HLabel* new_label_space = dp_.allocator_.allocate<HLabel>();
		HLabel* new_label = new (new_label_space)HLabel((*iter)->label_id,(*iter)->frame_parent,(*iter)->vertex_bucket,(*iter)->prev_corr,(*iter)->next_corr);

		// 		for (auto cvIter = new_label->vertex_bucket.begin(); cvIter!= new_label->vertex_bucket.end(); cvIter++)
		// 		{
		// 			cvIter->second->label_parent = new_label;
		// 		}

		leftLabels.push_back(new_label);
	}

}

IndexType PropagateThread::checkNextLabelBucket(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor)
{
	assert(edgePs.size() > 0);

	set<IndexType > labelS;
	labelS.clear();

	for (auto iter = edgePs.begin(); iter != edgePs.end(); iter ++)
	{
		IndexType corPId = iter->second->next_corr->vtx_id;
		//

		//IndexType TestnextVtxLabid = hier_components_[2].label_of_vtx[corPId];
		IndexType labParSize = iter->second->next_corr->label_parent.size();
		IndexType updateLevel = labParSize - 1;

		IndexType nextVtxLabid = iter->second->next_corr->label_parent[updateLevel]->label_id;

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

bool PropagateThread::checkPrevLabelBucket(map<IndexType,HVertex*>& edgePs, map<IndexType,HVertex*>& edgePsCor, IndexType& isSplit)
{
	assert(edgePs.size() > 0);

	set<IndexType > labelS;
	labelS.clear();

	for (auto iter = edgePs.begin(); iter != edgePs.end(); iter ++)
	{
		IndexType labParSize =  iter->second->prev_corr->label_parent.size();

		IndexType updateLevel = labParSize - 1;

		IndexType corPId = iter->second->prev_corr->vtx_id;

		IndexType nextVtxLabid = iter->second->prev_corr->label_parent[updateLevel]->label_id;

		labelS.insert(nextVtxLabid);

		HVertex* nextP = iter->second->prev_corr;

		edgePsCor[corPId] = nextP;

	}

	if (labelS.size() > 1)
	{
		Logger<<"边界点指向了下个块的两个Label!.\n";

	}

	isSplit = (* labelS.begin() );

	return (labelS.size() == 1);
}



void PropagateThread::determinateUnmarkPoints(IndexType cFrame,map<IndexType,HVertex*>& unMarkPs,vector<HLabel*> oriLabelBucket,IndexType nodeId,IndexType newLabe,IndexType tgSize)
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

			dp_.hier_componets_[cFrame].label_of_vtx[iter->first] = nodeId;

			iter->second->label_parent.resize(tgSize + 1);
			iter->second->label_parent[tgSize] = oriLabelBucket[nodeId];

		}else
		{

			oriLabelBucket[newLabe]->vertex_bucket.insert(*iter);

			dp_.hier_componets_[cFrame].label_of_vtx[iter->first] = newLabe;

			//iter->second->label_parent.push_back(oriLabelBucket[newLabe] );
			iter->second->label_parent.resize(tgSize + 1);
			iter->second->label_parent[tgSize] = oriLabelBucket[newLabe] ;
		}

		iter = unMarkPs.erase(iter); 
	}

}

void PropagateThread::minDistBeTwoParts(IndexType cFrame, map<IndexType,HVertex*>& fPart,map<IndexType,HVertex*>& sPart, ScalarType& misDis)
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



// 确保每个顶点的labparent指针数目层数相同
void PropagateThread::checkPsNewLabelParentPtr(vector<HLabel*> oriLabelBucket,IndexType labParSize)
{
	IndexType vId = 0;
	for (auto vIter = oriLabelBucket.begin(); vIter != oriLabelBucket.end(); vIter ++,vId ++)
	{
		map<IndexType,HVertex*>& vtxBucket = (*vIter)->vertex_bucket;

		HVertex* fVtx = (*vtxBucket.begin()).second;

		IndexType vlfSize = fVtx->label_parent.size();

		if ( vlfSize < labParSize)
		{
			for (auto lIter =vtxBucket.begin(); lIter != vtxBucket.end(); lIter ++ )
			{
				(*lIter).second->label_parent.push_back(oriLabelBucket[vId] );
			}
		}

	}
}

ScalarType PropagateThread::p2PatchAvgDis(IndexType cFrame, PointType& pCoor,map<IndexType,HVertex*> parthPs)
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

void PropagateThread::propagateLabel2Orignal()
{
	Logger<<"设置源点label开始"<<std::endl;
	auto bitr = dp_.hier_componets_.begin();
	auto eitr = dp_.hier_componets_.end();
	for( ; bitr != eitr ; ++ bitr){
		Logger<<"第 "<< bitr->first<<"帧 开始"<<std::endl;
		Sample& csmp = SampleSet::get_instance()[ bitr->first];
		HFrame& cframe = bitr->second;
		vector<IndexType>		sampleVtxId;
		vector<IndexType>		label_smp;
		vector<IndexType>		label_ori( csmp.num_vertices() , 0);

		IndexType bucketSize = cframe.hier_label_bucket.size();
		vector<HLabel*>&  hlabelvec = cframe.hier_label_bucket[ bucketSize -1]; // 获取最新的一层
		IndexType labelNum = hlabelvec.size();

		auto hbitr  = hlabelvec.begin();
		auto heitr  = hlabelvec.end();
		for(  ;hbitr != heitr ; ++hbitr){
			map<IndexType , HVertex*>&  vtxbucket = (*hbitr)->vertex_bucket;
			
			auto vtxbitr = vtxbucket.begin();
			auto vtxeitr = vtxbucket.end();
			for( ; vtxbitr != vtxeitr ;++ vtxbitr){
				sampleVtxId.push_back( vtxbitr->second->vtx_id);
				label_smp.push_back( (*hbitr)->label_id);
			}
		}
		propagateLabel2Orignal( csmp , sampleVtxId ,label_smp ,label_ori ,labelNum);
		IndexType vtxNum =  csmp.num_vertices();
		for( IndexType  i = 0 ; i<vtxNum ; ++i){
			csmp[i].set_visble( true);
			csmp[i].set_label( label_ori[i]);
		}

		Logger<<"第 "<< bitr->first<<"帧 结束"<<std::endl;
	
	}
 
	Logger<<"设置源点label结束"<<std::endl;

}


void PropagateThread::propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_ori , IndexType _labelNum)
{
		
	//IndexType nCluster =  _labelNum;  //  分成两类，就设为2
	IndexType nCluster = 50;   //干脆直接取20好了，分配多点没问题
	map<IndexType,IndexType> smpLabel;
	map<IndexType,IndexType>::iterator IsValidIter;
	for (int i = 0; i < label_smp.size(); i++)
	{
		smpLabel.insert(make_pair(sampleVtxId[i],label_smp[i]));
	}

	const IndexType k = 250;  //100;
	IndexType neighbours[k];
	ScalarType dist[k];

	IndexType vtx_num = oriPC.num_vertices();
	IndexType result_label;


	for(IndexType vtx_id = 0; vtx_id < vtx_num; vtx_id ++)
	{
		vector<IndexType> recordLabelTime(nCluster,0);
		result_label = -1;
		oriPC.neighbours(vtx_id, k, neighbours, dist);
		for(IndexType neig_id = 0; neig_id < k; neig_id ++)
		{
			IsValidIter = smpLabel.find(neighbours[neig_id]);
			if(IsValidIter != smpLabel.end())
			{
				recordLabelTime[IsValidIter->second] += 1;
			}
		}
		for (int i = 0; i < nCluster; i++)
		{
			if(result_label < recordLabelTime[i])
			{
				label_ori[vtx_id] = i;
				result_label = recordLabelTime[i];
			}
		}

	}

	smpLabel.clear();
}

