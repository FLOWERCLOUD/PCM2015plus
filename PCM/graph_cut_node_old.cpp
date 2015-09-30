#include "graph_cut_node.h"
#include <stdio.h>
#include <engine.h>
#ifdef WIN32
#include <direct.h>
#define get_current_dir _getcwd
#else
#include <unistd.h>
#define get_current_dir getcwd
#endif
#include <cstring>
#include"sample_set.h"

void GraphNodeCtr::run()
{
	Logger<<"Start!.\n";

	IndexType nbCluster = 5;

	//read_label_file("labelInfo.txt");
	//read_label_file("labelInfo_edit.txt");
	//read_label_file("labelInfo_28.txt");
	//read_corres_file("corInfo.txt");

	//read_label_file("labelInfo_456.txt");
	//read_corres_file("corInfo_456.txt");


	read_label_file("tot_labels_dancer.txt");//dancer girl_f(91-110)

	read_corres_file("tot_cor_dancer.txt");

	pca_box_ctr();


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
	  
	IndexType n = cur_graph_index_;
	mxArray *mx_distMat = NULL;
	numeric::float64* dm_buffer;
	dm_buffer = new numeric::float64[n*n];
	mx_distMat = mxCreateDoubleMatrix(n,n,mxREAL);
	
	ScalarType a_w = 0.01f;

	//ScalarType avg_a_dis ,a_mid;
	//compute_mid_and_avg(avg_a_dis,a_mid,&GraphNodeCtr::dist_inside_frame);
	//Logger<<"avg and mid of adjacency"<<avg_a_dis<<" "<<a_mid<<endl;
	//ScalarType avg_cor_dis,cor_dis;
	//compute_mid_and_avg(avg_cor_dis,cor_dis,&GraphNodeCtr::dist_between_frame);
	//Logger<<"avg and mid of corresponcdence"<<avg_cor_dis<<" "<<cor_dis<<endl;

	for (int i=0; i<n;i++)
	{
		//dm_buffer[i*(n+1)] = exp(-a_w * a_w);
		dm_buffer[i*(n+1)] = a_w;
		for (int j=i+1; j<n; j++)
		{
			//ScalarType dist = weight2nodes(node_vec[i],node_vec[j]);
			ScalarType dist = weight2nodes(node_vec[i],node_vec[j],a_w);
			dm_buffer[i*n+j] = dm_buffer[j*n+i] = (numeric::float64)dist;

		}

	}


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

	

	memcpy((char*)mxGetPr(mx_distMat),(char*)dm_buffer,n*n*sizeof(numeric::float64));
	delete [] dm_buffer;
	engPutVariable(ep,"W",mx_distMat);

	char cmd_buf[128];
	sprintf(cmd_buf,"[NcutDiscrete,NcutEigenvectors,NcutEigenvalues] = ncutW(W,%d);",nbCluster);
	engEvalString(ep,cmd_buf);

	//Display output information
	Logger<<result_buffer<<std::endl;

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
	//Visualize
	SampleSet &smp_set = SampleSet::get_instance();
	IndexType frames[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	for ( IndexType i=0; i<3;i++ )
	{
		for (IndexType j=0; j<smp_set[frames[i]].num_vertices(); j++)
		{
			smp_set[frames[i]][j].set_visble(false);
		}
	}
	for ( IndexType i=0; i<n;i++ )
	{
		GraphCutNode &node = *(node_vec[i]);
		smp_set[node.frame][node.index].set_label(node.graph_label);
		smp_set[node.frame][node.index].set_visble(true);
	}

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

	while (true)
	{
		int frame,label,index;
		int status = fscanf(in_file,"%d %d %d\n",&frame,&label,&index);
		if(status==EOF)break;
		add_node(frame, label, index);
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
		IndexType labelsize = box->vtxSize;
		assert(labelsize>0);

		for(auto iter = s_node->cor_frame_index.begin(); iter!= s_node->cor_frame_index.end(); iter++)
		{
			start_isInValidIter = s_node->cor_frame_index.find(iter->first);
			end_IsValidIter     = e_node->cor_frame_index.find(iter->first);

			GraphCutNode *s_cor = node_map[frame_index_to_key(start_isInValidIter->first,start_isInValidIter->second)];
			GraphCutNode *e_cor = node_map[frame_index_to_key(end_IsValidIter->first,end_IsValidIter->second)];
		    ScalarType dis = (start- end).norm();

			if(s_cor->label != e_cor->label)//in different label
			{
				return  8*(dis/(diag*labelsize));
			}else
			{
				return  dis/(diag * labelsize);
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
	}else//identity frame and label
	{
		ScalarType diag = 1.0;// diagonal of the box(Index by frame and label)
		PointType start,end;//index
		start = m_smpSet[s_node->frame].vertices_matrix().col(s_node->index);
		end   = m_smpSet[e_node->frame].vertices_matrix().col(e_node->index);

		//correspodence of start and end point are in the same label?2014-11-28
		map<IndexType,IndexType>::iterator start_isInValidIter,end_IsValidIter;

		PCABox* box = box_bucket[frame_label_to_key(s_node->frame,s_node->label) ];//calculate diag of box
	    diag = box->diagLen;

		ScalarType dis = 0.0;
		bool flagDiff = false;

		IndexType labelsize = box->vtxSize;
		assert(labelsize>0);

		IndexType nDiff = 0;
		IndexType nIden = 0;
		for(auto iter = s_node->cor_frame_index.begin(); iter!= s_node->cor_frame_index.end(); iter++)
		{
			start_isInValidIter = s_node->cor_frame_index.find(iter->first);
			end_IsValidIter     = e_node->cor_frame_index.find(iter->first);

			GraphCutNode *s_cor = node_map[frame_index_to_key(start_isInValidIter->first,start_isInValidIter->second)];
			GraphCutNode *e_cor = node_map[frame_index_to_key(end_IsValidIter->first,end_IsValidIter->second)];
		    dis = (start- end).norm();

			if(s_cor->label != e_cor->label)//in different label
			{
				//return 3 *(dis/diag);
				nDiff ++;
				//flagDiff = true;
			}else
			{
				//return dis/(diag);
				nIden ++;
			}
			
		}
	    
		if(flagDiff || (nDiff > nIden))
		{
			return 3 *(dis/diag);
		}

		return dis/(diag);


		///return 4 *(dis/(diag*labelsize));
		//return dis/(diag*labelsize);

	}
}


ScalarType GraphNodeCtr::dist_between_frame(GraphCutNode* s_node,GraphCutNode* e_node)
{
	map<IndexType,IndexType>::iterator isInValidIter;
	isInValidIter = s_node->cor_frame_index.find(e_node->frame);

	if (s_node->frame == e_node->frame || isInValidIter->second != e_node->index )
	{
		return 1e8;
	}else
	{
		ScalarType corDis = minCorrespondenceDis(s_node,e_node);//using min deformable of  the edge
		ScalarType boxRation = minVoxelRation(s_node,e_node); 
		
		if(corDis > 0.5)//增加一条-如果变形量超过了一定量，那么这个对应就没有意义。
		{
			return 1e8;
		}

		if(boxRation < 0.8)//推断出点对应属于不同的类,减小它们之间的距离
		{
			return 0.0;
			//return 1e-2* corDis;
			//return corDis * corDis;
		}else
		{
           return boxRation * boxRation * corDis;
		   //return 1e2 * corDis;
			//return corDis * corDis * corDis;
		}
		
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
	//ScalarType var_a = 0.0182201;//median of  the distances-3frames(0.020051)
	ScalarType var_a = 0.0369636;//0.0356051;//median of  the distances-3frames()

	ScalarType corDis = 0.0;
	//ScalarType var_c = 0.0901867;//avg-3frames(0.0363027)
	ScalarType var_c = 0.0663885;//2-frames--0.106298;//median-3frames(0.0561515)


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
		adjWeight = exp(- adjDis *adjDis/(2*var_a));
		//Logger<<"a distance = "<<adjDis<<endl;
	}
	
	if(corDis > 1e5)
	{
		corWeight = 0.0;
	}
	else
	{
		corWeight = exp(- corDis *corDis/(2*var_c));
	}

	return a_weight * adjWeight + (1 - a_weight) * corWeight;  
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