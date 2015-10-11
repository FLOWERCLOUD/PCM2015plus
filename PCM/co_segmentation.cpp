#include "co_segmentation.h"
#include <assert.h>
#define  INF 1000000

#define frame_index_to_key(f,i) ((f<<16)|i)
#define frame_label_to_key(f,l) ((f<<16)|l)
#define get_index_from_key(k) (k&0xffff)
#define get_frame_from_key(k) (k>>16)

void CoSegmentation::read_data(char *label_name, char* corr_name)
{

	read_label_file(label_name);
	read_corres_file(corr_name);

}

void CoSegmentation::read_label_file(char *filename)
{
	FILE* in_file = fopen(filename, "r");
	if (in_file==NULL)
	{
		return;
	}
	IndexType frame, label, vtx_idx;
	while ( true )
	{
		int stat = fscanf(in_file, "%d %d %d\n",&frame, &label, &vtx_idx);
		if(stat==EOF)break;
		auto fiter = compo_fast_frame_label.find( frame_label_to_key(frame,label) );
		IndexType compo_idx;
		if( fiter!=compo_fast_frame_label.end() )
			compo_idx = fiter->second;
		else
		{
			compo_fast_frame_label.insert( make_pair(frame_label_to_key(frame,label), components_.size()) );
			compo_idx = components_.size();
			components_.push_back( Component(frame, label) );
		}
		components_[compo_idx].vtx_corr_next_frame.insert(make_pair(vtx_idx,-1));
		compo_fast_frame_vtx.insert(make_pair(frame_index_to_key(frame, vtx_idx), compo_idx));
	}
	fclose(in_file);
}

void CoSegmentation::read_corres_file(char *filename)
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
		if( cur_frame+1 != next_frame && cur_frame-1 != next_frame )continue;
		auto fiter = compo_fast_frame_vtx.find(frame_index_to_key(cur_frame, cur_vtx_idx));
		if(fiter == compo_fast_frame_vtx.end())
			continue;
		IndexType compo_idx = fiter->second;
		if (cur_frame+1==next_frame)
		{
			components_[compo_idx].vtx_corr_next_frame[ cur_vtx_idx ] = next_vtx_idx;
		}
		else
		{
			components_[compo_idx].vtx_corr_prev_frame[ cur_vtx_idx ] = next_vtx_idx;
		}
		
	}

}

void CoSegmentation::propagate_component()
{
	assert(components_.size()!=0);
	IndexType sq_comp_siz = components_.size() * components_.size();
	dp_common_part = new IndexType[ sq_comp_siz ];
	memset( (void*)dp_common_part, 1, sq_comp_siz*sizeof(IndexType) );



	delete [] dp_common_part;
}

void CoSegmentation::point2point(Matrix3X & srCloud,Matrix3X & tgCloud,Matrix33 & rotMat,MatrixXX & transVec)
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

	srCloud.colwise() += X_mean;
	tgCloud.colwise() += Y_mean;

}

void CoSegmentation::calc_similarity()
{
	for(IndexType i=0;  i<components_.size()-1; ++i)
	{
			for (IndexType j=i+1;j<components_.size();++j)
			{
				if(components_[j].frame != components_[i].frame+1)
					continue;
				ScalarType d = similarity_between_component(i,j);
				if(d!=INF)
				{
					cq_.push( compo_dist_node(i,j,d) );
				}
			}
			
	}
}


void CoSegmentation::cluster_component()
{
	mint_.init( components_.size() );
	while (!cq_.empty())
	{
		compo_dist_node nod = cq_.top();
		cq_.pop();

		/*check whether to unite*/
		IndexType x = mint_.find(nod.i);
		IndexType y = mint_.find(nod.j);
		set<IndexType>& sx  =mint_.union_find_set[x];
		set<IndexType>& sy = mint_.union_find_set[y];
		bool same_frame = false;
		for ( set<IndexType>::iterator iter1 = sx.begin(); iter1!=sx.end(); iter1++ )
		{
			for (set<IndexType>::iterator iter2=sy.begin(); iter2!=sy.end(); iter2++)
			{
				Component& c1 = components_[*iter1];
				Component& c2 = components_[*iter2];
				if(c1.frame == c2.frame)
				{
					same_frame = true;
					break;
				}
			}
			if(same_frame)break;
		}

		if(!same_frame)
			mint_.unite(x, y);
	}
}

ScalarType CoSegmentation::similarity_between_component(IndexType i, IndexType j)
{
	assert( components_[i].frame==components_[j].frame-1 );
	ScalarType res = 0;
	map<IndexType,IndexType> com_part;
	IndexType com_num = compute_common(components_[i], components_[j],com_part);
	
	if (com_num==0)
	{
		return INF;
	}

	const IndexType k = 300;
	IndexType neighbours[k];
	ScalarType dist[k];

	map<IndexType, IndexType> valid_neigs;

	for ( map<IndexType,IndexType>::iterator iter = com_part.begin();
		iter != com_part.end(); iter++)
	{
		valid_neigs.clear();
		Sample& cur_frame = SampleSet::get_instance()[components_[i].frame];
		cur_frame.neighbours( iter->first, k, neighbours, dist );
		for (IndexType kk=0; kk<k; kk++)
		{
			auto fiter = com_part.find( neighbours[kk] );
			if( fiter!=com_part.end() )
			{
				assert(fiter->second!=-1);
				valid_neigs.insert( *fiter );
			}
		}

		IndexType neig_siz = valid_neigs.size();
		if(neig_siz==0)continue;
		Matrix3X s_coord, t_coord;
		s_coord.setZero(3, neig_siz);
		t_coord.setZero(3, neig_siz);

		Sample& next_frame = SampleSet::get_instance()[ components_[j].frame ];
		IndexType vi = 0;
		IndexType kk = 0;
		for ( map<IndexType,IndexType>::iterator neig_iter = valid_neigs.begin();
			neig_iter != valid_neigs.end(); ++neig_iter )
		{
			s_coord.col(kk) = cur_frame.vertices_matrix().col(neig_iter->first );
			t_coord.col(kk) = next_frame.vertices_matrix().col(neig_iter->second);
		}
		Matrix33 rot_mat;
		MatrixXX tran_vec;

		point2point(s_coord, t_coord, rot_mat, tran_vec);
		
		auto bias = rot_mat*cur_frame.vertices_matrix().col(iter->first) + tran_vec 
			- next_frame.vertices_matrix().col(iter->second);

		res += bias.norm();

	}
	IndexType max_part = max( components_[i].vtx_corr_next_frame.size(), components_[j].vtx_corr_next_frame.size() );
	ScalarType a = res/com_part.size();
	ScalarType b = max_part/com_part.size();
	//return a+0.4*b;
	return res;

}

void CoSegmentation::visualize_seg()
{

}

void CoSegmentation::build_mini_seg()
{
	mini_seg.clear();
	for ( map<IndexType,set<IndexType> >::iterator ii=mint_.union_find_set.begin();
		ii!=mint_.union_find_set.end(); ii++)
	{
		set<IndexType>& ss = ii->second;
		IndexType mini_idx = *(ss.begin()) ;
		for ( set<IndexType>::iterator jj=ss.begin(); jj!=ss.end(); jj++ )
		{
			if ( mini_idx != *jj && 
				components_[mini_idx].vtx_corr_next_frame.size()>components_[*jj].vtx_corr_next_frame.size() )
			{
				mini_idx = *jj;
			}
		}
		mini_seg.push_back(mini_idx);

	}
}

void CoSegmentation::co_segment()
{
	IndexType cluster_count=0;
	for ( map<IndexType,set<IndexType> >::iterator ii=mint_.union_find_set.begin();
		ii!=mint_.union_find_set.end(); ii++)
	{
		set<IndexType>& ss = ii->second;
		Logger<<"cluster "<<cluster_count++<<endl;
		for ( set<IndexType>::iterator jj=ss.begin(); jj!=ss.end(); jj++ )
		{
			IndexType frame = components_[*jj].frame;
			Logger<<frame<<"  "<<components_[*jj].label<<endl;
			for ( auto viter=components_[*jj].vtx_corr_next_frame.begin();
				 viter!=components_[*jj].vtx_corr_next_frame.end();viter++)
			{
				IndexType vtx_idx = viter->first;
				SampleSet::get_instance()[frame][vtx_idx].set_label( cluster_count );
			}
		}
	}

	//build_mini_seg();

}

void CoSegmentation::write_label_file(char *filename)
{
	FILE* outfile = fopen(filename, "w");
	IndexType cluster_count=0;
	for ( map<IndexType,set<IndexType> >::iterator ii=mint_.union_find_set.begin();
		ii!=mint_.union_find_set.end(); ii++)
	{
		set<IndexType>& ss = ii->second;
		Logger<<"cluster "<<cluster_count++<<endl;
		for ( set<IndexType>::iterator jj=ss.begin(); jj!=ss.end(); jj++ )
		{
			IndexType frame = components_[*jj].frame;
			Logger<<frame<<"  "<<components_[*jj].label<<endl;
			for ( auto viter=components_[*jj].vtx_corr_next_frame.begin();
				viter!=components_[*jj].vtx_corr_next_frame.end();viter++)
			{
				IndexType vtx_idx = viter->first;
				fprintf(outfile,"%d %d %d\n", frame, cluster_count, vtx_idx);
			}
		}
	}
	fclose(outfile);
}



IndexType CoSegmentation::compute_common( Component & c1, Component & c2, map<IndexType,IndexType> & common_part )
{
	IndexType count = 0;
	for ( map<IndexType,IndexType>::iterator iter = c1.vtx_corr_next_frame.begin();
			iter != c1.vtx_corr_next_frame.end(); iter++)
	{
		if( c2.vtx_corr_next_frame.find( iter->second )!=c2.vtx_corr_next_frame.end() )
		{
			count++;
			common_part.insert( *iter );
		}
	}
	return count;
}

void CoSegmentation::compute()
{
	calc_similarity();
	cluster_component();
	co_segment();
}

#undef frame_index_to_key
#undef frame_label_to_key
#undef get_index_from_key
#undef get_frame_from_key