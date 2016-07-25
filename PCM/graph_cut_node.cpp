#include "graph_cut_node.h"
#include <stdio.h>
#include "engine.h"
#ifdef WIN32
#include <direct.h>
#define get_current_dir _getcwd
#else
#include <unistd.h>
#define get_current_dir getcwd
#endif
#include <cstring>
#include"sample_set.h"
#include "vertex.h"
#include "bfs_classifier.hpp"

#include"sample_properity.h"

void GraphNodeCtr::run()
{
	Logger<<"Start Graph Cut!.\n";

	IndexType nbCluster = 8;
	Logger<<"segment number = "<<nbCluster<<endl;
	bool singleFrame = true;
	if (singleFrame)//processing single frame  aim to smooth boundary
	{
		//for('read file'){}
		//read_smooth_label_file("label_smp000_0.01200.txt");

		//2015-1-1-box
		//read_smooth_label_file("C:\\Users\\kobe\\Desktop\\graphCut\\2015-1-1 box\\label_cor_\\box_labels_smp05_10.00000.txt");//horse-(0-19)-顺序搞错了当时
		
		//read_smooth_label_file("box_labels_smp15_8.00000.txt");
		//read_smooth_label_file("shiqun_labels_smp10_13.00000.txt");

		//read_smooth_label_file("shiqun_labels_smp06_14.50000.txt");

		//read_smooth_label_file("shiqun_labels_smp10_19.50000.txt");

		//read_smooth_label_file("boxN_labels_smp20_25.00000.txt");
		/*read_smooth_label_file("box_labels_smp05_10.00000.txt");*/

	//	read_smooth_label_file("C:\\Users\\kobe\\Desktop\\graphCut\\2015-1-3-shiqun-walk\\labels-cor-info\\shiqun_labels_smp10_25.00000.txt");
		//read_smooth_label_file("sq_walk_labels_smp25_0.70.txt");
		//read_label_file("box_labels11_0.60.txt");

		read_smooth_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hand_leg_cor_labels\\hand_leg_labels_smp20_0.60.txt");
		pca_box_ctr();

		Engine*	ep;
		if (! (ep = engOpen(NULL)) )
		{
			Logger<< "Can't not start Matlab engine.\n";
			return;
		}

	//	///set buffer to display result
			IndexType	result_buffer_size = 1024*1000;
		char*		result_buffer = new char[result_buffer_size];
		engOutputBuffer(ep, result_buffer, result_buffer_size);

		///Get the executable file's path
			char cur_path[FILENAME_MAX];
		if (!get_current_dir(cur_path, sizeof(cur_path)))
		{
			return;
		}
		cur_path[sizeof(cur_path) - 1] = '\0';
		strcat(cur_path,"\\nCut");
		char cd_cur_path[FILENAME_MAX + 3] = "cd ";
		strcat(cd_cur_path, cur_path);
		engEvalString(ep, cd_cur_path );

		IndexType n = cur_graph_index_;
	//	//

	//	//
		mxArray *mx_distMat = NULL;
		numeric::float64* dm_buffer;
		dm_buffer = new numeric::float64[n*n];
		mx_distMat = mxCreateDoubleMatrix(n,n,mxREAL);

	//	///all
		for (int rowId = 0; rowId < n; rowId++)
		{
			/*dm_buffer[rowId*(n+1)] = 0;*/
			for (int colId = 0; colId < n; colId++)
			{
				ScalarType dist = weight2nodes_smooth(node_vec[rowId],node_vec[colId]);
				dm_buffer[rowId * n + colId] = (numeric::float64)dist;
			}
		}

		memcpy((char*)mxGetPr(mx_distMat),(char*)dm_buffer,n*n*sizeof(numeric::float64));
		delete [] dm_buffer;
		engPutVariable(ep,"W",mx_distMat);

		char cmd_buf[128];
		sprintf(cmd_buf,"[NcutDiscrete,NcutEigenvectors,NcutEigenvalues] = ncutW(W,%d);",nbCluster);
		engEvalString(ep,cmd_buf);

	//	///Display output information
	//		Logger<<result_buffer<<std::endl;

		mxArray *mx_NcutDiscrete = NULL;
		mx_NcutDiscrete = engGetVariable(ep,"NcutDiscrete");

		numeric::float64 *ncutDiscrete = mxGetPr(mx_NcutDiscrete);
		IndexType k=0;
		for ( IndexType i=0;i<nbCluster;i++ )
		{
			for (IndexType j=0;j<n;j++)
			{
				if ( ncutDiscrete[k++]!=0 )
				{
					node_vec[j]->graph_label = i;
				}
			}
		}


	//	//Visualize -12-26
		SampleSet &smp_set = SampleSet::get_instance();
		IndexType frames[] = {20/*,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19*/};

		//save the labels to kmeans 

 		vector<IndexType> vtxId;
 		vector<IndexType> vtxLabel;
 		for (int i = 0; i < n; i++)
 		{
 			vtxId.push_back(node_vec[i]->index);
 			/*vtxLabel.push_back(node_vec[i]->graph_label);*/
			vtxLabel.push_back(node_vec[i]->label);
 		}
 
 		diff_using_bfs(vtxLabel,vtxId,node_vec[0]->frame);
 
 		///identity new labels to nodes
 
 		//for (int i=0;i<n;i++)
 		//{
 		//	node_vec[i]->graph_label = vtxLabel[i];
 		//}
// 
// 
// 		/// transfer to orginal point cloud
		vector<IndexType> oriVtxLabels(smp_set[node_vec[0]->frame].num_vertices(),0);
		propagateLabel2Orignal(smp_set[node_vec[0]->frame],vtxId,vtxLabel,oriVtxLabels);
// 
// 		char label_file_name[256];
// 		char corr_file_name[256];
// 		sprintf(label_file_name,"hand_leg_labels%.2d.txt",frames[0]);
// 		sprintf(corr_file_name,"hand_leg_sample_labels%.2d.txt",frames[0]);
// 		FILE *in_label = fopen(label_file_name, "w");
// 		FILE *in_correspond = fopen(corr_file_name,"w");
// 
// 		IndexType i = 0;
// 		for (Sample::vtx_iterator v_iter = smp_set[node_vec[0]->frame].begin();
// 			v_iter != smp_set[node_vec[0]->frame].end();
// 			v_iter++,i++ )
// 		{
// 			fprintf(in_label,"%d %d %d\n",frames[0],oriVtxLabels[i],i);
// 		}
// 
// 		for (int i=0;i<n;i++)
// 		{
// 			fprintf(in_correspond,"%d %d %d\n",frames[0],vtxLabel[i],vtxId[i]);
// 		}
// 		fclose(in_correspond);
// 		fclose(in_label);
// 
		///visible ori poings

 		IndexType t = 0;
 		for (Sample::vtx_iterator v_iter = smp_set[node_vec[0]->frame].begin();
 			v_iter != smp_set[node_vec[0]->frame].end();
 			v_iter++,t++ )
 		{
 			(*v_iter)->set_label(/*node_vec[t]->label*/oriVtxLabels[t]);
 		}


		///visible sample points
// 		for ( IndexType i=0; i<1;i++ )
// 		{
// 			for (IndexType j=0; j<smp_set[frames[i]].num_vertices(); j++)
// 			{
// 				smp_set[frames[i]][j].set_visble(false);
// 			}
// 		}
// 		for ( IndexType i=0; i<n;i++ )
// 		{
// 			GraphCutNode &node = *(node_vec[i]);
// 			smp_set[node.frame][node.index].set_label(/*node.label*/node.graph_label);
// 			smp_set[node.frame][node.index].set_visble(true);
// 		}




		Logger<<"finished\n";
		return ;
	}



	//read_label_file("labelInfo.txt");
	//read_label_file("labelInfo_edit.txt");
	//read_label_file(".\\graphCut\\labelInfo_28.txt");
	//read_corres_file(".\\graphCut\\corInfo.txt");

// 	read_label_file("labelInfo_456.txt");
// 	read_corres_file("corInfo_456.txt");

	//read_label_file("C:\\Users\\kobe\\Desktop\\graphCut\\12-27-sample-horse\\label_cor_info(0_9)\\horse_labels(0_9).txt");//horse
	//read_corres_file("C:\\Users\\kobe\\Desktop\\graphCut\\12-27-sample-horse\\label_cor_info(0_9)\\horse_cor(0_9).txt");


	//2014-12-29

	//read_label_file("C:\\Users\\kobe\\Desktop\\graphCut\\12-27-sample-horse\\label_cor_info(0_19)\\labels.txt");//horse-(0-19)
	//read_corres_file("C:\\Users\\kobe\\Desktop\\graphCut\\12-27-sample-horse\\label_cor_info(0_19)\\cor.txt");

	//2014-12-22
	//read_label_file("tot_labels_dancer.txt");
	//read_corres_file("tot_cor_dancer.txt");

	//20150108-xudong-horse-(3-4-5)-non-interpolation
	//read_label_file("horse_labels_345.txt");
	//read_corres_file("horse_cor_345.txt");


	//7-8-9 co-segmenation示意图--horse
	//read_label_file("smp_labels_all.txt");
	//read_corres_file("smp_cor_all.txt");


	//7-8
	//read_label_file("smp_labels_7_8.txt");
	//read_corres_file("smp_cor_7_8.txt");


	//0111--9-10-11--box
	//read_smooth_label_file("labels_091011.txt");
	//read_corres_file("corrsp_091011.txt");

	//walk - 0-3
 	//read_smooth_label_file("walk_labels_0_4.txt");
 	//read_corres_file("walk_corre_0_4.txt");

// 	read_smooth_label_file("labels(23_24).txt");
// 	read_corres_file("corres(23_24.txt)");


	//read_smooth_label_file("handup_5_15_labels.txt");//单手
	//read_corres_file("handup_5_15_corres.txt");

	//手脚一起摆动
	//read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\cluster_result\\co-segmentation(15_25)\\labels_15_25.txt");
	//read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\cluster_result\\co-segmentation(15_25)\\corres_15_25.txt");


	//read_smooth_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hand_leg_cor_labels\\co-segmentation(6_7_8)\\labels(6_7_8).txt");
	//read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hand_leg_cor_labels\\co-segmentation(6_7_8)\\corres(6_7_8).txt");


	//read_smooth_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hand_leg_cor_labels\\18_19_20\\labels.txt");
	//read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hand_leg_cor_labels\\18_19_20\\corres.txt");

	//update 18 frame results
	//read_smooth_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hand_leg_cor_labels\\new_18_19_20\\labels.txt");
	//read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hand_leg_cor_labels\\new_18_19_20\\corres.txt");



	//scanning hands- teaser
	//read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hands\\labels_cor_info\\diff_labels(3_5).txt");
	//read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hands\\labels_cor_info\\diff_corres(3_5).txt");


	read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hands\\labels_cor_info\\diff_labels(0_7).txt");
	read_corres_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hands\\labels_cor_info\\diff_corres(0_7).txt");

	//read_label_file("C:\\Users\\kobe\\Desktop\\论文实验内容2014-12-30\\hands\\labels_cor_info\\final_labels(01234567).txt");

	IndexType n = cur_graph_index_;

	pca_box_ctr();//计算包围盒信息
 	calculateNorm();//计算法向
 	////construct boxes distances
 	constructBoxDis(label_bucket);//建立包围盒之间的对应准确率
 
 	Engine*	ep;
 	if (! (ep = engOpen(NULL)) )
 	{
 		Logger<< "Can't not start Matlab engine.\n";
 		return;
 	}
 
 	// set buffer to display result
 	IndexType	result_buffer_size = 1024*1000;
 	char*		result_buffer = new char[result_buffer_size];
 	engOutputBuffer(ep, result_buffer, result_buffer_size);
 
 	//Get the executable file's path
 	char cur_path[FILENAME_MAX];
 	if (!get_current_dir(cur_path, sizeof(cur_path)))
 	{
 		return;
 	}
 	cur_path[sizeof(cur_path) - 1] = '\0';
 	strcat(cur_path,"\\nCut");
 	char cd_cur_path[FILENAME_MAX + 3] = "cd ";
 	strcat(cd_cur_path, cur_path);
 	engEvalString(ep, cd_cur_path );
 
 
 	//
 	m_totNode = n;
 	Logger<<"total nodes number = "<<m_totNode<<endl;
 	//
 	mxArray *mx_distMat = NULL;
 	
 	double percent_sparse = 0.2;
 	mwSize	nzmax = n*n*percent_sparse;
 	mx_distMat = mxCreateSparse(n,n, nzmax,mxREAL);
 	double* sr;
 	mwIndex*	irs, * jcs;
 	sr = mxGetPr(mx_distMat);
 	irs = mxGetIr(mx_distMat);
 	jcs = mxGetJc(mx_distMat);
 
 
 	//ScalarType avg_a_dis ,a_mid;
 	//compute_mid_and_avg(avg_a_dis,a_mid,&GraphNodeCtr::dist_inside_frame);
 	//Logger<<"avg and mid of adjacency"<<avg_a_dis<<" "<<a_mid<<endl;
 	//ScalarType avg_cor_dis,cor_dis;
 	//compute_mid_and_avg(avg_cor_dis,cor_dis,&GraphNodeCtr::dist_between_frame);
 	//Logger<<"avg and mid of corresponcdence"<<avg_cor_dis<<" "<<cor_dis<<endl;
 
 
 	ScalarType a_w = 1.00f;
 	mwSize k = 0;
 	for (int i=0; i<n;i++)
 	{
 		jcs[i] = k; 
 		//FILE *dist_file = fopen("hans_dist.txt","w");
 
 		for (int j=0; j<n; j++)
 		{
 			//ScalarType dist = weight2nodes(node_vec[i],node_vec[j]);
 			///ScalarType dist = i==j?a_w:weight2nodes(node_vec[i],node_vec[j],a_w);
 			ScalarType dist = i==j?a_w:weight2nodes_dis(node_vec[i],node_vec[j],a_w);//paper method 0106
 
 			//if(dist !=0.0) fprintf(dist_file,"%d %d %lf\n",node_vec[j]->frame,node_vec[j]->index,dist);
 
 			
 			if(dist ==0.0)
 				continue;
 			if(k>=nzmax)
 			{
 				 /* Check to see if non-zero element will fit in
                  * allocated output array.  If not, increase percent_sparse
                  * by 10%, recalculate nzmax, and augment the sparse array
                  */
 				mwSize oldnzmax = nzmax;
 				percent_sparse += 0.1;
 				nzmax = (mwSize)ceil((double)n*(double)n*percent_sparse);
 				/* make sure nzmax increases at least by 1 */
 				if (oldnzmax == nzmax)
 					nzmax++;
 				mxSetNzmax(mx_distMat, nzmax);
 				mxSetPr(mx_distMat, (double*)mxRealloc(sr, nzmax*sizeof(double)));
 				mxSetIr(mx_distMat,(mwIndex*) mxRealloc(irs, nzmax*sizeof(mwIndex)));
 
 				sr = mxGetPr(mx_distMat);
 				irs = mxGetIr(mx_distMat);
 			}
 			sr[k] = dist;
 			irs[k] = j;
 			k++;
 		}
 
         //fclose(dist_file);
 	}
 	jcs[n] = k;
 
 
 	//FILE *out_file = fopen("disMat","w");
 	//for (int i=0;i<n;i++)
 	//{
 	//	for (int j=0;j<n;j++)
 	//	{
 	//		//fprintf(out_file,"%lf ",dm_buffer[i*n+j]);
 	//		fprintf(out_file,"%lf ",disMat(i,j));
 	//	}
 	//	fprintf(out_file,"\n");
 	//}
 	//fclose(out_file);
 
 
 	//FILE *in_file = fopen("weight_1_9","r");
 	//for (int i=0;i<n;i++)
 	//{
 	//	for (int j=0;j<n;j++)
 	//	{
 	//		fscanf(in_file,"%lf",&dm_buffer[i*n+j]);
 	//	}
 	//}
 	//fclose(in_file);
 
 
 	engPutVariable(ep,"W",mx_distMat);
 
 	char cmd_buf[128];
 	sprintf(cmd_buf,"[NcutDiscrete,NcutEigenvectors,NcutEigenvalues] = ncutW(W,%d);",nbCluster);
 	engEvalString(ep,cmd_buf);
 
 	//Display output information
 	Logger<<result_buffer<<std::endl;
 
 	mxArray *mx_NcutDiscrete = NULL;
 	mx_NcutDiscrete = engGetVariable(ep,"NcutDiscrete");
 
 	numeric::float64 *ncutDiscrete = mxGetPr(mx_NcutDiscrete);
 	IndexType t=0;
 	for ( IndexType i=0;i<nbCluster;i++ )
 	{
 		for (IndexType j=0;j<n;j++)
 		{
 			if ( ncutDiscrete[t++]!=0 )
 			{
 				node_vec[j]->graph_label = i;
 			}
 		}
 	}

	//Visualize
	SampleSet &smp_set = SampleSet::get_instance();
	IndexType frames[] = {0,1,2,3,4,5,6,7/*18,19,20*//*23,24*//*0,1,2,3*//*,2,3,4*//*9,10,11*//*0,1,2,3,4,*//*15,16,17,18,19,20,21,22,23,24,25*//*,16,17,18,19*/};

	//只可视化sample上的点
 	for ( IndexType i=0; i<(sizeof(frames)/sizeof(IndexType));i++ )
 	{
 		for (IndexType j=0; j<smp_set[frames[i]].num_vertices(); j++)
 		{
 			smp_set[frames[i]][j].set_visble(false);
 		}
 	}
 
 	////// save  labels results
 
 	for ( IndexType i=0; i<n;i++ )
 	{
 		GraphCutNode &node = *(node_vec[i]);
 		smp_set[node.frame][node.index].set_label(/*node.label*/node.graph_label);
 		smp_set[node.frame][node.index].set_visble(true);
 	}


	/////tranfer to ori poin把分割信息传播到原始点云上

	//IndexType fSize = sizeof(frames)/sizeof(IndexType);
	//vector<vector<IndexType> > vtxId(fSize);
	//vector<vector<IndexType> > vtxLabel(fSize);

	//for (int i = 0; i < n; i++)
	//{
	//	IndexType frame = node_vec[i]->frame;

	//	vtxId[frame-frames[0]].push_back(node_vec[i]->index);
	//	vtxLabel[frame-frames[0]].push_back(node_vec[i]->graph_label);
	//	//vtxLabel[frame-frames[0]].push_back(node_vec[i]->label);
	//}


	//vector<vector<IndexType> >resultLabel(fSize);
	//for (IndexType i=0;i<fSize;i++)
	//{
	//	resultLabel[i].resize(smp_set[i +frames[0]].num_vertices());
	//	propagateLabel2Orignal(smp_set[i + frames[0]],vtxId[i],vtxLabel[i],resultLabel[i]);
	//}

	//////给顶点赋号码//k-means
	//for (IndexType i = 0;i< fSize;i++)
	//{
	//	vtxId[i].clear();
	//	for (int j = 0;j< smp_set[i +frames[0]].num_vertices();j++)
	//	{
	//		vtxId[i].push_back(j);
	//	}
	//}
	//for (IndexType i = 0; i < fSize; i++)
	//{
	//	diff_using_bfs(resultLabel[i],vtxId[i],frames[i]);//在j-linkage中相同块分出不同的颜色出来
	//}
	
	//显示采样点可视化效果--保存最终点的label信息到文件中

//	for (IndexType i=0;i<fSize;i++)
//	{
//		int j = 0;
//
//// 		char label_labsmooth[256];
//// 		sprintf(label_labsmooth,"teaser_coseg_0_5_final_labels%.2d.txt",i+frames[0]);
//// 		FILE *in_label_smmoth = fopen(label_labsmooth, "w");
//
//		for (Sample::vtx_iterator v_iter = smp_set[i+frames[0]].begin();
//			v_iter != smp_set[i+frames[0]].end();v_iter++,j++)
//		{
//			//(*v_iter)->set_label(vtxLabel[i][j]);
//			(*v_iter)->set_label(resultLabel[i][j]);
//			//fprintf(in_label_smmoth,"%d %d %d\n",i+frames[0],resultLabel[i][j],j);
//		}
//
//		//fclose(in_label_smmoth);
//	}

	Logger<<"finished\n";
}
void GraphNodeCtr::add_node(IndexType frame, IndexType label, IndexType index)
{

	GraphCutNode *new_space = allocator_.allocate<GraphCutNode>();
	GraphCutNode *new_node = new(new_space) GraphCutNode(frame,label,index,cur_graph_index_++);
	node_vec.push_back(new_node);
	node_map[frame_index_to_key(frame,index)] = new_node;
}

void GraphNodeCtr::add_corresponding_relation( IndexType frame, IndexType index, IndexType cor_frame, IndexType cor_idx )
{
	node_map[frame_index_to_key(frame,index)]->cor_frame_index.insert(make_pair(cor_frame,cor_idx));
}

void GraphNodeCtr::read_label_file(char *filename)
{
	FILE *in_file = fopen(filename,"r");
	if (in_file==NULL)
	{
		return;
	}


	/// original sample labels
	IndexType num = 0;
	while (true)
	{
		int frame,label,index;
		int status = fscanf(in_file,"%d %d %d\n",&frame,&label,&index);
		if(status==EOF)break;
		add_node(frame, label, index);
		label_bucket[frame_label_to_key(frame,label)].insert(index);

		num++;
	}


	///smooth labels .txt
	//while (true)
	//{
	//	int frame,label,label_s,index;
	//	int status = fscanf(in_file,"%d %d %d %d\n",&frame,&label,&label_s, &index);
	//	if(status==EOF)break;
	//	add_node(frame, label_s, index);
	//	label_bucket[frame_label_to_key(frame,label)].insert(index);
	//}
}

IndexType GraphNodeCtr::readnLabelFile(char *filename)
{
	FILE *in_file = fopen(filename,"r");
	if (in_file==NULL)
	{
		return 0;
	}

	IndexType nLabels = 0;
    fscanf(in_file,"%d\n",&nLabels);

	/// original sample labels
	IndexType num = 0;
	while (true)
	{
		int frame,label,index;
		int status = fscanf(in_file,"%d %d %d\n",&frame,&label,&index);
		if(status==EOF)break;
		add_node(frame, label, index);
		label_bucket[frame_label_to_key(frame,label)].insert(index);

		num++;
	}

	return nLabels;
}

void GraphNodeCtr::read_smooth_label_file(char *filename)
{
	FILE *in_file = fopen(filename,"r");
	if (in_file==NULL)
	{
		return;
	}

	while (true)
	{
		int frame,label,slabel,index;
		int status = fscanf(in_file,"%d %d %d %d\n",&frame,&label,&slabel, &index);
		if(status==EOF)break;
		add_node(frame, slabel, index);
		//add_node(frame, label, index);
		label_bucket[frame_label_to_key(frame,label)].insert(index);
	}   
}
void GraphNodeCtr::read_corres_file(char *filename)
{
	FILE *in_file = fopen(filename,"r");
	if (in_file==NULL)
	{
		return;
	}

	while (true)
	{
		int frame,index,cor_frame,cor_index;
		int status = fscanf(in_file,"%d %d %d %d\n",&frame,&index,&cor_frame,&cor_index);
		if(status==EOF)break;
		add_corresponding_relation(frame,index,cor_frame,cor_index);
	}
}
ScalarType GraphNodeCtr::dist_inside_frame(GraphCutNode* s_node,GraphCutNode* e_node)
{
	if (s_node->frame != e_node->frame || s_node->label != e_node->label)
	{
		return 1e8;
	}
	Sample &smp = SampleSet::get_instance()[s_node->frame];
	const IndexType k = 100;
	IndexType neighbours[k];
	ScalarType dist[k];
	bool is_neighbour=false;
	smp.neighbours(s_node->index, k, neighbours, dist);
	for ( int i=0; i<k; i++ )
	{
		if(neighbours[i]==e_node->index)
		{
			is_neighbour = true;
			break;
		}
	}
	if (is_neighbour==false)
	{
		return 1e8;
	}
	
	{
		ScalarType diag = 1.0;// diagonal of the box(Index by frame and label)
		PointType start,end;//index
		start = m_smpSet[s_node->frame].vertices_matrix().col(s_node->index);
		end   = m_smpSet[e_node->frame].vertices_matrix().col(e_node->index);

		map<IndexType,IndexType>::iterator start_isInValidIter,end_IsValidIter;

		PCABox* box = box_bucket[frame_label_to_key(s_node->frame,s_node->label) ];//calculate diag of box

		ScalarType dis = (start- end).norm();
		diag = box->diagLen;

		for(auto iter = s_node->cor_frame_index.begin(); iter!= s_node->cor_frame_index.end(); iter++)
		{
			start_isInValidIter = s_node->cor_frame_index.find(iter->first);
			end_IsValidIter     = e_node->cor_frame_index.find(iter->first);

			GraphCutNode *s_cor = node_map[frame_index_to_key(start_isInValidIter->first,start_isInValidIter->second)];
			GraphCutNode *e_cor = node_map[frame_index_to_key(end_IsValidIter->first,end_IsValidIter->second)];
		    ScalarType dis = (start- end).norm();

			if(s_cor->label != e_cor->label)//in different label
			{
				return  4*(dis/diag);
			}else
			{
				return  dis/diag;
			}
			
		}


	}
}
void GraphNodeCtr::pca_box_ctr()
{
	IndexType boxid = 0;
	for ( unordered_map<IndexType,set<IndexType>>::iterator iter=label_bucket.begin(); iter!=label_bucket.end();iter++ )
	{
		IndexType frame_label = iter->first;
		set<IndexType>& members = iter->second;
		IndexType frame = get_frame_from_key(frame_label);
		IndexType k=members.size();
		MatrixX3 coords(k,3);
		Sample& smp = SampleSet::get_instance()[frame];
		int j=0;
		for ( set<IndexType>::iterator m_iter=members.begin(); m_iter!=members.end(); m_iter++,j++ )
		{
			coords.row(j)<<smp[*m_iter].x(),smp[*m_iter].y(),smp[*m_iter].z();
		}
		MatrixX3 vert_mean = coords.colwise().mean();
		MatrixX3 Q(k, 3);
		for (  IndexType j=0; j<k;j++)
		{
			Q.row(j) = coords.row(j) - vert_mean;
		}

		Matrix33 sigma = Q.transpose() * Q;

		Eigen::EigenSolver<Matrix33> eigen_solver(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
		auto evec = eigen_solver.eigenvectors();
		auto eval = eigen_solver.eigenvalues();

		PCABox *new_space = allocator_.allocate<PCABox>();
		box_bucket[frame_label] = new(new_space) PCABox;

		Matrix33 evecMat;
		for(int i=0;i<3;i++)
		{
			for(int j=0;j<3;j++)
			{
				evecMat(j,i) = (evec.col(i))(j).real(); 
			}
		}

		MatrixX3 newCoor = Q * evecMat;
		Matrix23 minmax;
		minmax.setZero();
		for (int i = 0;i<3;i++)
		{
            minmax(0,i) = newCoor.col(i).minCoeff();
			minmax(1,i) = newCoor.col(i).maxCoeff();
		}
		
		PCABox *pbox = box_bucket[frame_label];
		for (int i=0;i<3;i++)
		{
			pbox->center(i) = (vert_mean.row(0))(i);
		}

		pbox->minPoint = minmax.row(0);
		pbox->maxPoint = minmax.row(1);
		PointType dis = pbox->maxPoint - pbox->minPoint;

		pbox->volume = dis(0,0) *dis(1,0)*dis(2.0);
		pbox->diagLen = sqrt(dis(0,0)*dis(0,0) + dis(1,0)*dis(1,0) + dis(2,0)*dis(2,0));
		pbox->vtxSize = k;
		//Logger<<"boxes"<<boxid++<<endl;
		//Logger<<"box volume"<<pbox->volume<<endl;
		//Logger<<"box length"<<pbox->diagLen<<endl;
		//Logger<<"size"<<members.size()<<endl;
	}
}
ScalarType GraphNodeCtr::dist_inside_frame_all(GraphCutNode* s_node,GraphCutNode* e_node)
{
	if (s_node->frame != e_node->frame || s_node->label != e_node->label)
	{
		return 1e8;
	}else
	{
		ScalarType diag = 1.0;// diagonal of the box(Index by frame and label)
		PointType start,end;//index
		start = m_smpSet[s_node->frame].vertices_matrix().col(s_node->index);
		end   = m_smpSet[e_node->frame].vertices_matrix().col(e_node->index);

		//correspodence of start and end point are in the same label?2014-11-28
		map<IndexType,IndexType>::iterator start_isInValidIter,end_IsValidIter;

		PCABox* box = box_bucket[frame_label_to_key(s_node->frame,s_node->label) ];//calculate diag of box
	    diag = box->diagLen;
		IndexType v_n = box->vtxSize;

		ScalarType dis = 0.0;
		IndexType nDiff = 0;
		IndexType nIden = 0;
		bool flagDiff = false;
		dis = (start- end).norm();
		dis/=v_n;

		for(auto iter = s_node->cor_frame_index.begin(); iter!= s_node->cor_frame_index.end(); iter++)
		{
			start_isInValidIter = s_node->cor_frame_index.find(iter->first);
			end_IsValidIter     = e_node->cor_frame_index.find(iter->first);

			GraphCutNode *s_cor = node_map[frame_index_to_key(start_isInValidIter->first,start_isInValidIter->second)];
			GraphCutNode *e_cor = node_map[frame_index_to_key(end_IsValidIter->first,end_IsValidIter->second)];

			//if (s_cor != NULL && e_cor != NULL)
			//{
			//	if(s_cor->label != e_cor->label)//in different label
			//	{
			//		nDiff ++;
			//		flagDiff = true;
			//		break;

			//	}
// 				else
// 				{
// 					nIden ++;
// 				}
			//}


			//if(s_cor->label != e_cor->label)//in different label
			//{
			//	return  4 *(dis/diag);
			//}else
			//{
			//	return  dis/diag;
			//}
		
		}
	
		//if (flagDiff ||(nDiff>nIden))
		//{
		//	return 3 *(dis/diag);
		//}

		return dis/diag;
	}
}


ScalarType GraphNodeCtr::weight2nodes_dis(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType a_weight)
{

	ScalarType adjDis = 0.0;
	ScalarType corDis = 0.0;
	ScalarType ration = 0.0;
	ScalarType fai = 0.0;

	ScalarType lamda_adj = 160;
	ScalarType lamda_diff =100;	 
	ScalarType lamda_cor = 80;


	//Logger<<"lamda adj = "<<lamda_adj<<endl;
	//Logger<<"lamda cor = "<<lamda_cor<<endl;
	//Logger<<" segment number = 5.\n ";

	//adjDis = dist_inside_frame_all(s_node,e_node);//total 
	//corDis = dist_between_frame(s_node,e_node);

	adjDis = dist_inside_frame_all_paper(s_node,e_node,ration);//total 
	corDis = dist_between_frame_paper(s_node,e_node,fai);

	if (adjDis == 1e8 && corDis == 1e8)
	{
		return 0.0;
	}

	if (adjDis != 1e8)
	{
		//if (ration > 0.)
		//{
		//	adjDis  *= lamda_adj * ( 1 + lamda_cor);
		//}else
		//{
		//    adjDis  *= lamda_adj/* * ( 1 + ration * lamda_cor)*/;
		//}
		adjDis  *= (lamda_adj * ( 1 + ration * lamda_diff));
		return exp(- adjDis*adjDis);
	}

	if (corDis != 1e8)
	{
		if (fai > 0.5)
		{
			return 0.;
		}

		corDis *= (1 + fai * lamda_cor);
		return exp(-corDis*corDis);
	}

}
ScalarType GraphNodeCtr::dist_between_frame_paper(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType& jDis)
{
	map<IndexType,IndexType>::iterator isInValidIter;
	isInValidIter = s_node->cor_frame_index.find(e_node->frame);


	if (s_node->frame == e_node->frame || isInValidIter == s_node->cor_frame_index.end()|| isInValidIter->second != e_node->index )
	{
		return 1e8;
	}else
	{
// 		Logger<<s_node->frame<<"  "<<s_node->label<<endl;
// 		Logger<<e_node->frame<<"  "<<e_node->label<<endl;

		ScalarType corDis = minCorrespondenceDis(s_node,e_node);//using min deformable of  the edge

		//Logger<<"source point distor = "<<corDis<<endl;

		jDis  = max(calculateJa_dis(s_node,e_node),calculateJa_dis(e_node,s_node));

		//Logger<<"set dis = "<<jDis<<endl;

		return corDis;
	}
}

ScalarType GraphNodeCtr::dist_inside_frame_all_paper(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType& ration)
{
	if (s_node->frame != e_node->frame || s_node->label != e_node->label)
	{
		return 1e8;
	}else
	{
		ScalarType diag = 1.0;// diagonal of the box(Index by frame and label)
		PointType start,end;//index
		start = m_smpSet[s_node->frame].vertices_matrix().col(s_node->index);
		end   = m_smpSet[e_node->frame].vertices_matrix().col(e_node->index);

		//correspodence of start and end point are in the same label?2014-11-28
		map<IndexType,IndexType>::iterator start_isInValidIter,end_IsValidIter;

		PCABox* box = box_bucket[frame_label_to_key(s_node->frame,s_node->label) ];//calculate diag of box
		diag = box->diagLen;
		IndexType v_n = box->vtxSize;

		ScalarType dis = 0.0;
		IndexType nIden = 0;
		bool flagDiff = false;
		dis = (start- end).norm();


		for(auto iter = s_node->cor_frame_index.begin(); iter!= s_node->cor_frame_index.end(); iter++)
		{
			start_isInValidIter = s_node->cor_frame_index.find(iter->first);
			end_IsValidIter     = e_node->cor_frame_index.find(iter->first);

			GraphCutNode *s_cor = node_map[frame_index_to_key(start_isInValidIter->first,start_isInValidIter->second)];
			GraphCutNode *e_cor = node_map[frame_index_to_key(end_IsValidIter->first,end_IsValidIter->second)];

			if (s_cor != NULL && e_cor != NULL)
			{
				nIden ++;
 				//if(s_cor->label != e_cor->label)//in different label
 				//{
 				//	ration ++;
 				//}


				//using Jac distance
				IndexType rowId = boxId[frame_label_to_key(s_node->frame,s_node->label)];
				IndexType colId = boxId[frame_label_to_key(e_node->frame,e_node->label)];
				ScalarType boxCor = m_boxDis(rowId,colId);

				if (boxCor > 0.9)
				{
					ration ++;
				}
			}

		}

		ration /= nIden;

		//return /*v_n * */dis/diag;
// 
		assert(m_totNode >0&& diag > 0);
		return (dis * v_n)/(diag* m_totNode);
	}
}

ScalarType GraphNodeCtr::dist_between_frame(GraphCutNode* s_node,GraphCutNode* e_node)
{
	map<IndexType,IndexType>::iterator isInValidIter;
	isInValidIter = s_node->cor_frame_index.find(e_node->frame);


	if (s_node->frame == e_node->frame || isInValidIter == s_node->cor_frame_index.end()|| isInValidIter->second != e_node->index )
	{
		return 1e8;
	}else
	{
		ScalarType corDis = minCorrespondenceDis(s_node,e_node);//using min deformable of  the edge


		//ScalarType boxRation = minVoxelRation(s_node,e_node); 
		
		//if (corDis > 0.6)
		//{
		//	return 1e8;
		//}

// 		if(boxRation < 0.8)//
// 		{ 
			//return 0.0;
// 			return  corDis;
// 		}else
// 		{
//            return 5 * corDis;
// 		}
		
	}

}

ScalarType GraphNodeCtr::weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node)
{
	ScalarType adjDis = 0.0;
	ScalarType wg_a = 0.01;
	ScalarType var_a = 0.3;//median of  the distances

	ScalarType corDis = 0.0;
	ScalarType wg_c = 0.99;
	ScalarType var_c = 0.25;

	ScalarType adjWeight = 1.0;
	ScalarType corWeight = 1.0;

	adjDis = dist_inside_frame(s_node,e_node);
	//adjDis = dist_inside_frame_all(s_node,e_node);
	corDis = dist_between_frame(s_node,e_node);

	if (adjDis > 1e5)
	{
		adjWeight = 0.0;
	}else
	{
		adjWeight = exp(- adjDis *adjDis/var_a);
	}
	
	if(corDis > 1e5)
	{
		corWeight = 0.0;
	}
	else
	{
		corWeight = exp(- corDis *corDis/var_c);
		//Logger<<"cor dis"<<corDis<<endl;
	}

	return wg_a * adjWeight + wg_c * corWeight;  
}
ScalarType GraphNodeCtr::weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType a_weight)
{
	ScalarType adjDis = 0.0;
	//ScalarType var_a = 0.0182201;//median of  the distances
	//ScalarType var_a = 0.0369636;//median of  the distances

	ScalarType var_a = 0.02982;//median of  the distances  _horse(0-9)
	ScalarType avg_a = 0.02012;//median of  the distances  _horse(0-9)

	//ScalarType var_a = 0.006976;//median of  the distances  _horse(0-19)
	//ScalarType avg_a = 0.0319281;//median of  the distances  _horse(0-19)

	ScalarType corDis = 0.0;
	//ScalarType var_c = 0.0901867;//avg
	//ScalarType var_c = 0.0663885;//median

	ScalarType var_c = 0.0288677;//median---horse(0-9)
	ScalarType avg_c = 0.0151138;//median---horse(0-9)

	//ScalarType var_c = 0.08080804;//median---horse(0-19)
	//ScalarType avg_c = 0.0155155;//median---horse(0-19)

	ScalarType adjWeight = 1.0;
	ScalarType corWeight = 1.0;

	//adjDis = dist_inside_frame(s_node,e_node);//k-neighboor
	adjDis = dist_inside_frame_all(s_node,e_node);//total 
	corDis = dist_between_frame(s_node,e_node);

	if (adjDis > 1e5)
	{
		adjWeight = 0.0;
	}else
	{
		adjWeight = exp(- (adjDis-avg_a) *(adjDis-avg_a)/(2*var_a) );//minus avg
		//Logger<<"a distance = "<<adjDis<<endl;
	}
	
	if(corDis > 1e5)
	{
		corWeight = 0.0;
	}
	else
	{
		corWeight = exp(- (corDis -avg_c) *(corDis-avg_c)/(2*var_c));
	}

	return /*a_weight **/ adjWeight + /*(1 - a_weight) **/ 300 * corWeight;  
}


ScalarType GraphNodeCtr::weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node,ScalarType& adjDis,ScalarType& corDis)
{
	//ScalarType adjDis = 0.0;
	ScalarType wg_a = 0.01;
	ScalarType var_a = 0.3;//median of  the distances

	//ScalarType corDis = 0.0;
	ScalarType wg_c = 0.99;
	ScalarType var_c = 0.3;

	ScalarType adjWeight = 1.0;
	ScalarType corWeight = 1.0;

	adjDis = dist_inside_frame(s_node,e_node);
	corDis = dist_between_frame(s_node,e_node);


	if (adjDis > 1e5)
	{
		adjWeight = 0.0;
	}else
	{
		adjWeight = exp(- adjDis *adjDis/(2*var_a));
	}
	
	if(corDis > 1e5)
	{
		corWeight = 0.0;
	}
	else
	{
		corWeight = exp(- corDis *corDis/(2*var_c));
	}
	return wg_a * adjWeight + wg_c * corWeight;
}
ScalarType GraphNodeCtr::measureDeformableCorVer(IndexType sFrame,IndexType sId,IndexType tFrame,IndexType tId)
{
	IndexType * neigSrGraph = new IndexType[m_neigNum];
	IndexType * neigTgGraph = new IndexType[m_neigNum];
	Matrix3X neigSrCoor(3,m_neigNum);
	Matrix3X neigTgCoor(3,m_neigNum);

	VecX neigDis;     //record ori distance
	VecX resNeigDis;  // record target distances 
	neigDis.setZero(m_neigNum,1);
	resNeigDis.setZero(m_neigNum,1);

	neigSrCoor.setZero();
	neigTgCoor.setZero();

	m_smpSet[sFrame].neighbours(sId,m_neigNum,neigSrGraph);
	m_smpSet[tFrame].neighbours(tId,m_neigNum,neigTgGraph);

	getConstantNeigCoorByIndex(m_smpSet[sFrame],neigSrGraph,neigSrCoor);
	getConstantNeigCoorByIndex(m_smpSet[tFrame],neigTgGraph,neigTgCoor);

	calculateNeigDis(neigSrCoor,neigDis);
	calculateNeigDis(neigTgCoor,resNeigDis);

	return deformableValue(neigDis,resNeigDis);

}
void GraphNodeCtr::getConstantNeigCoorByIndex(Sample & smp,IndexType * neigIndex,Matrix3X & res_coor)
{
	for (IndexType v_it = 0; v_it < m_neigNum;v_it++)
	{
		res_coor.col(v_it) = smp.vertices_matrix().col(neigIndex[v_it]);
	}
}
void GraphNodeCtr::calculateNeigDis(Matrix3X & neigCoor,VecX& resDis)
{
	IndexType vtx_num = neigCoor.cols();
	PointType ori = neigCoor.col(0);
	PointType diff;
	diff.setZero();
	for (IndexType vtx_iter = 1; vtx_iter < vtx_num;vtx_iter ++)
	{
		diff = neigCoor.col(vtx_iter) - ori;
		resDis(vtx_iter,0) = diff.norm();
	}
}
ScalarType GraphNodeCtr::deformableValue(VecX& srNeigDis,VecX & resNeigDis)
 {
 	ScalarType totle = 0.0;
 	ScalarType molecule = 0.0;
 	ScalarType denominator = 1.0;
 	IndexType neigNum = resNeigDis.rows();
 	for (IndexType v_iter  = 1;v_iter < neigNum; v_iter++)
 	{
 		denominator = srNeigDis(v_iter,0);
 		molecule = abs(resNeigDis(v_iter,0) - denominator);
 		assert(denominator > 1e-7);
 		totle += molecule/denominator;
 	}
 	return totle/(neigNum - 1);
 }
ScalarType GraphNodeCtr::minCorrespondenceDis(GraphCutNode* s_node,GraphCutNode* e_node)
{
	map<IndexType,IndexType>::iterator isInValidIter;
	ScalarType toDeformable = measureDeformableCorVer(s_node->frame,s_node->index,e_node->frame,e_node->index);
	isInValidIter = e_node->cor_frame_index.find(s_node->frame);

	if(isInValidIter == e_node->cor_frame_index.end())
	{
		return toDeformable;
		Logger<<"can't find  inverse correspondence Error!.\n";
	}

	ScalarType fromDeformable = measureDeformableCorVer(e_node->frame,e_node->index,s_node->frame,isInValidIter->second);
		
	return min(toDeformable,fromDeformable);
}
ScalarType GraphNodeCtr::minVoxelRation(GraphCutNode* s_node,GraphCutNode* e_node)
{
	PCABox* sBox = box_bucket[frame_label_to_key(s_node->frame,s_node->label) ];
	PCABox* tBox = box_bucket[frame_label_to_key(e_node->frame,e_node->label) ];

	ScalarType sBoxVoxel = sBox->volume;
	ScalarType tBoxVoxel = tBox->volume;
	
	return min(sBoxVoxel/tBoxVoxel,tBoxVoxel/sBoxVoxel);
}
void GraphNodeCtr::compute_mid_and_avg(ScalarType &mid, ScalarType &avg, pDistFunc dist_func)
{
	const ScalarType DEF_INF = 1e8;
	int n = node_vec.size();
	vector<ScalarType> dist_vec;
	ScalarType sum=0.f;
	for (int i=0;i<n;i++)
	{
		for(int j=i+1;j<n;j++)
		{
			ScalarType dist = (this->*dist_func)(node_vec[i],node_vec[j]);
			if (dist==DEF_INF)
			{
				continue;
			}
			dist_vec.push_back(dist);
			sum += dist;
		}
	}
	sort(dist_vec.begin(),dist_vec.end());
	mid = dist_vec[dist_vec.size()/2];
	avg = sum/dist_vec.size();
}

//////////////////////////////////////////////////////////////////////////
ScalarType GraphNodeCtr::weight2nodes_smooth(GraphCutNode* s_node,GraphCutNode* e_node)
{
	if (s_node->index == e_node->index)
	{
		return 0.0;
	}

	ScalarType dis = 0.0;

	Sample & smp = m_smpSet[s_node->frame];

	const IndexType k = 100;
	IndexType neighbours[k];
	ScalarType dist[k];

	smp.neighbours(s_node->index,k,neighbours,dist);
	
	set<IndexType> neigId;
	set<IndexType>::iterator IsValidIter;

	for (IndexType neig_N = 0; neig_N < k;neig_N ++)
	{
		neigId.insert(neighbours[neig_N]);
	}

	IsValidIter = neigId.find(e_node->index);

	if (IsValidIter == neigId.end())
	{
		return 0.0;
	}

	PointType start,end;//index
	start = smp.vertices_matrix().col(s_node->index);
	end   = smp.vertices_matrix().col(e_node->index);

	ScalarType diag = 0.0;
	PCABox* box = box_bucket[frame_label_to_key(s_node->frame,s_node->label) ];//calculate diag of box
	diag = box->diagLen;

	if ( s_node->label != e_node->label)
	{
		dis = 50 * (start - end).norm()/diag;
	}
	else
	{
		//dis = (start - end).norm()/diag;
		dis = 0.;
	}

	return exp(-dis*dis);
}

ScalarType GraphNodeCtr::weight2nodes_smooth_Sy(GraphCutNode* s_node,GraphCutNode* e_node)
{
	if (s_node->index == e_node->index)
	{
		return 0.0;
	}

	ScalarType dis = 0.0;
	Sample & smp = m_smpSet[s_node->frame];

	const IndexType k = 100;
	IndexType neighbours[k];
	ScalarType dist[k];

	smp.neighbours(s_node->index,k,neighbours,dist);

	set<IndexType> neigId;
	set<IndexType>::iterator IsValidIter;

	for (IndexType neig_N = 0; neig_N < k;neig_N ++)
	{
		neigId.insert(neighbours[neig_N]);
	}

	IsValidIter = neigId.find(e_node->index);

	if (IsValidIter == neigId.end())
	{
		return 0.0;
	}

}

////// paper method////
ScalarType GraphNodeCtr::calculateJa_dis(GraphCutNode* s_node,GraphCutNode* e_node)
{
	IndexType rowId = boxId[frame_label_to_key(s_node->frame,s_node->label)];
	IndexType colId = boxId[frame_label_to_key(e_node->frame,e_node->label)];
	

	return m_boxDis(rowId,colId);


	////find elements which  have the same label with s_node
	//set<IndexType>& members = label_bucket[frame_label_to_key(s_node->frame,s_node->label)]; //find A
	//set<IndexType> corPointId;

	//map<IndexType,IndexType>::iterator isInValidIter;

	//for (set<IndexType>::iterator m_iter=members.begin(); m_iter!=members.end(); m_iter++)//find  A'
	//{
	//	GraphCutNode* sPoint = node_map[frame_index_to_key(s_node->frame,*m_iter)];

	//	isInValidIter = sPoint->cor_frame_index.find(e_node->frame);
	//	if (isInValidIter!= sPoint->cor_frame_index.end())
	//	{
	//		GraphCutNode* tPoint = node_map[frame_index_to_key(e_node->frame,isInValidIter->second)];
	//		
	//		if ( tPoint->label == e_node->label)
	//		{
	//			corPointId.insert(tPoint->index);
	//		}
	//	}

	//}

	//// find B 
	//set<IndexType>& b_menbers = label_bucket[frame_label_to_key(e_node->frame,e_node->label)];

	////calculate Jac dis of A' and B
	////set<IndexType>::iterator first1 = corPointId.begin();
	////set<IndexType>::iterator last1 = corPointId.end();

	////set<IndexType>::iterator first2 = b_menbers.begin();
	////set<IndexType>::iterator last2 = b_menbers.end();

	//ScalarType inter_num = 0;
	//ScalarType union_num = 0;

	////while (first1!=last1 && first2!=last2)
	////{
	////	if ( *first1 < *first2 )++first1;
	////	else if ( *first1 > *first2 )++first2;
	////	else
	////	{
	////		inter_num ++;
	////		++first1;++first2;
	////	}
	////}

	//union_num =  (members.size() - corPointId.size() ) + b_menbers.size();


	//return  (union_num - corPointId.size())/union_num;
}
//
void GraphNodeCtr::propagateLabel2Orignal(Sample& oriPC,vector<IndexType>& sampleVtxId,vector<IndexType>& label_smp,vector<IndexType>& label_ori)
{
	IndexType nCluster = -1;
	for (int i = 0; i < label_smp.size(); i++)
	{
		if(nCluster < label_smp[i])
		{
			nCluster = label_smp[i];
		}
	}
	Logger<<"The number of segments are "<<nCluster + 1<<endl;

	map<IndexType,IndexType> smpLabel;
	map<IndexType,IndexType>::iterator IsValidIter;
	for (int i = 0; i < label_smp.size(); i++)
	{
		smpLabel.insert(make_pair(sampleVtxId[i],label_smp[i]));
	}

	const IndexType k = 300;
	IndexType neighbours[k];
	ScalarType dist[k];

	IndexType vtx_num = oriPC.num_vertices();
	IndexType result_label;


	for(IndexType vtx_id = 0; vtx_id < vtx_num; vtx_id ++)
	{
		vector<IndexType> recordLabelTime(nCluster + 1,0);
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
		for (int i = 0; i <= nCluster; i++)
		{
			if(result_label < recordLabelTime[i])
			{
				label_ori[vtx_id] = i;
				result_label = recordLabelTime[i];
			}
		}

	}
	Logger<<"k-neig's k = "<<k<<endl;
	Logger<<"End Propagate!"<<endl;
}
void GraphNodeCtr::constructBoxDis(unordered_map<IndexType, set<IndexType>>& label_set)
{
	IndexType box_num = label_set.size();
	m_boxDis.setZero(box_num,box_num);

	map<IndexType,IndexType> inverIndex;
	IndexType boxKey;
	IndexType i = 0;
	for (auto it = label_set.begin(); it != label_set.end();it ++,i++)
	{
		boxKey = it->first;

// 		Logger<<"frame = "<<get_frame_from_key(boxKey)<<endl;
// 		Logger<<"label = "<<get_label_from_key(boxKey)<<endl;

		boxId[boxKey] = i; 
		inverIndex[i] = boxKey;
	}



	for (int i = 0; i < box_num; i++)
	{
		for (int j = 0; j < box_num; j++)
		{
			if (i == j) 
			{
               m_boxDis(i,j) = 0;
			   continue;
			}
			IndexType rowKey = inverIndex[i];
			IndexType colKey = inverIndex[j];

			m_boxDis(i,j) = calculateJa_dis(rowKey,colKey);
			//Logger<<i<<"  "<<j<<m_boxDis(i,j)<<endl;
		}

	}

// 	char label_labsmooth[256];
// 	sprintf(label_labsmooth,"test_hands_0_1_distance.txt");
// 	FILE *in_label_smmoth = fopen(label_labsmooth, "w");
// 	for (IndexType i =0; i< box_num;i++)
// 	{
// 		for (IndexType j = 0;j< box_num;j++)
// 		{
// 			fprintf(in_label_smmoth,"%f  ",m_boxDis(i,j));
// 		}
// 		fprintf(in_label_smmoth,"\n");
// 	}
// 	fclose(in_label_smmoth);
}

ScalarType GraphNodeCtr::calculateJa_dis(IndexType rowKey,IndexType colKey)
{
	set<IndexType>& members = label_bucket[rowKey]; //find A
	set<IndexType> corPointId;

	map<IndexType,IndexType>::iterator isInValidIter;

	for (set<IndexType>::iterator m_iter=members.begin(); m_iter!=members.end(); m_iter++)//find  A'
	{
		GraphCutNode* sPoint = node_map[frame_index_to_key(get_frame_from_key(rowKey),*m_iter)];

		isInValidIter = sPoint->cor_frame_index.find(get_frame_from_key(colKey));
		if (isInValidIter!= sPoint->cor_frame_index.end())
		{
			GraphCutNode* tPoint = node_map[frame_index_to_key(get_frame_from_key(colKey),isInValidIter->second)];

			if ( tPoint->label == get_label_from_key(colKey))
			{
				corPointId.insert(tPoint->index);
			}
		}

	}

	// find B 
	set<IndexType>& b_menbers = label_bucket[colKey];

	ScalarType inter_num = 0;
	ScalarType union_num = 0;

	union_num =  (members.size() - corPointId.size() ) + b_menbers.size();

	return  (union_num - corPointId.size())/union_num;
}
void GraphNodeCtr::diff_using_bfs( std:: vector<IndexType>& labels,std::vector<IndexType>& vtxId,IndexType centerFrame )
{
	SampleSet& set = SampleSet::get_instance();
	IndexType max_label= *max_element(labels.begin(),labels.end());
	vector< std::set<IndexType> > label_bucket(max_label+1);

	for ( int i=0; i<labels.size(); i++ )
	{
		label_bucket[labels[i]].insert( i );
	}
	IndexType new_label = max_label;
	Logger<<"max label before post:"<<new_label<<endl;
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
			IndexType a = vtxId[*iter];
			vtx_data(0,i)  = set[centerFrame][vtxId[*iter]].x();
			vtx_data(1,i)  = set[centerFrame][vtxId[*iter]].y();
			vtx_data(2,i)  = set[centerFrame][vtxId[*iter]].z();
		}


#ifdef USE_RADIUS_NEAREST
		ScalarType rad =  set[centerFrame].box_diag()/2;
		BFSClassifier<ScalarType> classifier(vtx_data,rad);
#else
		ScalarType rad = set[centerFrame].box_diag()/10;
		BFSClassifier<ScalarType> classifier(vtx_data,rad,20);
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
	Logger<<"max label after post:"<<new_label<<endl;

}

void GraphNodeCtr::calculateNorm()
{
	Logger<<"Start calculate normal .\n";

	NormalType viewpoint(0,0,-1.);//kinect device
	//NormalType viewpoint(-1,0,0);//horse

	for (IndexType frameId = 0; frameId<m_smpSet.size(); frameId++)
	{
		SampleManipulation::compute_normal(frameId,viewpoint);
	}
	Logger<<"End calculate normal.\n";
}

GraphNodeCtr::GraphNodeCtr() :cur_graph_index_(0),allocator_(),m_smpSet(SampleSet::get_instance())
{
	m_neigNum = 50;
}
