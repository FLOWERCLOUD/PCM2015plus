#include "graph_cut_node.h"
#include <stdio.h>

void GraphNodeCtr::run()
{
	Logger<<"Start!.\n";

	read_label_file("labelInfo.txt");
	read_corres_file("corInfo.txt");

	pca_box_ctr();

	//dist_inside_frame(node_map[frame_index_to_key(4,23803)],node_map[frame_index_to_key(5,23983)]);
	//dist_between_frame(node_map[frame_index_to_key(4,1263)],node_map[frame_index_to_key(5,1241)]);
	//dist_between_frame(node_map[frame_index_to_key(4,2165)],node_map[frame_index_to_key(5,2735)]);
	//dist_between_frame(node_map[frame_index_to_key(4,7)],node_map[frame_index_to_key(5,30)]);
	//dist_between_frame(node_map[frame_index_to_key(4,2653)],node_map[frame_index_to_key(5,2452)]);
	dist_between_frame(node_map[frame_index_to_key(4,23803)],node_map[frame_index_to_key(5,23983)]);
}
void GraphNodeCtr::add_node(IndexType frame, IndexType label, IndexType index)
{

	GraphCutNode *new_space = allocator_.allocate<GraphCutNode>();
	node_map[frame_index_to_key(frame,index)] = new(new_space) GraphCutNode(frame,label,index,cur_graph_index_++);
}

void GraphNodeCtr::add_corresponding_relation( IndexType frame, IndexType index, IndexType cor_frame, IndexType cor_idx )
{
	//node_map[frame_index_to_key(frame,index)]->frame_index_of_corresponding_node_.push_back( frame_index_to_key(cor_frame,cor_idx) );
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

void GraphNodeCtr::pca_box_ctr()
{
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
		
	}
}
ScalarType GraphNodeCtr::dist_inside_frame(GraphCutNode* s_node,GraphCutNode* e_node)
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

		PCABox* box = box_bucket[frame_label_to_key(s_node->frame,s_node->label) ];//calculate diag of box

		PointType bbxScale = box->maxPoint - box->minPoint;
		 
		diag = sqrt(bbxScale(0,0)*bbxScale(0,0) + bbxScale(1,0)*bbxScale(1,0) + bbxScale(2,0)*bbxScale(2,0));
		ScalarType dis = (start- end).norm();
		
		return (1 - dis/(diag))*(1 - dis/(diag));  
	}
}

ScalarType GraphNodeCtr::dist_between_frame(GraphCutNode* s_node,GraphCutNode* e_node)
{
	map<IndexType,IndexType>::iterator isInValidIter;
	isInValidIter = s_node->cor_frame_index.find(e_node->frame);

	if (s_node->frame == e_node->frame || isInValidIter == s_node->cor_frame_index.end() )
	{
		return 1e8;
	}else
	{
		ScalarType corDis = minCorrespondenceDis(s_node,e_node);//using min deformable of  the edge
		ScalarType boxRation = minVoxelRation(s_node,e_node); 
		
		return boxRation * boxRation * corDis;
	}

}
ScalarType GraphNodeCtr::weight2nodes(GraphCutNode* s_node,GraphCutNode* e_node)
{
	ScalarType adjDis = 0.0;
	ScalarType wg_a = 1.0;
	ScalarType var_a = 1.0;//median of  the distances

	ScalarType corDis = 0.0;
	ScalarType wg_c = 1.0;
	ScalarType var_c = 1.0;

	adjDis = dist_inside_frame(s_node,e_node);
	corDis = dist_between_frame(s_node,e_node);

	return wg_a * exp(- adjDis *adjDis/var_a) + wg_c * exp(- corDis * corDis/ var_c);  
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

	PointType bbxS = sBox->maxPoint - sBox->minPoint;
	PointType bbxT = tBox->maxPoint - sBox->minPoint;

	ScalarType sBoxVoxel =   (bbxS(0,0) * bbxS(1,0) * bbxS(2,0) );
	ScalarType tBoxVoxel =   (bbxT(0,0) * bbxT(1,0) * bbxT(2,0) );
	
	return min(sBoxVoxel/tBoxVoxel,tBoxVoxel/sBoxVoxel);
}
