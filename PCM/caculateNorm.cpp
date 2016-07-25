#include "caculateNorm.h"
#include "sample_properity.h"
#include "multiway_propagation.h"
#include "sample_set.h"
#include "vertex.h"
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

		}

			



	}
}
