#include "caculateNorm.h"
#include "sample_properity.h"
#include "sample_set.h"
#include "vertex.h"
#include "triangle.h"
namespace SampleManipulation{

	void CaculateNorm::compute_normal_all(NormalType& baseline)
	{
		baseline_  = baseline;


	}



	void CaculateNorm::run()
	{
		
		
		const IndexType k = 36;   //36;

		
		int  size = SampleSet::get_instance().size();
		for ( int ii = 0 ; ii<size; ++ii){
			Sample& smp = SampleSet::get_instance()[ii];
			std::cout<< "caculate:"<< ii<<std::endl;
			if( !smp.num_triangles())  //only have points
			{
				for ( IndexType i=0; i < smp.num_vertices(); i++ )
				{

					MatrixX3	k_nearest_verts(k, 3);
					IndexType		neighbours[k];
					ScalarType dist[k];
					smp.neighbours( i, k, neighbours, dist );
					for ( IndexType j=0; j<k; j++ )
					{
						IndexType neighbour_idx = neighbours[j];

						k_nearest_verts.row(j) << smp[neighbour_idx].x(), smp[neighbour_idx].y(), smp[neighbour_idx].z();
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
					NormalType nv; 
					nv(0) = (ev.col(min_idx))(0).real();
					nv(1) = (ev.col(min_idx))(1).real();
					nv(2) = (ev.col(min_idx))(2).real();

					nv.normalize();
					if ( (baseline_).dot(nv) < 0 )
					{
						nv = - nv;
					}

					smp[i].set_normal( nv );

				}

			}else
			{ //has face
				//auto& m_triangles  = smp.triangle_array;
				for ( IndexType i=0; i < smp.num_triangles(); ++i )
				{	
					IndexType i_vetex1 = smp.getTriangle(i).get_i_vertex(0);
					IndexType i_vetex2 = smp.getTriangle(i).get_i_vertex(1);
					IndexType i_vetex3 = smp.getTriangle(i).get_i_vertex(2);
					PointType vtx1(smp[i_vetex1].x(),smp[i_vetex1].y(),smp[i_vetex1].z());
					PointType vtx2(smp[i_vetex2].x(),smp[i_vetex2].y(),smp[i_vetex2].z());
					PointType vtx3(smp[i_vetex3].x(),smp[i_vetex3].y(),smp[i_vetex3].z());
					PointType vector1 = vtx2 - vtx1;
					PointType vector2 = vtx3 - vtx1;
					vector1.normalize();
					vector2.normalize();
					PointType vector3 = vector1.cross(vector2); //get the normal of the triangle
					vector3.normalize();
					//Logger<<"vector1: "<<vector1(0)<<" "<<vector1(1)<<" "<<vector1(2)<<std::endl;
					//Logger<<"vector2: "<<vector2(0)<<" "<<vector2(1)<<" "<<vector2(2)<<std::endl;
					//Logger<<"vector3: "<<vector3(0)<<" "<<vector3(1)<<" "<<vector3(2)<<std::endl;
					//assign the normal to all the vertex of the triangle
					for(int x = 0 ; x<3;++x)
					{
						IndexType i_normal = smp.getTriangle(i).get_i_normal(x);
						//Logger<<"norm: "<<smp[i_normal].nx()<<" "<<smp[i_normal].ny()<<" "<<smp[i_normal].nz()<<std::endl;
						smp[i_normal].set_normal( 
							NormalType( smp[i_normal].nx() + vector3(0),smp[i_normal].ny() +vector3(1),smp[i_normal].nz()+vector3(2) ));
					}					
					

				}
				for ( IndexType i=0; i < smp.num_vertices(); i++ )
				{
					NormalType norm(smp[i].nx(),smp[i].ny(),smp[i].nz());
					norm.normalize();
		//			Logger<<"norm: "<<norm(0)<<" "<<norm(1)<<" "<<norm(2)<<std::endl;
					smp[i].set_normal(norm);
				}

			}


		}

			



	}
}
