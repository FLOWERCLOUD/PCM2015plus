#include "GCop.h"
#include"sample_set.h"

#include "InOutputDefinition.h"
//#define SHOW_SAMPLE
//#define OUTPUT_LABELS


GCop::GCop()
{
	gcNode = NULL;
	m_gc = NULL;
	m_data = NULL;
	m_smooth = NULL;
	m_nLabels = 2;

	m_nGraphNeig = 10;
	m_nCurNeig = 10;
	m_nExpansion = 1;
	m_nSwap = 1;
	m_nTradeOff = 1;
	m_cSigma = 1.;
    m_dSigma = 1.;
	m_nDiffu = 1.;
	m_centerF = 10;

	if (!m_optLabel.empty())
	{
		m_optLabel.clear();
	}
}

GCop::~GCop()
{
  if (gcNode)
  {
	  delete gcNode;
	  gcNode = NULL;
  }

  if (m_gc)
  {
	  delete m_gc;
	  m_gc = NULL;
  }

  if (m_data)
  {
	   delete [] m_data;
	   m_data = NULL;
  }

  if (m_smooth)
  {
	 delete [] m_smooth;
	 m_smooth = NULL;
  }

  m_optLabel.clear();
  m_smpId.clear();
  m_isSelect.clear();
  m_isEdgePoint.clear();
}
void GCop::run()
{
	Logger<<"Start GCoptimization!.\n";

#ifdef ONLY_THREE
	m_nDim = 3;
#else
	m_nDim = 6;
#endif // ONLY_THREE

	//refineSegm();//single frame processing
	
	coSegmentation();

   Logger<<"End GCoptimization.\n";
}


//refine single frame !
void GCop::refineSegm()
{
	gcNode = new GraphNodeCtr();//diff_labels(0) 

	char input_label_file[2048];
	char input_cor_file[2048];

	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hands\\labels_cor_info\\diff_labels(0_7).txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hands\\labels_cor_info\\diff_corres(0_7).txt");

	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\测试gco\\收集采样点聚类结果\\girl4_labels15_01.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\测试gco\\收集采样点聚类结果\\girl4_corr15.txt");

	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\用plan with norm测试\\plan_labels01.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\用plan with norm测试\\plan_corr01.txt");


	//2015-4-4
	//nLabels = 3
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\测试gco\\15_13_17序列\\edit_labels15.txt");
	////gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\测试gco\\15_13_17序列\\ori_labels15.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\测试gco\\15_13_17序列\\corr15.txt");


	//扫描的手模型-4类
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\测试gco\\hand2_dis\\labels03.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\测试gco\\hand2_dis\\corr03.txt");

	//虚拟扫描的马-4类
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\用horse数据进行测试\\edit_labels03_4.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\用horse数据进行测试\\corr03.txt");

	//网格模型的dancer-girl 5类C:\Users\kobe\Desktop\Trajectory_data\human_dance
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\human dancer 数据\\labels03.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\human dancer 数据\\corr03.txt");
	
	//:C:\Users\kobe\Desktop\Trajectory_data\Scanning virtual camera data\hip_hop part(0_9);3类

	//sprintf(input_label_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hip_hop测试\\labels_smooth_sample03.txt");
	//sprintf(input_cor_file,"D:\\desk_file\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hip_hop测试\\corr03.txt");

	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hip_hop测试\\0418_hiphop_outputlabels03.txt");
	//m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hip_hop测试\\labels_smooth_sample03.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hip_hop测试\\corr03.txt");


	//C:\Users\kobe\Desktop\Trajectory_data\girl_shake_hand ;2类
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_shake_hand手伸展\\2(3)_labels10_0.8.txt");//2类
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_shake_hand手伸展\\0418_smooth_outputlabels10.txt");//3类
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_shake_hand手伸展\\corr10.txt");
	


	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_shake_hand手伸展\\2(3)_labels10_0.8.txt");//2类
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\human pose\\labels10_0.70_4.txt");//4类
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\human pose\\corr10.txt");
	
	//box_edit_1 2类:C:\Users\kobe\Desktop\Trajectory_data\box_edited_1----------用的最多
	
	//sprintf(input_label_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\0527labels_smp18_0.70.txt");
	//sprintf(input_cor_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\0527_corr18_0.70.txt");


	//sprintf(input_label_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\0411output_labels10_smooth.txt");
	//sprintf(input_cor_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\corr10.txt");

	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\edit(2)_labels10.txt");
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\0411output_labels10_smooth.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\corr10.txt");


	//box_edit_1 2类:C:\Users\kobe\Desktop\Trajectory_data\box_edited_1
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\第五帧\\labels05.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\第五帧\\corr05.txt");


	//测试旋转矩阵-testdata nCluster = 2
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\test data\\labels.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\test data\\corr.txt");


	//girl walk nCluster = 4
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\4_labels30.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\corr30.txt");

	//hand_dis ncluster = 5
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand_dis\\labels04.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand_dis\\corr04.txt");
	

	//girl walk  ncluster = 7
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\15帧数据\\0420_labels15_7seg.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\15帧数据\\0420_corr15.txt");


	//girl walk  ncluster = 5
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\第九帧单帧分割实验\\labels09.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\第九帧单帧分割实验\\corr09.txt");

	//m_nLabels = 2;

	//IndexType nodeNum = gcNode->node_map.size();


	//box_edit_1 2类:C:\Users\kobe\Desktop\Trajectory_data\box_edited_1===第九帧
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\第九帧\\labels09.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\橱柜模型box_edit_1\\第九帧\\corr09.txt");


	//hand_up_down
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand_up_down\\3_labels20.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand_up_down\\corr20.txt");


	//hand_dis 15frames ncluster = 3
	//gcNode->read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand_dis\\添加帧后数据\\labels05.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand_dis\\添加帧后数据\\corr05.txt");


    // girl walk 
    //sprintf(input_label_file,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\(20-30)32_聚类结果\\0427walk_labels_smp%2d_0.70.txt",m_centerF);
	//sprintf(input_cor_file,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\(20-30)32_对应信息\\0427_corr%2d_0.70.txt",m_centerF);

    //m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\(20-30)32_聚类结果\\0427walk_labels_smp20_0.70.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\(20-30)32_对应信息\\0427_corr20_0.70.txt");
	

   //sprintf(input_label_file,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\(10-19)32_细化后样本聚类信息\\labels_smooth_sample%2d.txt",m_centerF); 
   //sprintf(input_cor_file,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_walk\\(10-19)32_对应信息\\0430_corr%2d.txt",m_centerF);

    

    //girl dancer(60-90)
	//m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\dancer  girl(60-90)\\0429walk_labels_smp22_0.65.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\dancer  girl(60-90)\\0429_corr22_0.70.txt");
 
     //64
    //sprintf(input_label_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\dancer  girl(60-90)\\15-24\\refine sample\\labels_smooth_sample%2d.txt",m_centerF);
	//sprintf(input_cor_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\dancer  girl(60-90)\\15-24\\correspondence\\0430_corr%2d_0.70.txt",m_centerF);
    //m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\dancer  girl(60-90)\\64\\0502walk_labels_smp16_0.70.txt");
    //gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\dancer  girl(60-90)\\64\\0430_corr16_0.70.txt");
    
    //girl  rotate
    //m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_rotate\\labels13.txt");
    //gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\girl_rotate\\corr13.txt");


    //horse-scanning-inter
    //sprintf(input_label_file,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\horse(1-20)\\horse_smooth_sample%2d.txt",m_centerF);
	//sprintf(input_cor_file,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\0506_corr%2d_0.70.txt",m_centerF);
	//m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\horse(1-20)\\horse_smooth_sample09.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\0506_corr09_0.70.txt");


	//0509
	//m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand skeleton\\smooth_sample10.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\hand skeleton\\corr10.txt");

	//0511
	//sprintf(input_label_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\mesh4_down\\refine_mehs4\\0511_smooth_sample%2d.txt",m_centerF);
	//sprintf(input_cor_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\mesh4_down\\0511_corr%2d_0.70.txt",m_centerF);
	//m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\mesh4_down\\refine_mehs4\\0511_smooth_sample18.txt");
	//gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\mesh4_down\\0511_corr18_0.70.txt");


    //m_nLabels = gcNode->readnLabelFile("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\fans\\0514_smooth_sample23.txt");
    //gcNode->read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\fans\\0513_corr23_0.70.txt");


    sprintf(input_label_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\car\\0803labels_car03_0.60.txt");
    sprintf(input_cor_file,"D:\\desk_file\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\car\\0803corr_car03_0.60.txt");

    m_nLabels = gcNode->readnLabelFile(input_label_file);
    gcNode->read_corres_file(input_cor_file); 
	IndexType nodeNum = gcNode->node_vec.size();
	m_gc = new GCoptimizationGeneralGraph(nodeNum,m_nLabels,gcNode);



 	//setNeighbor();//set the initial label  -- the index using graph_index of node
    setNeighborSmpTree(); 

	//ransacAlgorithm(gcNode->node_vec);
    //emAlgoritm(gcNode->node_vec);
 	//ransacAllAlgorithm(gcNode->node_vec);//gaussian distribution  
 	//ransacRotTan(gcNode->node_vec);//using rotate matrix
 	    
#ifdef EXPSWAP

   	MatrixXX totError;
   	ransacMultiRotTan(gcNode->node_vec,totError); //meanwhile set data items
       
 //          	unordered_map<IndexType, set<IndexType>> edgePoints;
 //          	findEdgePoint(edgePoints);
 //          
 //          	//visualEdgePoints(edgePoints);//可视化边界点
 //          	setDataItem(totError,edgePoints);
   
         	  setSmoothItem();
         	  m_gc->setLabelOrder(true);
         
    		  m_gc->expansion(m_nExpansion);
    		  //m_gc->alpha_expansion(m_nExpansion);
    
    		  m_gc->swap(m_nSwap);
        	  //m_gc->alpha_beta_swap(1,2);
#else
	ransacMultiRotTan(gcNode->node_vec);//利用多帧间的平均误差---用来可视化
#endif // Expansion



  	visulizationLabels();


}

//-----------------------------------------
void GCop::setParamter(IndexType _nlabel,IndexType _nExpansion,IndexType _nSwap,IndexType _nGNeig,IndexType _nCNeig,
					   ScalarType _nTraOf,ScalarType _CSigma,ScalarType _DSigma,ScalarType _nDiffu,IndexType centerF)
{
	m_nLabels = _nlabel;
	m_nExpansion = _nExpansion;
	m_nSwap = _nSwap;
	m_nGraphNeig = _nGNeig;
	m_nCurNeig = _nCNeig;
	m_nTradeOff = _nTraOf;
	m_cSigma = _CSigma;
	m_dSigma = _DSigma;
	m_nDiffu = _nDiffu;
	m_centerF = centerF;
}
//-----------------------------------------

void GCop::setNeighborSmpTree()
{
	IndexType * neighbours = new IndexType[m_nGraphNeig];

	ScalarType * dist = new ScalarType [m_nGraphNeig];

	IndexType fId = (gcNode->node_vec)[0]->frame;

	Sample & curF = gcNode->m_smpSet[fId];

	m_isSelect.resize(curF.num_vertices(),false);

	m_smpId.resize(m_gc->numSites());

	unordered_map<IndexType,bool> neigh_pair;

	IndexType nodeId = 0;

	Sample* downSmp = new Sample;

	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		Vertex& vtx = curF[(*iter)->index];

		PointType v( vtx.x(), vtx.y(), vtx.z() );
		ColorType cv(vtx.r(), vtx.g(), vtx.b(), vtx.alpha());
		NormalType nv(vtx.nx(), vtx.ny(), vtx.nz());

		downSmp->add_vertex(v,nv,cv);
	}

	downSmp->build_kdtree();

	nodeId = 0;

	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		m_isSelect[(*iter)->index] = true;

		m_smpId[nodeId] = (*iter)->index;

		m_gc->setLabel((*iter)->graph_index,(*iter)->label);

		downSmp->neighbours((*iter)->graph_index,m_nGraphNeig,neighbours,dist);

		for (IndexType neig_id = 1; neig_id < m_nGraphNeig; neig_id ++)
		{
			bool temp = neigh_pair[neighbours[neig_id],(*iter)->graph_index];

			if (!temp)
			{
			    m_gc->setNeighbors((*iter)->graph_index,neighbours[neig_id] );

				neigh_pair[(*iter)->graph_index,neighbours[neig_id]] = true;
			}

		}

	}

	delete [] neighbours;

	delete [] dist;

	delete downSmp;

	neigh_pair.clear();

}
//
void GCop::setNeighbor()
{
	IndexType * neighbours = new IndexType[m_nGraphNeig];
	ScalarType * dist = new ScalarType [m_nGraphNeig];

	IndexType fId = (gcNode->node_vec)[0]->frame;

	Sample & curF = gcNode->m_smpSet[fId];

	m_isSelect.resize(curF.num_vertices(),false);
	m_smpId.resize(m_gc->numSites());

	unordered_map<IndexType,bool> neigh_pair;

	IndexType nodeId = 0;

	vector<ScalarType> all_dist;
	vector<ScalarType> all_curvature;

	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		m_isSelect[(*iter)->index] = true;

		m_smpId[nodeId] = (*iter)->index;

		curF.neighbours((*iter)->index,m_nGraphNeig,neighbours,dist);

		
		m_gc->setLabel((*iter)->graph_index,(*iter)->label);

	  // // this node's curvature
 		//ScalarType cva_1 = rationCurNode(fId,(*iter)->index);
 
 		////Logger<<"cva_1 = "<<cva_1<<endl;
 
 		//PointType sPoint = curF.vertices_matrix().col((*iter)->index);

		for (IndexType neig_id = 1; neig_id < m_nGraphNeig; neig_id ++)
		{
			IndexType pId = neighbours[neig_id];
			GraphCutNode *fNode = gcNode->node_map[frame_index_to_key(fId,pId)];

			if(fNode)
			{
			    bool temp = neigh_pair[frame_index_to_key(fNode->graph_index,(*iter)->graph_index)];

				if (!temp)//重复添加了邻域边?
				{

	     		   m_gc->setNeighbors((*iter)->graph_index,fNode->graph_index);//因为在优化能量项中 数据从0下标开始

				   neigh_pair[frame_index_to_key((*iter)->graph_index,fNode->graph_index)] = true;

   				   //ScalarType cva_2 = rationCurNode(fId,pId);
   
   				   ////Logger<<"cva_2 = "<<cva_2<<endl;
   
   				   //all_curvature.push_back(max(1/cva_1,1/cva_2));
   
   				   //PointType tPoint = curF.vertices_matrix().col(pId);
   
   				   //all_dist.push_back((tPoint-sPoint).norm());

				}
			
			}
		}
	}

	//m_gc->printfNeig();

	//MatrixXX stats;
	//stats.setZero(all_dist.size(),2);
	//for (int i = 0; i < all_dist.size(); i++)
	//{
	//	stats(i,0) = all_curvature[i];
	//	stats(i,1) = all_dist[i];
	//}

	//ScalarType sig_cur,sig_dis;

	//MatrixXX mean_two,sig_two;
	//mean_two.setZero(1,2);
	//sig_two.setZero(2,2);

	//Logger<<"sum = "<<stats.col(0).sum()<<endl;
	//Logger<<"sum = "<<stats.col(1).sum()<<endl;

	//mean_two(0,0)  = stats.col(0).sum()/all_dist.size();
	//mean_two(0,1) = stats.col(1).sum()/all_dist.size();

	//Logger<<"mean(0,0) = "<<mean_two(0,0)<<endl;
	//Logger<<"mean(0,1) = "<<mean_two(0,1)<<endl;

	//stats.rowwise()-= mean_two.row(0);

	//sig_two = stats.transpose() * stats;

	//sig_two /= all_dist.size();

	//Logger<<"sig convarance = "<<endl;
	//Logger<<sig_two<<endl;
 

	delete [] neighbours;
	delete [] dist;

	neigh_pair.clear();
}

//-----------------------------------------
ScalarType GCop::rationCurNode(IndexType frame,IndexType index)
{
	ScalarType totCur = 0.0;

	Sample& curF = gcNode->m_smpSet[frame];

	const IndexType k = 10;

	MatrixX3	k_nearest_verts(k, 3);
	IndexType		neighbours[k];
	ScalarType dist[k];

	curF.neighbours( index, k, neighbours, dist );

	for ( IndexType j=0; j<k; j++ )
	{
		IndexType neighbour_idx = neighbours[j];

		k_nearest_verts.row(j) << curF[neighbour_idx].x(), curF[neighbour_idx].y(), curF[neighbour_idx].z();
	}

	MatrixX3 vert_mean = k_nearest_verts.colwise().mean();
	MatrixX3 Q(k, 3);
	for (  IndexType j=0; j<k;j++)
	{
		Q.row(j) = k_nearest_verts.row(j) - vert_mean;
	}

	Matrix33 sigma = Q.transpose() * Q;

	Eigen::EigenSolver<Matrix33> eigen_solver(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);

	auto ev = eigen_solver.eigenvectors();
	auto eval = eigen_solver.eigenvalues();
	ScalarType tmp[3] = { eval(0).real(),eval(1).real(),eval(2).real() };
	IndexType min_idx = std::min_element(tmp,tmp+3) - tmp;

	ScalarType deno = tmp[0] + tmp[1] + tmp[2];

	if (min_idx == 0)
	{
		totCur = tmp[0]/deno;
	}else if(min_idx == 1)
	{
		totCur = tmp[1]/deno;
	}else
	{
		totCur =  tmp[2]/deno;
	}

	totCur = max(totCur,0.0015f);

	return totCur;
}
//-----------------------------------------
void GCop::setDataItem()
{
	IndexType numLabes = m_gc->numLabels();

	//m_data = new float[m_gc->numSites() *numLabes];

 	IndexType itNum = 0;
 	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,itNum++)
 	{
 		for (IndexType labelId = 0; labelId < numLabes; labelId ++)
 		{
 			if ((*iter)->label == labelId)
 			{
 				//m_data[itNum * numLabes + labelId] = 1;      //method 1
 				m_gc->setDataCost((*iter)->graph_index,labelId,-log(0.8)); //method 2
 			}else
 			{
 				//m_data[itNum * numLabes + labelId] = 5;
 				m_gc->setDataCost((*iter)->graph_index,labelId,-log(0.2));
 			}
 
 		}
 	}

	////m_gc->setDataCost((GCoptimization::EnergyTermType*)m_data);


// 	GCoptimization::DataCostFunctor* fn = new GCoptimizationGeneralGraph::dataCostFn(m_gc,m_nCurNeig);
// 	m_gc->setDataCostFunctor(fn);
}

//----------------------------------------------
void GCop::setDataItem(MatrixXX&totError,unordered_map<IndexType,set<IndexType> >& edgePoints )
{
	IndexType nSamples = totError.rows();
	IndexType nLabels = totError.cols();
	Sample & curF = gcNode->m_smpSet[m_curFId];

	//计算每条边对应的重心,每个点找到一个最近的重心,对应的(自己label,别人label)
	unordered_map<IndexType,PointType> edgeMeanCoor;
	calculateEdgeMean(edgePoints,edgeMeanCoor);

	//每个点对应一个相邻的label
	vector<IndexType> neiglabel;
	neiglabel.resize(nSamples,-1);
	for (IndexType i = 0; i < nSamples; i ++)
	{
		IndexType ownLabel = gcNode->node_vec[i]->label;
		PointType pCoor = curF.vertices_matrix().col(gcNode->node_vec[i]->index);

		ScalarType dist = 0.;
		unordered_map<IndexType,PointType>::iterator mit = edgeMeanCoor.begin();

		ScalarType * dist2Edge = new ScalarType[edgeMeanCoor.size()];
		IndexType edgeRor = 0;
		for (; mit != edgeMeanCoor.end(); mit ++,edgeRor ++)
		{
			PointType  eMean  = mit->second;
			ScalarType tempDist = (eMean - pCoor).norm();
			dist2Edge[edgeRor] = tempDist;
		}

		IndexType min_id = std::min_element(dist2Edge,dist2Edge + edgeMeanCoor.size()) - dist2Edge;

		mit = edgeMeanCoor.begin() ;
		while(min_id -- > 0){mit ++;}
		IndexType keyLL = mit->first;
		
		IndexType minLabel = get_frame_from_key(keyLL);
		IndexType maxLabel = get_label_from_key(keyLL);

		if ( ownLabel == minLabel)
		{
			neiglabel[i] = maxLabel;
		}else if(ownLabel == maxLabel)
		{
			neiglabel[i] = minLabel;
		}else
		{
			Logger<<" find near edge wired????"<<endl;
		}
		delete [] dist2Edge;

	}


	//最近邻可视化
	//m_optLabel.resize(nSamples,0);
// 	for (IndexType i = 0; i < nSamples; i ++)
// 	{
// 	   //m_optLabel[i] = neiglabel[i];
// 	   m_gc->setLabel(i,neiglabel[i]);
// 	}
	//误差做归一化

	for (IndexType i = 0; i < nSamples; i ++)
	{
		IndexType ownLabel = gcNode->node_vec[i]->label;
		IndexType neigLabel = neiglabel[i];
// 		ScalarType totErr = totError(i,ownLabel) + totError(i,neigLabel);
// 		totError(i,ownLabel) /= totErr;
// 		totError(i,neigLabel) /= totErr;

		for ( IndexType j = 0; j < nLabels; j++)
		{
		   if ( j == ownLabel || j == neigLabel) continue;
           totError(i,j) = 1e5;
		}
	}

	//设置数据项
//  	char outputLabelName[256]; 
//  	sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\NSmo526_Middle_error_box.txt");
//  	FILE *in_label = fopen(outputLabelName,"w");

	for (IndexType i = 0; i < nSamples; i ++)
	{
		for ( IndexType j = 0; j < nLabels; j++)
		{
			//fprintf(in_label,"%f ",totError(i,j));
			m_gc->setDataCost(i,j,m_nCurNeig *totError(i,j));
		}
		//fprintf(in_label," \n");
	}
	//fclose(in_label);
}
//----------------------------------------------
void GCop::calculateEdgeMean(unordered_map<IndexType, set<IndexType>> & edgePoints,unordered_map<IndexType,PointType>&edgeMeanCoor)
{
	Sample & curF = gcNode->m_smpSet[m_curFId];

    unordered_map<IndexType,set<IndexType> >::iterator mit = edgePoints.begin();
	for (; mit != edgePoints.end(); mit ++)
	{
		IndexType keyLL = mit->first;
		set<IndexType> Points = mit->second;

		IndexType sSize = Points.size();
		Matrix3X eCoor;
		eCoor.setZero(3,sSize);

		IndexType nId = 0;
		for (set<IndexType>::iterator sit = Points.begin(); sit != Points.end(); sit ++,nId ++)
		{
			IndexType pId = (*sit);
			eCoor.col(nId) = curF.vertices_matrix().col(pId);
		}

		PointType mCoor = eCoor.rowwise().mean();

		edgeMeanCoor[keyLL] = mCoor;

	}
}
//----------------------------------------------
void GCop::set2DataItem(vector<GraphCutNode*>& oriData)
{

	IndexType nSamples = oriData.size();
	//int nCluster = 2;//box
	int nCluster = m_nLabels;

	vector<IndexType> labels;
	vector<IndexType> nLabels;
	nLabels.resize(nCluster,0);
	labels.resize(nSamples,0);

	getInitLabels(oriData,labels,nLabels);

//
	MatrixXX samples;
	productionSamples(oriData,samples);

//
	MatrixXX mean0;
	MatrixXX cov0;
	MatrixXX cov1;
	MatrixXX pro0;
	mean0.setZero(nCluster,m_nDim);
	cov0.setZero(nCluster,m_nDim*m_nDim);
	pro0.setZero(nSamples,nCluster);
	//calculateMean(samples,mean0,cov0,cov1,pro0,labels,nLabels);

//
	IndexType numLabes = m_gc->numLabels();

	IndexType itNum = 0;
	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,itNum++)
	{
		for (IndexType labelId = 0; labelId < numLabes; labelId ++)
		{
			ScalarType val = dist2Mean(samples,mean0,itNum,labelId);
			//Logger<<"label = "<<labelId<< "val = "<<val<<endl;
			m_gc->setDataCost((*iter)->graph_index,labelId,val); //method 2
		}
	}

}

//----------------------------------------------
ScalarType GCop::dist2Mean(MatrixXX& _sample,MatrixXX& _mean,IndexType Itid,IndexType LabelId)
{
	assert(Itid >= 0 && LabelId >= 0);

	ScalarType dist = 0.;
	MatrixXX smp = _sample.row(Itid);
	MatrixXX mean_ = _mean.row(LabelId);
	dist = (smp - mean_).norm();

	return dist;
}
//----------------------------------------------
void GCop::setSmoothItem()
{

	GCoptimization::SmoothCostFunctor* fn = new GCoptimizationGeneralGraph::smoothCostFn(m_gc,m_nCurNeig,m_nTradeOff,m_cSigma,m_dSigma);
	m_gc->setSmoothCostFunctor(fn);

}

//------------------------------------------------
void GCop::visulizationLabels()
{
	IndexType centFrame = (gcNode->node_vec)[0]->frame;
	Sample & curF = gcNode->m_smpSet[centFrame];


	IndexType nNode = gcNode->node_vec.size();
	// init prolabel

	IndexType k = 0;
	for (IndexType smpId = 0; smpId < nNode; smpId ++,k++)
	{
		m_optLabel.push_back(m_gc->whatLabel(k));
	}


// 	IndexType nIter = 3;
// 	while (nIter-->0)
// 	{
// 		smoothSampleLabel(curF,m_smpId,m_optLabel,m_optLabel);
// 	}
//// 
 	//diff_using_bfs(m_optLabel,m_smpId,centFrame);

	//IndexType finalLabels = orderLabels(m_optLabel);


#ifdef  SHOW_SAMPLE

		#ifdef OUTPUT_LABELS
			char outputLabelName[256]; 
			sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\RANSAC0602\\0602_horse_ransac%.2d.txt",centFrame);
			FILE *in_label = fopen(outputLabelName,"w");
			fprintf(in_label,"%d\n",finalLabels);
		#endif // OUTPUT_LABELS

			IndexType i = 0;
			IndexType ik = 0;
			for (Sample::vtx_iterator v_iter = curF.begin();v_iter != curF.end();v_iter++,i++)
			{
				if (m_isSelect[i])
				{
#ifdef EXPSWAP
					(*v_iter)->set_visble(true);
// 					if (m_isEdgePoint[i])
// 					{
// 						(*v_iter)->set_label(m_nLabels +1);
// 					}else
// 					{
                        (*v_iter)->set_label( m_optLabel[ik]);
//					}

#else
					(*v_iter)->set_visble(true);

       				if (m_inliers[i] == false)//可视化拟合平面--剔除噪声
       				{
					    (*v_iter)->set_label( m_optLabel[ik]);

 					}else
 					{
 						(*v_iter)->set_label(m_nLabels + 1);	
 					}
#endif // EXPSWAP
				
		#ifdef OUTPUT_LABELS
					fprintf(in_label,"%d %d %d\n",centFrame,m_optLabel[ik],i);
		#endif // OUTPUT_LABELS

					ik++;
				}else
				{
					(*v_iter)->set_visble(false);
				}
			}

		#ifdef OUTPUT_LABELS
			fclose(in_label);
		#endif // OUTPUT_LABELS

#else

	vector<IndexType> result_labels(curF.num_vertices(),0);
	propagateLabel2Orignal(curF,m_smpId,m_optLabel,result_labels);
	
	#ifdef OUTPUT_LABELS
		char outputLabelName[256];
		sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\fans\\final_labels%.2d.txt",centFrame);
		FILE *in_label = fopen(outputLabelName,"w");
		fprintf(in_label,"%d\n",finalLabels);
	#endif // OUTPUT_LABELS

	IndexType i = 0;
	for (Sample::vtx_iterator v_iter = curF.begin();v_iter != curF.end();v_iter++,i++)
	{
		(*v_iter)->set_label( result_labels[i]);

	#ifdef OUTPUT_LABELS
			//fprintf(in_label,"%d %d %d\n",centFrame,m_gc->whatLabel(k),i);
			fprintf(in_label,"%d %d %d\n",centFrame,result_labels[i],i);
	#endif // OUTPUT_LABELS

		}

	#ifdef OUTPUT_LABELS
		fclose(in_label);
	#endif // OUTPUT_LABELS

#endif //  SHOW_SAMPLE


}
//--------------------------------------------------
void GCop::findEdgePoint(unordered_map<IndexType, set<IndexType>> & edgePoints)
{
	//label_bucket[frame_label_to_key(frame,label)].insert(index);
	const IndexType k = 150;
	IndexType neighbours[k];
	ScalarType dist[k];
	//IndexType s_id = 0;

	Sample & curF = gcNode->m_smpSet[m_curFId];
	IndexType nSamples = gcNode->node_vec.size();
	int nCluster = m_nLabels;

// 	for (IndexType smpId = 0; smpId < nSamples; smpId ++)
// 	{
// 		IndexType pId = gcNode->node_vec[smpId]->index;
// 		curF.neighbours(pId,k,neighbours,dist);
// 		//curF.neighbours()
// 		IndexType label = gcNode->node_vec[smpId]->label;
// 
// 		for (IndexType neigId = 1; neigId < k; neigId ++)
// 		{
// 			IndexType fId= neighbours[neigId];
// 			GraphCutNode *fNode = gcNode->node_map[frame_index_to_key(m_curFId,fId)];
// 			if (fNode)
// 			{
// 				if (fNode->label != label)
// 				{
// 					if (label < fNode->label)
// 					{
// 						edgePoints[frame_label_to_key(label,fNode->label)].insert(pId);
// 					}else
// 					{
// 						edgePoints[frame_label_to_key(fNode->label,label)].insert(pId);
// 					}
// 
// 				    break;
// 				}
// 
// 			}
// 		}
// 	}


	IndexType nRand = 0.7 * nSamples;

	while ( nRand -- > 0)
	{
		IndexType smpId = rand()% nSamples;
		IndexType pId = gcNode->node_vec[smpId]->index;
		curF.neighbours(pId,k,neighbours,dist);
		IndexType label = gcNode->node_vec[smpId]->label;

		for (IndexType neigId = 1; neigId < k; neigId ++)
		{
			IndexType fId= neighbours[neigId];
			GraphCutNode *fNode = gcNode->node_map[frame_index_to_key(m_curFId,fId)];
			if (fNode)
			{
				if (fNode->label != label)
				{
					if (label < fNode->label)
					{
						edgePoints[frame_label_to_key(label,fNode->label)].insert(pId);
					}else
					{
						edgePoints[frame_label_to_key(fNode->label,label)].insert(pId);
					}

					break;
				}

			}
		}
	}

}
//--------------------------------------------------
void GCop::visualEdgePoints(unordered_map<IndexType, set<IndexType>> & edgePoints)
{
	Sample & curF = gcNode->m_smpSet[m_curFId];
	m_isEdgePoint.resize(curF.num_vertices(),false);

	for (IndexType i = 0; i < m_nLabels; i ++)
	{
		for(IndexType j = i; j < m_nLabels; j ++)
		{
			set<IndexType> edgePs;
			edgePs = edgePoints[frame_label_to_key(i,j)];
			if (!edgePoints.empty())
			{
				for ( auto it = edgePs.begin(); it != edgePs.end(); it ++)
				{
					m_isEdgePoint[(*it)] = true;
				}
			}
		}
	}

}
//--------------------------------------------------
void GCop::smoothSampleLabel(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_smooth)
{
	IndexType nCluster = m_nLabels;

	map<IndexType,IndexType> smpLabel;
	map<IndexType,IndexType>::iterator IsValidIter;
	for (int i = 0; i < label_smp.size(); i++)
	{
		smpLabel.insert(make_pair(sampleVtxId[i],label_smp[i]));
	}

	const IndexType k = 400;
	IndexType neighbours[k];
	ScalarType dist[k];

	IndexType result_label;
	IndexType s_id = 0;
	for(auto vec_iter =sampleVtxId.begin(); vec_iter != sampleVtxId.end(); vec_iter++,s_id ++)
	{
		IndexType smpId = *vec_iter;

		vector<IndexType> recordLabelTime(nCluster,0);
		result_label = -1;
		oriPC.neighbours(smpId, k, neighbours, dist);
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
				label_smooth[s_id] = i;
				result_label = recordLabelTime[i];
			}
		}
	}

// 	Logger<<"k-neig's k = "<<k<<endl;
// 	Logger<<"End  Sample Propagate!"<<endl;
}

//--------------------------------------------------

void GCop::propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_ori)
{
	IndexType nCluster = m_nLabels;

	map<IndexType,IndexType> smpLabel;
	map<IndexType,IndexType>::iterator IsValidIter;
	for (int i = 0; i < label_smp.size(); i++)
	{
		smpLabel.insert(make_pair(sampleVtxId[i],label_smp[i]));
	}

	const IndexType k = 250;
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
//--------------------------------------------------

void GCop::emAlgoritm(vector<GraphCutNode*>& oriData)
{
	IndexType nSamples = oriData.size();
	//int nCluster = 2;//box
	int nCluster = m_nLabels;

	vector<IndexType> labels;
	vector<IndexType> nLabels;
	nLabels.resize(nCluster,0);
	labels.resize(nSamples,0);

	getInitLabels(oriData,labels,nLabels);


	MatrixXX samples;
	productionSamples(oriData,samples);
	//m_sampleData = samples;

	//CvMat * mean_ = cvCreateMat(nCluster,3,CV_32FC1);
	//CvMat * pro_  = cvCreateMat(nSamples,nCluster,CV_32FC1);//需要拆分样本矩阵来求解各自的初始类的均值和方差
	//CvMat* labels_ = cvCreateMat(nSamples,1,CV_32FC1);	

	//CvMat * weig_ = cvCreateMat(nCluster,1,CV_32FC1);

	//CvMat * sample_ = cvCreateMat(nSamples,m_nDim,CV_32FC1);

	//CvMat *cov_1 = cvCreateMat(m_nDim,m_nDim,CV_32FC1);;
	//CvMat *cov_2 = cvCreateMat(m_nDim,m_nDim,CV_32FC1);;

	//CvMat* smpId_ = cvCreateMat(nSamples,1,CV_32FC1);	

	//for (IndexType i = 0; i < nSamples; i++)
	//{
	//	cvmSet(smpId_,i,0,i);
	//	cvmSet(labels_,i,0,0);
	//}

	//for (IndexType i = 0; i < nCluster; i++)
	//{
	//  cvmSet(weig_,i,0,1/nCluster);
	//}

	//const CvMat* cvv[2]={cov_1,cov_2};



// 	MatrixXX mean0;
// 	MatrixXX cov0;
// 	MatrixXX cov1;
// 	MatrixXX pro0;
// 	mean0.setZero(nCluster,m_nDim);
// 	cov0.setZero(nCluster,m_nDim*m_nDim);
// 	pro0.setZero(nSamples,nCluster);
// 	calculateMean(samples,mean0,cov0,cov1,pro0,labels,nLabels);
// 
// 	for (IndexType i = 0 ; i < 50; i++)
// 	{
// 		Logger<<samples.row(i)<<endl;
// 		Logger<<labels(i,0)<<endl;
// 	}
// 
// 
// 	Logger<<"mean"<<endl;
// 	Logger<<mean0<<endl;
// 
// 	Logger<<"covariance = "<<endl;
// 	Logger<<cov0<<endl;
// 	Logger<<endl;
// 	Logger<<cov1<<endl;




	// eigen to cv::Mat-----------------------

// 	cv::Mat cvSamples(nSamples,m_nDim,CV_64FC1);
// 	cv::eigen2cv(samples,cvSamples);
// 
// 	cv::Mat cvMean0;
// 	cv::eigen2cv(mean0,cvMean0);
// 
// 	cv::Mat cvPro0(nSamples,nCluster,CV_64FC1);
// 	cv::eigen2cv(pro0,cvPro0);
// 
// 	cv::Mat cvLabels;
// 	cv::eigen2cv(labels,cvLabels);
// 
// 
// 	cv::Mat cvCov0;
// 	cv::eigen2cv(cov0,cvCov0);
// 
// 	cv::Mat cvCov1;
// 	cv::eigen2cv(cov1,cvCov1);

	////cv::Mat to cvMat-------------------------
// 	CvMat temp_1 = cvMean0;
// 	cvCopy(&temp_1,mean_);
// 
// 
// 	 CvMat temp_2 = cvPro0;
// 	cvCopy(&temp_2,pro_);
// 
// 
// 	 CvMat temp_3 = cvLabels;
// 	cvCopy(&temp_3,labels_);
// 
// 
// 	 CvMat temp_4 = cvSamples;
// 	cvCopy(&temp_4,sample_);
// 
// 
// 
// 	CvMat temp_5 = cvCov0;
// 	cvCopy(&temp_5,cov_1);
// 
// 	 CvMat temp_6 = cvCov1;
// 	cvCopy(&temp_6,cov_2);


	//-----------------------------------

	


//   	cv::EM em_model; 
//   	cv::Mat like/*(nSamples,1,CV_64FC1)*/;
//   	cv::Mat outLike/*(nSamples,1,CV_64FC1)*/;
//   	em_model.trainM(cvSamples,cvPro0,like,cvLabels,outLike);

////QT!


	//em_model.trainM(cvSamples)
	//CvEMParams params;
	//CvEM em_model;
// 	params.term_crit.type = CV_TERMCRIT_EPS| CV_TERMCRIT_ITER;
// 	params.nclusters = nCluster;
// 	params.start_step = cv::EM::START_M_STEP;
// 	params.cov_mat_type = cv::EM::COV_MAT_DIAGONAL;
// 	params.means = mean_;
// 	params.probs = pro_;
// 	params.covs = cvv;
// 	params.weights = weig_;
// 	params.term_crit.max_iter = 10;
// 	params.term_crit.epsilon  = 0.1;


// 	params_cv.term_crit.type = CV_TERMCRIT_EPS| CV_TERMCRIT_ITER;
// 	params_cv.nclusters = nCluster;
// 	params_cv.start_step = CvEM::START_M_STEP;
// 	params_cv.cov_mat_type = CvEM::COV_MAT_DIAGONAL;
// 	
// 	params_cv.means = mean_;
// 	params_cv.probs = pro_;
// 	params_cv.covs = cvv;
// 	params_cv.weights = weig_;
// 	params_cv.term_crit.max_iter = 10;
// 	params.term_crit.epsilon  = 0.1;

// 	MatrixXX tSmp;
// 
// 	cv::cv2eigen(sample_,tSmp);
// 
// 	for (int i = 0; i < 20; i++)
// 	{
// 		Logger<<tSmp.row(i)<<endl;
// 	}

	//em_model.train(sample_,0,params,labels_);//error!!!  cv::Exception at memory location 0x0000000020ECEDA0.


// 	cv::cv2eigen(cvLabels,labels);
// 
// 	for (IndexType i = 0; i < 50; i++)
// 	{
// 		Logger<<labels(i,0)<<endl;
// 	}
}


//---------------------------------
void GCop::productionSamples(vector<GraphCutNode*>& oriData,MatrixXX & samples)
{
	IndexType nNode = oriData.size();
	samples.setZero(nNode,m_nDim);//zero() and setzero() are different

	MatrixXX trans;

	IndexType nodeId = 0;
	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		GraphCutNode* temp = (*iter);
	    trans.Zero(1,6);
		calculateTrans(temp,trans);//(0-2)平移-旋转

#ifdef ONLY_THREE
        samples.block(nodeId,0,1,3) = trans.block(0,0,1,3);
#else
		samples.row(nodeId) = trans.row(0);
#endif // ONLY_THREE

	}
}

void GCop::calculateTrans(GraphCutNode* node,MatrixXX & trans)
{
	IndexType fId = node->frame;

	Sample & curF = gcNode->m_smpSet[fId];

//  	const IndexType k = 150;//设置一个参数
//  	IndexType neighbours[k];
//  	ScalarType dist[k];
// 	curF.neighbours(node->index,k,neighbours,dist);

 	IndexType *neighbours = new IndexType [m_nSwap];
 	ScalarType * dist = new ScalarType [m_nSwap];
	curF.neighbours(node->index,m_nSwap,neighbours,dist);

	vector<IndexType> recordNeigId;//查找J帧的邻域顶点索引

 	vector<IndexType> mapId;
	for (IndexType neig_id = 0; neig_id < m_nSwap; neig_id ++)
	{
		IndexType pId = neighbours[neig_id];
		GraphCutNode *fNode = gcNode->node_map[frame_index_to_key(fId,pId)];
		if (fNode)
		{
			recordNeigId.push_back(pId);

			map<IndexType,IndexType>::iterator start_isInValidIter;

			start_isInValidIter = fNode->cor_frame_index.find(fId + 1);

			if (start_isInValidIter != fNode->cor_frame_index.end())
			{
				mapId.push_back(start_isInValidIter->second);
			}

		}
	}
	
	IndexType neigSize = mapId.size();
	Matrix3X sCoor,tCoor;

	if ( mapId.size() != recordNeigId.size())
	{
		Logger<<"error ! no correspondence happend.\n";
	}

	//Logger<<mapId.size()<<endl;

	sCoor.setZero(3,neigSize);
	tCoor.setZero(3,neigSize);

	Sample & nextF = gcNode->m_smpSet[fId + 1];//获取坐标帧的坐标

	for (IndexType i = 0; i < neigSize; i++)
	{
		sCoor.col(i) = curF.vertices_matrix().col(recordNeigId[i]);
		tCoor.col(i) = nextF.vertices_matrix().col(mapId[i]);
	}


	Matrix33 rotMat;
	MatrixXX tranVec;
	rotMat.setZero();
	tranVec.setZero(1,3);

	point2point(sCoor,tCoor,rotMat,tranVec);

	Vec3 eulerAngle;
	eulerAngle.setZero();
 	rotMat2EAngle(rotMat,eulerAngle);

	trans.setZero(1,6);
	trans.block(0,0,1,3) = (tranVec.transpose()).block(0,0,1,3);//平移
	trans.block(0,3,1,3) = (eulerAngle.transpose()).block(0,0,1,3);//旋转

// 	delete [] neighbours;//动态申请时候调用
// 	delete [] dist;
}

void GCop::point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat,MatrixXX & transVec)
{
	assert(srCloud.cols() > 2 && tgCloud.cols() > 2);

	Eigen::Vector3f X_mean, Y_mean;

	for(int i=0; i<3; ++i) //计算两点云的均值
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
		transformation.linear().noalias() = svd.matrixV()*svd.matrixU().transpose();//计算旋转矩阵
	}

	transVec = Y_mean - transformation.linear()*X_mean;
	rotMat = transformation.linear() ;

	srCloud.colwise() += X_mean;
	tgCloud.colwise() += Y_mean;

}
//------------------------------------------
void GCop::point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat,MatrixXX & transVec,Vec3&sMean,Vec3&tMean)
{
	Eigen::Vector3f X_mean, Y_mean;

	for(int i=0; i<3; ++i) //计算两点云的均值
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
		transformation.linear().noalias() = svd.matrixV()*svd.matrixU().transpose();//计算旋转矩阵
	}

	transVec = Y_mean - transformation.linear()*X_mean;
	rotMat = transformation.linear() ;

	sMean = X_mean;
	tMean = Y_mean;

	srCloud.colwise() += X_mean;
	tgCloud.colwise() += Y_mean;
}
//------------------------------------------
void GCop::calculateMean(MatrixXX& sample_,MatrixXX & mean_,MatrixXX& cov_0,MatrixXX& cov_1,MatrixXX & pro,MatrixXX& labels,vector<IndexType>&nLabels)
{
	//申请nLabels.size()个数目的矩阵
	MatrixXX fMat;
	MatrixXX sMat;
	IndexType f_size = nLabels[0];
	IndexType s_size = nLabels[1];

	fMat.setZero(f_size,m_nDim);
	sMat.setZero(s_size,m_nDim);


//  	char outputLabelName[256]; 
//  	sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\rot_girl_walk_label_1.txt");
//  	FILE *in_label = fopen(outputLabelName,"w");

	IndexType sId = 0;
	IndexType fId = 0;
	for (IndexType rowId = 0; rowId < sample_.rows(); rowId++)
	{
		if (labels(rowId,0) == 0)
		{
			fMat.row(fId) = sample_.row(rowId);
			//fprintf(in_label,"%f %f %f\n",sample_(rowId,0),sample_(rowId,1),sample_(rowId,2));
			fId++;

		}else if(labels(rowId,0) == 1)
		{
			sMat.row(sId) = sample_.row(rowId);

			sId++;
		}
		else
		{

		}
	}


  //fclose(in_label);


	for (IndexType colId = 0; colId < m_nDim;colId ++)
	{
		mean_(0,colId) = fMat.col(colId).sum()/fMat.rows();
		mean_(1,colId) = sMat.col(colId).sum()/sMat.rows();
	}




//  	Logger<<"mean = "<<endl;
//  	Logger<<mean_<<endl;


	//计算协方差矩阵和概率

	//MatrixXX cov0;
	//MatrixXX cov1;
	//cov0.setZero(m_nDim,m_nDim);
	//cov1.setZero(m_nDim,m_nDim);

	//fMat.rowwise() -= mean_.row(0);
	//sMat.rowwise() -= mean_.row(1);

// 	Logger<<"first row of fMat"<<fMat.row(0)<<endl;
// 	Logger<<"first row of sMat"<<sMat.row(0)<<endl;

	//cov0 = fMat.transpose() * fMat;
	//cov1 = sMat.transpose() * sMat;

	//cov_0 = cov0/fMat.rows();
	//cov_1 = cov1/sMat.rows();



	////计算初始的概率值

	//MatrixXX m_1 = mean_.row(0);
	//MatrixXX m_2 = mean_.row(1);
	//for (IndexType smpId = 0; smpId < sample_.rows(); smpId ++)
	//{
	//	MatrixXX s_id = sample_.row(smpId);
	//	pro(smpId,0) = calculateProb(m_nDim,m_1,cov_0,s_id);
	//	pro(smpId,1) = calculateProb(m_nDim,m_2,cov_1,s_id);
	//}

}
//-------------------------
// void GCop::calculateMean(MatrixXX& sample_,MatrixXX & mean_,CvMat* cov_1,CvMat*cov_2,MatrixXX& pro,MatrixXX& labels,vector<IndexType>& nLabels)
// {
// 	//申请nLabels.size()个数目的矩阵
// 	MatrixXX fMat;
// 	MatrixXX sMat;
// 	IndexType f_size = nLabels[0];
// 	IndexType s_size = nLabels[1];
// 
// 	fMat.setZero(f_size,m_nDim);
// 	sMat.setZero(s_size,m_nDim);
// 
// 	IndexType sId = 0;
// 	IndexType fId = 0;
// 	for (IndexType rowId = 0; rowId < sample_.rows(); rowId++)
// 	{
// 		if (labels(rowId,0) == 0)
// 		{
// 			fMat.row(fId) = sample_.row(rowId);
// 			fId++;
// 		}else
// 		{
// 			sMat.row(sId) = sample_.row(rowId);
// 			sId++;
// 		}
// 	}
// 
// 
// 	for (IndexType colId = 0; colId < m_nDim;colId ++)
// 	{
// 		mean_(0,colId) = fMat.col(colId).sum()/fMat.rows();
// 		mean_(1,colId) = sMat.col(colId).sum()/sMat.rows();
// 	}
// 
// 	// 	Logger<<"mean = "<<endl;
// 	// 	Logger<<mean_<<endl;
// 
// 	MatrixXX cov0;
// 	MatrixXX cov1;
// 	cov0.setZero(m_nDim,m_nDim);
// 	cov1.setZero(m_nDim,m_nDim);
// 
// 	fMat.rowwise() -= mean_.row(0);
// 	sMat.rowwise() -= mean_.row(1);
// 
// 	// 	Logger<<"first row of fMat"<<fMat.row(0)<<endl;
// 	// 	Logger<<"first row of sMat"<<sMat.row(0)<<endl;
// 
// 	cov0 = fMat.transpose() * fMat;
// 	cov1 = sMat.transpose() * sMat;
// 
// // 	cv::Mat temp;
// // 	cv::eigen2cv(cov0,temp);
// // 	
// // 	CvMat midd = temp;
// // 	cvCopy(&midd,&cov_1);
// // 
// // 	eigen2cv(cov1,temp);
// // 	midd = temp;
// // 	cvCopy(&midd,&cov_2);
// 
// 	// 	Logger<<"cov1 = "<<endl;
// 	// 	Logger<<cov0<<endl;
// 	// 	Logger<<"cov2 = "<<endl;
// 	// 	Logger<<cov1<<endl;
// 
// 
// 	//计算初始的概率值
// 
// 	MatrixXX m_1 = mean_.row(0);
// 	MatrixXX m_2 = mean_.row(1);
// 	for (IndexType smpId = 0; smpId < sample_.rows(); smpId ++)
// 	{
// 		MatrixXX s_id = sample_.row(smpId);
// 		pro(smpId,0) = calculateProb(m_nDim,m_1,cov0,s_id);
// 		pro(smpId,1) = calculateProb(m_nDim,m_2,cov1,s_id);
// 	}
// }
//-------------------------------------------
void GCop::calculateCov(MatrixXX& sample_,MatrixXX& cov_)
{

}

//--------------------------------------------
void GCop::calculatePro(MatrixXX& sample_,MatrixXX& mean_,MatrixXX& cov_,MatrixXX pro_)
{

}
//-------------------------------------------
void GCop::getInitLabels(vector<GraphCutNode*>& oriData_,vector<IndexType> & labels_,vector<IndexType>& nLabels)
{
	IndexType id = 0;
	for (auto iter = oriData_.begin(); iter!= oriData_.end(); iter++,id ++)
	{
		labels_[id] = (*iter)->label;
		nLabels[labels_[id]] ++;
	}
}


//---------------------------------------------
ScalarType GCop::calculateProb(IndexType nDim,MatrixXX& mean_,MatrixXX& cov,MatrixXX& sValue)
{
// 	ScalarType coffPi=1.0;
// 	ScalarType coffCov = 1.0;
// 	for (IndexType sId = 0; sId < nDim; sId ++)
// 	{
// 		coffPi *= (2*PI);
// 	}
// 
// 	coffPi = sqrt(coffPi);

   //Logger<<cov<<endl;


   MatrixXX covInve;
   MatrixXX IdMat;
   IdMat.setIdentity(nDim,nDim);
   covInve.setZero(nDim,nDim);

   //coffCov = cov.determinant();
   //Logger<<"determinant = "<<coffCov<<endl;
  // coffCov = sqrt(coffCov);

   Eigen::HouseholderQR<MatrixXX> qr(cov);
   covInve = qr.solve(IdMat);



//    Logger<<covInve<<endl;
// 
//    Logger<<"res = "<<endl;
//    Logger<<cov * covInve<<endl;
// 
//    Logger<<"coff = "<<coffCov<<endl;

   MatrixXX temp = (sValue - mean_) * covInve * (sValue - mean_).transpose();
   ScalarType mo = temp(0,0);
   ScalarType pro = exp(-mo *(0.5));

//    Logger<<"prob = "<<pro<<endl;
    //Logger<<"last = "<<pro/(coffCov * coffPi)<<endl;

   return pro;

}

//----------------------------------------------------
void GCop::rotMat2EAngle(Matrix33& rotMat,Vec3& eulerAg)
{
	ScalarType x_A = 0.0;
	ScalarType y_A = 0.0;
	ScalarType z_A = 0.0;

	y_A = -asin(rotMat(2,0));

	assert(cos(y_A) > 1e-6 || cos(y_A)< -1e-6);

	x_A = atan2f(rotMat(2,1)/cos(y_A),rotMat(2,2)/cos(y_A));
	z_A = atan2f(rotMat(1,0)/cos(y_A),rotMat(0,0)/cos(y_A));

	eulerAg(0,0) = x_A;
	eulerAg(1,0) = y_A;
	eulerAg(2,0) = z_A;

}

void GCop::diff_using_bfs( std:: vector<IndexType>& labels,std::vector<IndexType>& centerVtxId,IndexType centerFrame )
{
	SampleSet& set = SampleSet::get_instance();
	IndexType max_label= *max_element(labels.begin(),labels.end());
	vector< std::set<IndexType> > label_bucket(max_label+1);

	//IndexType centerFrame = 10;
	for ( int i=0; i<labels.size(); i++ )
	{
		label_bucket[labels[i]].insert( i );
	}
	IndexType new_label = max_label ;
	Logger<<"max label before post:"<<new_label + 1<<endl;
	for ( IndexType l=0; l<label_bucket.size(); l++ )
	{
		std::set<IndexType>& idx_set = label_bucket[l];
		if (idx_set.size()==0)
		{
			continue;
		}
		IndexType idx_set_size = idx_set.size();
		Matrix3X vtx_data;
		vtx_data.setZero( 3, idx_set_size );
		size_t i=0;
		for (std::set<IndexType>::iterator iter = idx_set.begin();
			iter != idx_set.end();
			iter++,i++)
		{
			IndexType a = *iter;
			vtx_data(0,i)  = set[centerFrame][centerVtxId[*iter]].x();
			vtx_data(1,i)  = set[centerFrame][centerVtxId[*iter]].y();
			vtx_data(2,i)  = set[centerFrame][centerVtxId[*iter]].z();
		}


#ifdef USE_RADIUS_NEAREST
		ScalarType rad = set[centerFrame].box_diag()/m_nDiffu;
		BFSClassifier<ScalarType> classifier(vtx_data,rad);
#else
		ScalarType rad = set[centerFrame].box_diag()/m_nDiffu;
		BFSClassifier<ScalarType> classifier(vtx_data,rad,10);
#endif

		classifier.run();
		int *sep_label = classifier.get_class_label();
		i=0;
		for (std::set<IndexType>::iterator iter = idx_set.begin();
			iter != idx_set.end();
			iter++,i++)
		{
			if(sep_label[i]==0)continue;
			labels[*iter] = new_label + sep_label[i];
		}
		new_label += classifier.get_num_of_class()-1;


	}
	
	m_nLabels = new_label + 1;

	Logger<<"max label after post:"<<m_nLabels<<endl;
}

//-----------------------------------
IndexType GCop::orderLabels(vector<IndexType>& labels)
{
	if (labels.size() == 0)
	{
		Logger<<" label's vector is empty!.\n";
		return 0;
	}

	map<IndexType,IndexType> recordLabel;
	map<IndexType,IndexType>::iterator fIt;


	IndexType temp = 0;
	auto iter = labels.begin();

	recordLabel[(*iter)] = temp;

	for (;iter != labels.end(); iter ++)
	{
		fIt = recordLabel.find(*iter);
		if (fIt != recordLabel.end())
		{
			(*iter) = fIt->second;
		}else
		{
			temp++;
			recordLabel[(*iter)] = temp;
			(*iter) = temp;
		}
	}

	recordLabel.clear();

	return temp + 1;
}


void  GCop::ransacAlgorithm(vector<GraphCutNode*>& oriData)
{
	IndexType nSamples = oriData.size();
	int nCluster = m_nLabels;

	vector<IndexType> labels;
	labels.resize(nSamples,0);

	vector<IndexType> nLabels;
	nLabels.resize(nCluster,0);

	getInitLabels(oriData,labels,nLabels);//记录每个点的label号码,以及各自号码有多少个点

	set<IndexType> inliers;//记录内部点的索引[对应后的索引并非原始点云中的索引]
	inliers.empty();

	map<IndexType,IndexType> id2Id;	//记录对应

	IndexType slClass = m_nSwap;//设置选择的类

	IndexType fId = gcNode->node_vec[0]->frame;
	Sample & curF = gcNode->m_smpSet[fId];
    m_inliers.resize(curF.num_vertices(),false);//为了可视化inliers和outliers

	// start iterator
	ScalarType noiserR = m_nTradeOff;
	IndexType iterNum = m_nExpansion;//迭代次数



	
#ifdef FIT_PLAN           //用RANSAC拟合一个平面--用来做测试;
	Matrix3X OneLabelCoor;
	OneLabelCoor.setZero(3,nLabels[slClass]);
#else
	MatrixXX samples;//(n×3)
	productionSamples(oriData,samples);
    MatrixXX trans;
	trans.setZero(nLabels[slClass],m_nDim);
#endif // FIT_PLAN

	//输出运动参数统计量
	//char outputLabelName[256]; 
	//sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\motion_para.txt");
	//FILE *in_label = fopen(outputLabelName,"w");

	
	IndexType nodeId = 0;
	IndexType inId = 0;
	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		if ((*iter)->label == slClass)
		{
			IndexType pId = (*iter)->index;
#ifdef FIT_PLAN
			OneLabelCoor.col(inId) = curF.vertices_matrix().col(pId);
#else
			trans.row(inId) = samples.row(nodeId);
		    //fprintf(in_label,"%f %f %f %f %f %f\n",trans(inId,0),trans(inId,1),trans(inId,2),trans(inId,3),trans(inId,4),trans(inId,5));
#endif // FIT_PLAN

			id2Id[inId] = pId;
			inId ++;
		}
   }

  //fclose(in_label);


#ifdef FIT_PLAN
   PointType  center;
   NormalType planNorm;//模型参数[点法式]
   initPlan(OneLabelCoor,center,planNorm,inliers);//fits  a plan

   realRansac(OneLabelCoor,center,planNorm,inliers,noiserR,iterNum);

   setInliers(id2Id,inliers);

   //drawPlan(center,planNorm);//可视化拟合的平面

#else
  MatrixXX miu;
  MatrixXX sigma;//model- gauss distribution

  miu.setZero(1,m_nDim);
  sigma.setIdentity(m_nDim,m_nDim);
  initDistribution(trans,miu,sigma,inliers);
  gaussRansac(trans,miu,sigma,inliers,noiserR,iterNum);
  setInliers(id2Id,inliers);

#endif

}

//---------------------------------------
void GCop::ransacAllAlgorithm(vector<GraphCutNode*>& oriData)
{
	IndexType nSamples = oriData.size();
	int nCluster = m_nLabels;

	vector<IndexType> labels;
	labels.resize(nSamples,0);

	vector<IndexType> nLabels;
	nLabels.resize(nCluster,0);

	map<IndexType,IndexType> id2Id;	//记录对应

	set<IndexType> inliers;//记录内部点的索引[对应后的索引并非原始点云中的索引]
	inliers.empty();

	getInitLabels(oriData,labels,nLabels);//记录每个点的label号码,以及各自号码有多少个点

	MatrixXX samples;//(n×3)
	productionSamples(oriData,samples);//产生所有点的运动样本数据

	ScalarType noiserR = m_nTradeOff;
	IndexType iterNum = m_nExpansion;//迭代次数

	vector< set<IndexType> >labelIdSet;//记录每个label对应的点graph ID索引
	labelIdSet.resize(nCluster);

	int pId = 0;
	for (auto it = labels.begin(); it != labels.end(); ++it,pId++)
	{
		labelIdSet[(*it)].insert(pId);
	}

	MatrixXX *totMean  = new MatrixXX [nCluster];
	MatrixXX *totSigma = new MatrixXX [nCluster];


	for ( int i = 0; i < nCluster; i ++)
	{
		IndexType pSize = labelIdSet[i].size();
		MatrixXX trans;
		trans.setZero(pSize,m_nDim);

		int j = 0;
		for (auto it = labelIdSet[i].begin(); it != labelIdSet[i].end(); it ++,j++)
		{
			trans.row(j) = samples.row(*it);
		}

		MatrixXX miu;
		MatrixXX sigma;//model- gauss distribution


		miu.setZero(1,m_nDim);
		sigma.setIdentity(m_nDim,m_nDim);

		initDistribution(trans,miu,sigma,inliers);
		gaussRansac(trans,miu,sigma,inliers,noiserR,iterNum);

		totMean[i] = miu;
        totSigma[i] = sigma;


		Logger<<"tanslation = "<<endl;
		Logger<<miu.transpose()<<endl;
		Logger<<"rotmate = "<<endl;
		Logger<<sigma<<endl;

	}


	// 设置数据项
// 	GCoptimization::DataCostFunctor* fn = new GCoptimizationGeneralGraph::dataCostFn(m_gc,m_nCurNeig,totMean,totSigma,samples);
// 	m_gc->setDataCostFunctor(fn);
	///

    //---------------------------------------------------
	IndexType fId = gcNode->node_vec[0]->frame;
	Sample & curF = gcNode->m_smpSet[fId];
	m_inliers.resize(curF.num_vertices(),false);//为了可视化inliers和outliers
	inliers.clear();
	//输出点到所有模型的距离
 //	char outputLabelName[256]; 
 //	sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\all_prob_dist.txt");
 //	FILE *in_label = fopen(outputLabelName,"w");

	////数据到自己类率大则标记为inliers
	ScalarType *allDist = new ScalarType[nCluster]; 
	for ( int i = 0; i < nSamples; i ++ )
	{
		MatrixXX temp = samples.row(i);
		//fprintf(in_label,"%d ",labels[i]);

		for (int j = 0; j < nCluster; j ++)
		{
			ScalarType dist = calculateProb(m_nDim,totMean[j],totSigma[j],temp);
			allDist[j] = dist;
			//fprintf(in_label,"%f ",dist);
		}

		IndexType max_idx = std::max_element(allDist,allDist + nCluster) - allDist;

		if (labels[i] == max_idx)
		{
			inliers.insert(i);
		}
		
		//fprintf(in_label," \n");
	}

	delete [] allDist;
	//fclose(in_label);


	//可视化每个点是否为噪声,每个类点若保持自己类颜色则为噪声,变为第m_nLabels + 1 则为inliers
 	IndexType nodeId = 0;
 	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
 	{
 		IndexType pId = (*iter)->index;
 		id2Id[nodeId] = pId;
 	}
 
 	setInliers(id2Id,inliers);

}
//---------------------------------------fit a rotate matrix and translation vector
void GCop::ransacRotTan(vector<GraphCutNode*>& oriData)
{
	IndexType nSamples = oriData.size();
	int nCluster = m_nLabels;

	vector<IndexType> labels;
	labels.resize(nSamples,0);

	vector<IndexType> nLabels;
	nLabels.resize(nCluster,0);

	map<IndexType,IndexType> id2Id;	//记录对应

	set<IndexType> inliers;//记录内部点的索引[对应后的索引并非原始点云中的索引]
	inliers.empty();

	getInitLabels(oriData,labels,nLabels);//记录每个点的label号码,以及各自号码有多少个点

	IndexType nodeId = 0;
	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		IndexType pId = (*iter)->index;
		id2Id[nodeId] = pId;
	}

	ScalarType noiserR = /*0.9;//*/m_nTradeOff;
	IndexType iterNum = /*2;*/m_nExpansion;//迭代次数

	vector< set<IndexType> >labelIdSet;//记录每个label对应的点graph ID索引
	labelIdSet.resize(nCluster);

	int pId = 0;
	for (auto it = labels.begin(); it != labels.end(); ++it,pId++)
	{
		labelIdSet[(*it)].insert(pId);
	}

	IndexType fId = gcNode->node_vec[0]->frame;
	Sample & curF = gcNode->m_smpSet[fId];
	Sample & nextF = gcNode->m_smpSet[fId + 1];

	gcNode->node_vec[0]->cor_frame_index.size();

	m_inliers.resize(curF.num_vertices(),false);//为了可视化inliers和outliers

	MatrixXX *totTrans  = new MatrixXX [nCluster];
	MatrixXX *totRotmate = new MatrixXX [nCluster];
	Vec3 * srMean = new Vec3[nCluster];
	Vec3 * tgMean = new Vec3[nCluster];

	for ( int i = 0; i < nCluster; i ++)
	{
		IndexType pSize = labelIdSet[i].size();
		Matrix3X sCoor;
		Matrix3X tCoor;
		sCoor.setZero(3,pSize);
		tCoor.setZero(3,pSize);

		int j = 0;
		for (auto it = labelIdSet[i].begin(); it != labelIdSet[i].end(); it ++,j++)
		{
			IndexType pd = id2Id[(*it)];
			sCoor.col(j) = curF.vertices_matrix().col(pd);

			IndexType nextPd =  gcNode->node_vec[(*it)]->cor_frame_index[fId + 1];
			PointType nextPoint =  nextF.vertices_matrix().col(nextPd);
			tCoor.col(j) = nextPoint;
		}

		MatrixXX trans;
		Matrix33 rotmate;//model- gauss distribution


		trans.setZero(1,3);
		rotmate.setIdentity(3,3);

		
		initRotTrans(sCoor,trans,rotmate,inliers,tCoor);
		rotRansac(sCoor,trans,rotmate,inliers,noiserR,iterNum,labelIdSet[i],tCoor);


// 		Logger<<"label = "<<i<<endl;
// 		Logger<<"tanslation = "<<endl;
// 		Logger<<trans.transpose()<<endl;
// 		Logger<<"rotmate = "<<endl;
// 		Logger<<rotmate<<endl;
// 		Logger<<"label sr_mean = "<<endl;
// 		Logger<<sMean.transpose()<<endl;
// 		Logger<<"label tg_mean = "<<endl;
// 		Logger<<tMean.transpose()<<endl;


		totTrans[i] = trans;
		totRotmate[i] = rotmate;

	}

	// 设置数据项
// 	GCoptimization::DataCostFunctor* fn = new GCoptimizationGeneralGraph::dataCostFn(m_gc,totTrans,totRotmate);
// 	m_gc->setDataCostFunctor(fn);


	//输出点到所有模型的距离
// 	char outputLabelName[256]; 
// 	sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\all_rot_tran_dist.txt");
// 	FILE *in_label = fopen(outputLabelName,"w");

	////数据到自己类率大则标记为inliers
	ScalarType *allDist = new ScalarType[nCluster]; 
	inliers.clear();

	for ( int i = 0; i < nSamples; i ++ )
	{
		IndexType pId = id2Id[i];
		PointType tPoint = curF.vertices_matrix().col(pId);
		IndexType corPid = gcNode->node_vec[i]->cor_frame_index[fId + 1];

		PointType corPoint = nextF.vertices_matrix().col(corPid);
		//fprintf(in_label,"%d ",labels[i]);

		for (int j = 0; j < nCluster; j ++)
		{
			PointType transPoint = totRotmate[j] * tPoint + totTrans[j];
			ScalarType dist = (corPoint - transPoint).norm();
			allDist[j] = dist;

			//fprintf(in_label,"%f ",dist);
		}

		IndexType max_idx = std::min_element(allDist,allDist + nCluster) - allDist;

		if (labels[i] == max_idx)
		{
			inliers.insert(i);
		}

		//fprintf(in_label," \n");
	}

	//fclose(in_label);

	delete [] allDist;

	setInliers(id2Id,inliers);

}
//--------------------------------------
void GCop::ransacMultiRotTan(vector<GraphCutNode*>& oriData)
{
	IndexType nSamples = oriData.size();
	int nCluster = m_nLabels;

	vector<IndexType> labels;
	labels.resize(nSamples,0);

	vector<IndexType> nLabels;
	nLabels.resize(nCluster,0);

	map<IndexType,IndexType> id2Id;	//记录对应

	set<IndexType> inliers;//记录内部点的索引[对应后的索引并非原始点云中的索引]
	inliers.clear();

	getInitLabels(oriData,labels,nLabels);//记录每个点的label号码,以及各自号码有多少个点

	IndexType nodeId = 0;
	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		IndexType pId = (*iter)->index;
		id2Id[nodeId] = pId;
	}

	ScalarType noiserR = 0.8;//m_nTradeOff;
	IndexType iterNum = 2;//m_nExpansion;//迭代次数

	vector< set<IndexType> >labelIdSet;//记录每个label对应的点graph ID索引
	labelIdSet.resize(nCluster);

	int pId = 0;
	for (auto it = labels.begin(); it != labels.end(); ++it,pId++)
	{
		labelIdSet[(*it)].insert(pId);
	}

	IndexType fId = gcNode->node_vec[0]->frame;
	Sample & curF = gcNode->m_smpSet[fId];

	m_curFId = fId;

	IndexType trajLen = gcNode->node_vec[0]->cor_frame_index.size();
	//IndexType trajLen = 2;
	m_inliers.resize(curF.num_vertices(),false);//为了可视化inliers和outliers

	MatrixXX ** totTrans;
	MatrixXX ** totRotmate;

	totTrans = new MatrixXX *[trajLen];
	totRotmate = new MatrixXX *[trajLen];

	for (IndexType i = 0; i < trajLen; i++)
	{
		totTrans[i] = new MatrixXX [nCluster];
		totRotmate[i] = new MatrixXX [nCluster];
	}

	IndexType frameId = 0;
	for (IndexType trajIt = -trajLen/2; trajIt <= trajLen/2; trajIt ++)
	{
		if (0 == trajIt) continue;

		IndexType nextFrameId = fId + trajIt;//绝对帧号码
		Sample & nextF = gcNode->m_smpSet[nextFrameId];

		for ( int i = 0; i < nCluster; i ++)
		{
			IndexType pSize = labelIdSet[i].size();
			Matrix3X sCoor;
			Matrix3X tCoor;
			sCoor.setZero(3,pSize);
			tCoor.setZero(3,pSize);

			int j = 0;
			for (auto it = labelIdSet[i].begin(); it != labelIdSet[i].end(); it ++,j++)
			{
				IndexType pd = id2Id[(*it)];
				sCoor.col(j) = curF.vertices_matrix().col(pd);//可以优化--只需要去除一次就可以

				IndexType nextPd =  gcNode->node_vec[(*it)]->cor_frame_index[nextFrameId];
				PointType nextPoint =  nextF.vertices_matrix().col(nextPd);
				tCoor.col(j) = nextPoint;
			}

			MatrixXX trans;
			Matrix33 rotmate;//model- gauss distribution

			trans.setZero(1,3);
			rotmate.setIdentity(3,3);

			inliers.clear();
			initRotTrans(sCoor,trans,rotmate,inliers,tCoor);
			rotRansac(sCoor,trans,rotmate,inliers,noiserR,iterNum,labelIdSet[i],tCoor);


			totTrans[frameId][i] = trans;
			totRotmate[frameId][i] = rotmate;

		} //end  for label

		frameId ++;//相对帧号码

	}//end for traj idex


	//输出点到所有模型的距离
// 	char outputLabelName[256]; 
// 	sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\SigleSmoo526dist_error_box.txt");
// 	FILE *in_label = fopen(outputLabelName,"w");

	
	MatrixXX allDist;//每个点误差
	allDist.setZero(trajLen,nCluster);

	MatrixXX totDist;//记录所有点的误差
	totDist.setZero(nSamples,nCluster);

	inliers.clear();
	IndexType startF = fId - trajLen/2;
	for ( int i = 0; i < nSamples; i ++ )
	{
		IndexType pId = id2Id[i];
		PointType tPoint = curF.vertices_matrix().col(pId);

		//fprintf(in_label,"%d ",labels[i]);

		IndexType itframe = 0;
		for (int k = 0; k <= trajLen; k ++)
		{
			IndexType nextframe = startF + k;
			if (nextframe == fId) continue;
		    IndexType corPid = gcNode->node_vec[i]->cor_frame_index[nextframe];
			Sample & nextF = gcNode->m_smpSet[nextframe];

		    PointType corPoint = nextF.vertices_matrix().col(corPid);

			for (int j = 0; j < nCluster; j ++)
			{
				PointType transPoint = totRotmate[itframe][j] * tPoint + totTrans[itframe][j];
				ScalarType dist = (corPoint - transPoint).norm();
				allDist(itframe,j) = dist;
				//fprintf(in_label,"%f ",dist);
			}
			itframe ++;
		}
		

        ScalarType * errMean = new ScalarType[nCluster]; //统计每个点的误差
		for(int cId = 0; cId<nCluster; ++cId) 
		{
			errMean[cId] = allDist.col(cId).sum();
			totDist(i,cId) = errMean[cId];
			//m_gc->setDataCost(i,cId,totDist(i,cId));
			//fprintf(in_label,"%f ",errMean[cId]);
		}

// 		IndexType min_idx = std::min_element(errMean,errMean + nCluster) - errMean;
// 		if (labels[i] == min_idx) //数据到自己类误差小则标记为inliers
// 		{
// 			inliers.insert(i);
// 		}
// 		delete [] errMean;
// 	    setInliers(id2Id,inliers);
		//fprintf(in_label," \n");

	}//end for samples

	//fclose(in_label);


 	smoothError(totDist,id2Id);//对误差进行滤波
//  
//  	//output the data
// 
// 	for (IndexType i = 0; i < nSamples; i ++)
// 	{
// 		for (IndexType j = 0; j < nCluster; j++)
// 		{
// 			//fprintf(in_label,"%f ",totDist(i,j));
// 			m_gc->setDataCost(i,j,totDist(i,j));
// 		}
// 		//fprintf(in_label," \n");
// 	}
// 	//fclose(in_label);

	//set inliers to visual
	for (int i = 0; i < nSamples; i ++)
	{
		ScalarType * errMean = new ScalarType[nCluster]; //统计每个点的误差
		for(int cId = 0; cId<nCluster; ++cId) 
		{
			errMean[cId] = totDist(i,cId);
		}
		IndexType min_idx = std::min_element(errMean,errMean + nCluster) - errMean;
		if (labels[i] == min_idx) //数据到自己类误差小则标记为inliers
		{
			inliers.insert(i);
		}
		delete [] errMean;
	}
	setInliers(id2Id,inliers);

	delete [] totTrans;
	delete [] totRotmate;

}
//-----------------------------------------
void GCop::ransacMultiRotTan(vector<GraphCutNode*>& oriData,MatrixXX & totDist)
{
	IndexType nSamples = oriData.size();
	int nCluster = m_nLabels;

	vector<IndexType> labels;
	labels.resize(nSamples,0);

	vector<IndexType> nLabels;
	nLabels.resize(nCluster,0);

	map<IndexType,IndexType> id2Id;	//记录对应

	set<IndexType> inliers;//记录内部点的索引[对应后的索引并非原始点云中的索引]
	inliers.clear();

	getInitLabels(oriData,labels,nLabels);//记录每个点的label号码,以及各自号码有多少个点

	IndexType nodeId = 0;
	for (auto iter = gcNode->node_vec.begin(); iter!= gcNode->node_vec.end(); iter ++,nodeId ++)
	{
		IndexType pId = (*iter)->index;
		id2Id[nodeId] = pId;
	}

	ScalarType noiserR = 0.7;//m_nTradeOff;
	IndexType iterNum = 2;//m_nExpansion;//迭代次数

	vector< set<IndexType> >labelIdSet;//记录每个label对应的点graph ID索引
	labelIdSet.resize(nCluster);

	int pId = 0;
	for (auto it = labels.begin(); it != labels.end(); ++it,pId++)
	{
		labelIdSet[(*it)].insert(pId);
	}

	IndexType fId = gcNode->node_vec[0]->frame;
	Sample & curF = gcNode->m_smpSet[fId];

	m_curFId = fId;

	IndexType trajLen = gcNode->node_vec[0]->cor_frame_index.size();

	m_inliers.resize(curF.num_vertices(),false);//为了可视化inliers和outliers

	MatrixXX ** totTrans;
	MatrixXX ** totRotmate;

	totTrans = new MatrixXX *[trajLen];
	totRotmate = new MatrixXX *[trajLen];

	for (IndexType i = 0; i < trajLen; i++)
	{
		totTrans[i] = new MatrixXX [nCluster];
		totRotmate[i] = new MatrixXX [nCluster];
	}

	IndexType frameId = 0;
	for (IndexType trajIt = -trajLen/2; trajIt <= trajLen/2; trajIt ++)
	{
		if (0 == trajIt) continue;

		IndexType nextFrameId = fId + trajIt;//绝对帧号码
		Sample & nextF = gcNode->m_smpSet[nextFrameId];

		for ( int i = 0; i < nCluster; i ++)
		{
			IndexType pSize = labelIdSet[i].size();
			Matrix3X sCoor;
			Matrix3X tCoor;
			sCoor.setZero(3,pSize);
			tCoor.setZero(3,pSize);

			int j = 0;
			for (auto it = labelIdSet[i].begin(); it != labelIdSet[i].end(); it ++,j++)
			{
				IndexType pd = id2Id[(*it)];
				sCoor.col(j) = curF.vertices_matrix().col(pd);//可以优化--只需要取出一次就可以

				IndexType nextPd =  gcNode->node_vec[(*it)]->cor_frame_index[nextFrameId];
				PointType nextPoint =  nextF.vertices_matrix().col(nextPd);
				tCoor.col(j) = nextPoint;
			}

			MatrixXX trans;
			Matrix33 rotmate;//model- gauss distribution

			trans.setZero(1,3);
			rotmate.setIdentity(3,3);

			inliers.clear();
			initRotTrans(sCoor,trans,rotmate,inliers,tCoor);
			rotRansac(sCoor,trans,rotmate,inliers,noiserR,iterNum,labelIdSet[i],tCoor);


			totTrans[frameId][i] = trans;
			totRotmate[frameId][i] = rotmate;

		} //end  for label

		frameId ++;//相对帧号码

	}//end for traj idex

	MatrixXX allDist;//每个点误差
	allDist.setZero(trajLen,nCluster);

	totDist.setZero(nSamples,nCluster);

	inliers.clear();
	IndexType startF = fId - trajLen/2;
	for ( int i = 0; i < nSamples; i ++ )
	{
		IndexType pId = id2Id[i];
		PointType tPoint = curF.vertices_matrix().col(pId);

		IndexType itframe = 0;
		for (int k = 0; k <= trajLen; k ++)
		{
			IndexType nextframe = startF + k;
			if (nextframe == fId) continue;
			IndexType corPid = gcNode->node_vec[i]->cor_frame_index[nextframe];
			Sample & nextF = gcNode->m_smpSet[nextframe];

			PointType corPoint = nextF.vertices_matrix().col(corPid);

			for (int j = 0; j < nCluster; j ++)
			{
				PointType transPoint = totRotmate[itframe][j] * tPoint + totTrans[itframe][j];
				ScalarType dist = (corPoint - transPoint).norm();
				allDist(itframe,j) = dist;
			}
			itframe ++;
		}

		ScalarType * errMean = new ScalarType[nCluster]; //统计每个点的误差
		for(int cId = 0; cId<nCluster; ++cId) 
		{
			errMean[cId] = allDist.col(cId).sum();
			totDist(i,cId) = errMean[cId];
		}

		delete [] errMean;

	}//end for samples

	//smoothError(totDist,id2Id);//对误差进行滤波

	for (IndexType i = 0; i < nSamples; i ++)
	{
		for ( IndexType j = 0; j < nCluster; j++)
		{
			m_gc->setDataCost(i,j,m_nCurNeig *totDist(i,j));
		}
	}

}
//------------------------------------------
void GCop::smoothError(MatrixXX& totDist,map<IndexType,IndexType>& id2Id)
{

	IndexType nLabels = totDist.cols();
	IndexType nSmapleSize = totDist.rows();

	MatrixXX smoothTotDist;
	smoothTotDist.setZero(nSmapleSize,nLabels);

	map<IndexType,IndexType> inverId2Id;

	for (IndexType i = 0; i < id2Id.size(); i++)
	{
		inverId2Id.insert(make_pair(id2Id[i],i) );
	}

	map<IndexType,IndexType>::iterator IsValidIter;

	Sample & curF = gcNode->m_smpSet[m_curFId];
	const IndexType k = 50;
	IndexType neighbours[k];
	ScalarType dist[k];

	vector<IndexType>  gNodeId;
	vector<ScalarType> rDist;

	//对权重进行滤波
	for (IndexType smpId = 0; smpId < nSmapleSize; smpId ++)
	{
		IndexType pId = id2Id[smpId];
		curF.neighbours(pId,k,neighbours,dist);
		for(IndexType neig_id = 0; neig_id < k; neig_id ++)
		{
			IsValidIter = inverId2Id.find(neighbours[neig_id]);//find 给定的关键词错误
			if(IsValidIter != inverId2Id.end())
			{
				gNodeId.push_back(IsValidIter->second);
				rDist.push_back(dist[neig_id]);
			}
		}

		//开始平滑处理
		assert(rDist.size() > 0);
		ScalarType totW = 0.;
		for (IndexType i = 0; i < rDist.size(); i ++)
		{
			ScalarType tDis = rDist[i]; 
			rDist[i] = exp(-tDis*tDis);
			totW += rDist[i];
		}

		for (IndexType i = 0; i < rDist.size(); i ++)
		{
			rDist[i] /= totW;
		}


		for (IndexType labId = 0; labId < nLabels; labId ++) //每列的距离进行单独计算
		{
			ScalarType resErr = 0.;
			for (IndexType i = 0; i < rDist.size(); i ++)//距离此时变成了权重
			{
				resErr += (rDist[i] * totDist(gNodeId[i],labId)) ;
			}

			smoothTotDist(smpId,labId) = resErr;
		}

		gNodeId.clear();
		rDist.clear();

	}//end for smpId

	totDist = smoothTotDist;

	//对权重进行归一化---
	for (IndexType smpId = 0; smpId < nSmapleSize; smpId ++)
	{
		MatrixXX temp;
		temp.setZero(1,nLabels);

		ScalarType rowTotD = totDist.row(smpId).sum();
		for (IndexType labId = 0; labId < nLabels; labId ++)
		{
			totDist(smpId,labId) /= rowTotD;
		}
	}


}

//--------------------------------------
void GCop::rotRansac(Matrix3X& trans,MatrixXX& miu,Matrix33& sigma,set<IndexType>&inliers,
					 ScalarType noiseR,IndexType itNum,set<IndexType>& graphId,Matrix3X& tCoor)
{
	while (itNum-- > 0)
	{
		//1:计算所有样本点到分布的概率
		//2:用内部点拟合处一个分布
		dist2Transformation(trans,miu,sigma,inliers,noiseR,graphId,tCoor);

		Matrix3X smpCoor;
		getSmpCoor(smpCoor,trans,inliers);

		Matrix3X smpCorCoor;
		getSmpCorCoor(smpCorCoor,inliers,tCoor);

		point2point(smpCoor,smpCorCoor,sigma,miu);
		
	}
}
//---------------------------------------
void GCop::getSmpCorCoor(Matrix3X& smpCoor,set<IndexType>&inliers,Matrix3X& tCoor)
{
	smpCoor.setZero(3,inliers.size());
	IndexType i = 0;
	for(auto it = inliers.begin(); it != inliers.end(); it ++,i++)
	{
		smpCoor.col(i) = tCoor.col(*it);
	}
}
//---------------------------------------
void GCop::dist2Transformation(Matrix3X& srCoor,MatrixXX& miu,Matrix33& sigma,set<IndexType>&inliers,ScalarType noiseR,set<IndexType>& graphId,Matrix3X& tCoor)
{
	vector<ScalarType> distRecord;
	distRecord.resize(srCoor.cols(),0.);

	for (int i = 0; i < srCoor.cols(); i++)
	{
		PointType pCoor = srCoor.col(i);
		PointType tPoint = sigma * pCoor + miu;//变换到下一帧
		PointType corCoor = tCoor.col(i);
		ScalarType dist = (corCoor - tPoint).norm();
		distRecord[i] = dist;
	}

	vector<ScalarType> distCopy;
	distCopy = distRecord;

	sort(distRecord.begin(),distRecord.begin() + distRecord.size());
	//sort(distRecord.begin(),distRecord.end() );

	ScalarType markDist = distRecord [( noiseR * distRecord.size())];

	//Logger<<"mark value = "<<markDist<<endl;

	inliers.clear();
	for (int i = 0; i < srCoor.cols(); i++)
	{
		if (distCopy[i] <= markDist /*|| distCopy[i] == markDist*/)
		{
			inliers.insert(i);
		}
	}
}

//---------------------------------------
void GCop::initRotTrans(Matrix3X& trans,MatrixXX& miu,Matrix33& sigma,set<IndexType>&inliers,Matrix3X&tCoor)
{
	//IndexType nSmpSize = m_nCurNeig;
	IndexType nSmpSize = 25;
	

	Matrix3X srCloud;
	Matrix3X tgCloud;
	srCloud.setZero(3,nSmpSize);
	tgCloud.setZero(3,nSmpSize);

	assert(trans.cols() > 0);

	
	IndexType i = 0;
	while (nSmpSize-- > 0)
	{
		IndexType num = rand()%trans.cols();
		srCloud.col(i) = trans.col(num);
		tgCloud.col(i) = tCoor.col(num);

		inliers.insert(num);
		i++;
	}

	point2point(srCloud,tgCloud,sigma,miu);


}
//---------------------------------------
// void GCop::getCorCoor(PointType& pCoor,IndexType gId)
// {
// 	IndexType fId = gcNode->node_vec[gId]->frame;
// 	IndexType corId = gcNode->node_vec[gId]->cor_frame_index[fId + 1];
// 	Sample & nextF = gcNode->m_smpSet[fId + 1];
// 
// 	pCoor = nextF.vertices_matrix().col(corId);
// }
//---------------------------------------fit a gauss distribution

void GCop::initDistribution(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma,set<IndexType>&inliers)
{
	IndexType nSmpSize = m_nCurNeig;

	//IndexType nSmpSize = 50;
	vector<IndexType> recordId;
	recordId.resize(nSmpSize,0);

	MatrixXX smpTrans;
	smpTrans.setZero(nSmpSize,m_nDim);

	IndexType i = 0;
	while (nSmpSize-- > 0)
	{
		IndexType num = rand()%trans.rows();
		smpTrans.row(i) = trans.row(num);
		inliers.insert(num);
		i++;
	}

	calculateMeanVariance(smpTrans,miu,sigma);
}
//---------------------------------------
void GCop::gaussRansac(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma,set<IndexType>&inliers,ScalarType noiseR,IndexType itNum)
{
	while (itNum-- > 0)
	{
		//1:计算所有样本点到分布的概率
		//2:用内部点拟合处一个分布
		dist2Distribution(trans,miu,sigma,inliers,noiseR);

		MatrixXX smpCoor;
		getSmpTrans(smpCoor,trans,inliers);
        calculateMeanVariance(smpCoor,miu,sigma); 
	}
}
//---------------------------------------
void GCop::dist2Distribution(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma,set<IndexType>&inliers,ScalarType noiseR)
{
	vector<ScalarType> distRecord;
	distRecord.resize(trans.rows(),0.);

	//输出点到面的距离
//  	char outputLabelName[256]; 
//  	sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\17_prob_dist.txt");
//  	FILE *in_label = fopen(outputLabelName,"w");


	for (int i = 0; i < trans.rows(); i++)
	{
		MatrixXX s_id = trans.row(i);
		ScalarType dist = calculateProb(m_nDim,miu,sigma,s_id);
		distRecord[i] = dist;
		//fprintf(in_label,"%f \n",dist);
		//Logger<<"pro = "<<dist<<endl;
	}

	//fclose(in_label);

	vector<ScalarType> distCopy;
	distCopy = distRecord;

	sort(distRecord.begin(),distRecord.begin() + distRecord.size());
	ScalarType markDist = distRecord [( (1-noiseR) * distRecord.size())];

	//Logger<<"mark value = "<<markDist<<endl;

	inliers.clear();
	for (int i = 0; i < trans.rows(); i++)
	{
		if (distCopy[i] > markDist || distCopy[i] == markDist)
		{
			inliers.insert(i);
		}
	}
}
void GCop::calculateMeanVariance(MatrixXX& trans,MatrixXX& miu,MatrixXX& sigma)
{

	miu = trans.colwise().mean();//均值(1×3)
	trans.rowwise() -= miu.row(0);
    
	sigma = trans.transpose() * trans;

	sigma /= trans.rows();

}

void GCop::getSmpTrans(MatrixXX & smpTrans,MatrixXX&oriTrans,set<IndexType>& inliers)
{
	IndexType pSize = inliers.size();

	assert(pSize > 0);
	smpTrans.setZero(pSize,m_nDim);

	set<IndexType>::iterator it = inliers.begin();
	int i = 0;

	for (;it != inliers.end(); ++it,++i)
	{
		smpTrans.row(i) =  oriTrans.row((*it));
	}
}
//----------------------------------------fit a plan
void GCop::setInliers(map<IndexType,IndexType>& id2Id,set<IndexType>& inliers)
{
	set<IndexType>::iterator it = inliers.begin();
	int i = 0;

	for (;it != inliers.end(); ++it,++i)
	{
		IndexType allId = id2Id[(*it)];
		m_inliers[allId] = true;
	}	
}
//----------------------------------------
void GCop::realRansac(Matrix3X& oriCoor,PointType& center,NormalType& planNorm,set<IndexType>& inliers,ScalarType noiseR,IndexType iterNum)
{
	while (iterNum-- > 0)
	{
		//1:计算所有点到平面的距离,选取出内部点
		//2:用内部点拟合处一个平面
		dist2Plan(oriCoor,center,planNorm,inliers,noiseR);
		
		Matrix3X smpCoor;
		getSmpCoor(smpCoor,oriCoor,inliers);
		calculateCenterNorm(smpCoor,center,planNorm);
	}

	//Logger<<inliers.size()<<endl;
}

//----------------------------------------
void GCop::dist2Plan(Matrix3X& oriCoor,PointType& center,NormalType& planNorm,set<IndexType>& inliers,ScalarType noiseR)
{
	vector<ScalarType> distRecord;
	distRecord.resize(oriCoor.cols(),0.);

	//输出点到面的距离
	char outputLabelName[256]; 
	sprintf(outputLabelName,"C:\\Users\\kobe\\Desktop\\plan_dist.txt");
	FILE *in_label = fopen(outputLabelName,"w");

	for (int i = 0; i < oriCoor.cols(); i++)
	{
		PointType coor = oriCoor.col(i);
		ScalarType dist = planNorm.transpose() * (coor - center);
		distRecord[i] = abs(dist);
		fprintf(in_label,"%f \n",dist);
	}

	fclose(in_label);

	vector<ScalarType> distCopy;
	distCopy = distRecord;

	sort(distRecord.begin(),distRecord.begin() + distRecord.size());
	ScalarType markDist = distRecord [(noiseR * distRecord.size())];

	inliers.clear();
	for (int i = 0; i < oriCoor.cols(); i++)
	{
		if (distCopy[i] <= markDist)
		{
			inliers.insert(i);
		}
	}

}

//----------------------------------------
void GCop::getSmpCoor(Matrix3X&smpCoor,Matrix3X & oriCoor,set<IndexType>& inliers)
{
	IndexType pSize = inliers.size();
	
	smpCoor.setZero(3,pSize);

	set<IndexType>::iterator it = inliers.begin();
	int i = 0;

	for (;it != inliers.end(); ++it,++i)
	{
		smpCoor.col(i) =  oriCoor.col((*it));
	}

}

//----------------------------------------
void GCop::initPlan(Matrix3X & oriCoor,PointType & center,NormalType & planNorm,set<IndexType>& inliers)
{
  IndexType nSmpSize = m_nCurNeig;
  vector<IndexType> recordId;
  recordId.resize(nSmpSize,0);

  Matrix3X smpCoor;
  smpCoor.setZero(3,nSmpSize);

  IndexType i = 0;
  while (nSmpSize-- > 0)
  {
	  IndexType num = rand()%oriCoor.cols();
      smpCoor.col(i) = oriCoor.col(num);
	  inliers.insert(num);
	  i++;
  }


  calculateCenterNorm(smpCoor,center,planNorm);

}

void GCop::drawPlan(PointType&center,NormalType&planNorm)
{
	Tracer& cor = Tracer::get_instance();
	cor.drawPlan(center,planNorm);
}

//----------------------
void GCop::calculateCenterNorm(Matrix3X& smpCoor,PointType& center,NormalType & planNorm)
{
	center =  smpCoor.rowwise().mean();

	for (IndexType i = 0; i<smpCoor.cols();i++)
	{
		smpCoor.col(i) -= center;
	}

	Matrix33 sigma = smpCoor * smpCoor.transpose();

	Eigen::EigenSolver<Matrix33> eigen_solver(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);

	auto ev = eigen_solver.eigenvectors();
	auto eval = eigen_solver.eigenvalues();
	ScalarType tmp[3] = { eval(0).real(),eval(1).real(),eval(2).real() };
	IndexType min_idx = std::min_element(tmp,tmp+3) - tmp;
	planNorm(0) = (ev.col(min_idx))(0).real();
	planNorm(1) = (ev.col(min_idx))(1).real();
	planNorm(2) = (ev.col(min_idx))(2).real();

	planNorm.normalize();

}
//----------------------
void GCop::coSegmentation()
{
	char* in_label_file = IN_LABEL_FILE_GCOP;
	char* in_corr_file = IN_CORR_FILE_GCOP;
	char* out_label_file = OUT_LABEL_FILE_GCOP;

	//char* in_label_file = "C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\RANSAC0602\\tot_labels(10_20).txt";
	//char* in_corr_file = "C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\RANSAC0602\\tot_corr(10_20).txt";
	//char* out_label_file = "C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\2015-3-10-算法在设计\\horse_scanningInter\\RANSAC0602\\0602AlloutputLabels_rd2.txt";

	DualwayPropagation& dp_solver = DualwayPropagation::get_instance();
	dp_solver.read_data(in_label_file, in_corr_file);
	visualCosegmentation(in_label_file);
	//dp_solver.compute();
	//dp_solver.write_label_file(out_label_file);

	//CoSegmentation coseg_solver;
	//coseg_solver.read_data(out_label_file, in_corr_file);
	//coseg_solver.compute();
	//coseg_solver.write_label_file(out_label_file);

	//visualCosegmentation(out_label_file);
	////visualCosegOriPointCloud(out_label_file);
}
//--------------------------
void GCop::visualCosegmentation(char *labels_file)
{
	FILE* in_file = fopen(labels_file,"r");
	if (in_file==NULL)
	{
		return;
	}

	IndexType frame, label, vtx_idx;

	SampleSet& smpSet = SampleSet::get_instance();

	IndexType frames[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

	for ( IndexType i=0; i<(sizeof(frames)/sizeof(IndexType));i++ )
	{
		for (IndexType j=0; j<smpSet[frames[i]].num_vertices(); j++)
		{
			smpSet[frames[i]][j].set_visble(false);
		}
	}

	while (true)
	{
		int stat = fscanf(in_file, "%d %d %d\n",&frame, &label, &vtx_idx);
		if(stat==EOF)break;
		smpSet[frame][vtx_idx].set_visble(true);
		//smpSet[frame][vtx_idx].set_label(label);
	}
	fclose(in_file);
}

//--------------------------
void GCop::visualCosegOriPointCloud(char *labels_file)
{

	gcNode = new GraphNodeCtr();

	gcNode->node_vec.clear();

	gcNode->node_map.clear();

	gcNode->read_label_file(labels_file);

	IndexType frame, label, vtx_idx;

	SampleSet& smpSet = SampleSet::get_instance();

	IndexType frames[] = {/*1,2,3,4,5,6,7,8,9,*/10,11,12,13,14,15,16,17,18,19,20};


	const IndexType k = 300;
	IndexType neighbours[k];
	ScalarType dist[k];

	IndexType result_label;

	for ( IndexType i=0; i<(sizeof(frames)/sizeof(IndexType));i++ )
	{
		for (IndexType j=0; j<smpSet[frames[i]].num_vertices(); j++)
		{
			//smpSet[frames[i]][j].set_visble(false);
			vector<IndexType> recordLabelTime(15,0);
			result_label = -1;

			IndexType temp_label;

			smpSet[frames[i]].neighbours(j,k,neighbours,dist);

			for(IndexType neig_id = 0; neig_id < k; neig_id ++)
			{
				IndexType pId = neighbours[neig_id];

				GraphCutNode *fNode = gcNode->node_map[frame_index_to_key(frames[i],pId)];

				if(fNode)
				{
					recordLabelTime[fNode->label] += 1;
				}
			}

			for (int i = 0; i <= 14; i++)
			{
				if(result_label < recordLabelTime[i])
				{
					temp_label = i;
					result_label = recordLabelTime[i];
				}
			}

			smpSet[frames[i]][j].set_label(temp_label);

		}//end  for vertex

	}//end  for frame
}
//--------------------------example using EM algorithm
