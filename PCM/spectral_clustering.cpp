#include "spectral_clustering.h"
#include "Eigen/Sparse"
#include <math.h>
#include "math_fun.h"
#include "globals.h"
//#include <engine.h>
#include <assert.h>
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WIN32
#include <direct.h>
#define get_current_dir _getcwd
#else
#include <unistd.h>
#define get_current_dir getcwd
#endif



using namespace Eigen;


void SpectralClusteringThread::run()
{


 //	 SampleSet& set = SampleSet::get_instance();

 //	 const IndexType	num_vtx = set[0].num_vertices();
 //	 const IndexType num_of_neighbours = 20;
	// IndexType num_of_cluster = 10;
 //	 IndexType	num_sample = set.size();
 //	 IndexType	dim = 4*(num_sample-1) + 3*num_sample;
 //	 MatrixXX	feature_vector( num_vtx, dim );

	// std::cout<<" Number of Cluster :";
	// std::cin >> num_of_cluster;

	// Logger << "Begin Clustering.\n";
 //	 
	// //** Step 1: Compute feature vector for every vertex **/
	// for(IndexType s_idx = 0; s_idx < num_sample - 1;
	//	 s_idx++ )
	// {

	//	 Matrix3X&	orig_vtx_coord_matrix = set[s_idx].vertices_matrix();
	//	 Matrix3X&	dest_vtx_coord_matrix = set[s_idx + 1].vertices_matrix();


	//	 for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
	//	 {

	//		 IndexType origin_neighbours[num_of_neighbours];
	//		 IndexType dest_neighbours[num_of_neighbours];

	//		 //Get neighbours of the specific vertex between two sample
	//		 set[s_idx].neighbours(v_idx, num_of_neighbours, origin_neighbours);
	//		 set[s_idx + 1].neighbours(v_idx, num_of_neighbours, dest_neighbours);
	//		 MatrixX3	X(num_of_neighbours, 3);
	//		 MatrixX3	Y(num_of_neighbours, 3);

	//		 for ( int j = 0; j<num_of_neighbours; j++ )
	//		 {
	//			 X.row(j) << orig_vtx_coord_matrix(0, origin_neighbours[j]),
	//				 orig_vtx_coord_matrix(1, origin_neighbours[j]),
	//				 orig_vtx_coord_matrix(2, origin_neighbours[j]);
	//			 Y.row(j) << dest_vtx_coord_matrix(0, origin_neighbours[j]),
	//				 dest_vtx_coord_matrix(1, origin_neighbours[j]),
	//				 dest_vtx_coord_matrix(2, origin_neighbours[j]);
	//		 }

	//		 Matrix33 sigma = (X.rowwise() - X.colwise().mean()).transpose() * (Y.rowwise() - Y.colwise().mean());
	//		 Matrix33 rot_mat;

	//		 Eigen::JacobiSVD<Matrix33> svd(sigma, Eigen::ComputeFullU | Eigen::ComputeFullV);
	//		 if(svd.matrixU().determinant()*svd.matrixV().determinant() < 0.0) {
	//			 Vec3 S = Vec3::Ones(); S(2) = -1.0;
	//			 rot_mat = svd.matrixV()*S.asDiagonal()*svd.matrixU().transpose();
	//		 } else {
	//			 rot_mat = svd.matrixV()*svd.matrixU().transpose();
	//		 }
	//		 Vec4 quat;
	//		 Math_Utility::rotation_matrix2quat(rot_mat,quat);

	//		 feature_vector.block<1,4>( v_idx,s_idx*4 ) << quat.transpose();
	//	 }
	// }

	// ///Add position features
	// for(IndexType s_idx = 0; s_idx < num_sample;
	//	 s_idx++ )
	// {
	//	 for (IndexType v_idx = 0; v_idx < num_vtx; v_idx++)
	//	 {
	//		feature_vector.block<1,3>( v_idx, (num_sample-1)*4+s_idx*3 ) << set[s_idx][v_idx].x(), set[s_idx][v_idx].y(), set[s_idx][v_idx].z();
	//	 }

	// }




	////** Step 2: Spectral Clustering, use Matlab **/
 // 	Engine*	ep;
 //	if (! (ep = engOpen(NULL)) )
 //	{
 //		Logger<< "Can't not start Matlab engine.\n";
 //		return;
 //	}
 //
 //	// set buffer to display result
 //	IndexType	result_buffer_size = 1024*1000;
 //	char*		result_buffer = new char[result_buffer_size];
 //	engOutputBuffer(ep, result_buffer, result_buffer_size);
 //

	////Get the executable file's path
	//char cur_path[FILENAME_MAX];
	//if (!get_current_dir(cur_path, sizeof(cur_path)))
	//{
	//	return;
	//}

	//cur_path[sizeof(cur_path) - 1] = '\0'; 
	//char cd_cur_path[FILENAME_MAX + 3] = "cd ";
	//strcat(cd_cur_path, cur_path);
	//engEvalString(ep, cd_cur_path );

	//mxArray*	mx_feature_vec = NULL;
	//numeric::float64*	fv_buffer;
	//fv_buffer = new numeric::float64[ feature_vector.rows()*feature_vector.cols() ];
	//IndexType k =0;
	//for (IndexType i =0 ; i<feature_vector.cols(); i++)
	//{
	//	for (IndexType j=0; j<feature_vector.rows(); j++)
	//	{
	//		fv_buffer[k++] = feature_vector(j, i);
	//	}
	//}

	//mx_feature_vec = mxCreateDoubleMatrix( feature_vector.rows(),
	//					feature_vector.cols(), mxREAL);
	//memcpy( (char*)mxGetPr(mx_feature_vec),(char*)fv_buffer, 
	//	feature_vector.rows()*feature_vector.cols()*sizeof(numeric::float64) );

 // 	engPutVariable( ep, "data", mx_feature_vec );
	//char	cmd_buf[128];
	//engEvalString( ep, cmd_buf );
	/////use default sigma
	/////sprintf(cmd_buf,"[labels]=sc(data,%d,100,0,%d);",num_of_neighbours, num_of_cluster);
	/////just kmeans here
	//sprintf(cmd_buf, "[labels]=k_means(data,\'random\',%d);",num_of_cluster);
	//engEvalString(ep,cmd_buf);
 //
 // 
 // 	///Display output information
 // 	Logger<<result_buffer<<std::endl;
 //

	//mxArray* mx_labels = NULL;
	//mx_labels = engGetVariable( ep, "labels" ); 

	//if ( mx_labels == NULL )
	//{

	//	mxDestroyArray( mx_feature_vec );
	//	mxDestroyArray(mx_labels);
	//	engClose(ep);
	//	delete [] fv_buffer;
	//	delete	[] result_buffer;
	//	Logger << "Get Data from Matlab error. End Clustering."<<std::endl;
	//	return;
	//}

	//numeric::float64 *labels = mxGetPr(mx_labels);	
	//assert(( mxGetNumberOfElements(mx_labels))==num_vtx );


 //   ///** Step 3: Store the results **/
	//LOCK( set[0] );
	//Sample& sample0 = set[0];
	//IndexType i = 0;
	//for (Sample::vtx_iterator v_iter = sample0.begin();
	//	v_iter != sample0.end();
	//	v_iter++,i++ )
	//{
	//	(*v_iter)->set_label( labels[i] );
	//}
	//UNLOCK(set[0]);

	//mxDestroyArray( mx_feature_vec );
	//mxDestroyArray(mx_labels);

 //	engClose(ep);
	//delete [] fv_buffer;
 //	delete	[] result_buffer;


	//Logger << "End Clustering."<<std::endl;
}