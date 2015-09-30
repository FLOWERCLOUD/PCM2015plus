#ifndef _WLOP_H
#define _WLOP_H

#include <vector>
#include "basic_types.h"
#include "sample_set.h"

using namespace std;

class WLOP
{
public:

	void init(IndexType si, ScalarType rad, ScalarType gau, ScalarType rep_pow, ScalarType mu)
	{
		radius_ = rad;
		gaussian_para_ = gau;
		rep_mu_ = mu;
		smp_idx_ = si;
		rep_power_ = rep_pow;
		avg_.clear();
		IndexType v_siz = SampleSet::get_instance()[smp_idx_].num_vertices();
		avg_.resize( v_siz );
		avg_w_sum_.resize( v_siz );
		repulsion_.resize( v_siz );
		rep_w_sum_.resize( v_siz );
	}

	void compute_repulsion_term()
	{
		ScalarType radius2 = radius_ * radius_;
		ScalarType iradius16 = - gaussian_para_ / radius2;

		Sample& smp = SampleSet::get_instance()[smp_idx_];

		for (IndexType i = 0; i < smp.num_vertices(); ++i)
		{
			Vertex& v = smp[i];
			const IndexType k = 20;
			IndexType neigs[k];
			smp.neighbours(i,k, neigs);
			for ( IndexType j =0; j<k; j++ )
			{
				Vertex& neig_v = smp[ neigs[j] ];
				ScalarType dx = v.x() - neig_v.x();
				ScalarType dy = v.y() - neig_v.y();
				ScalarType dz = v.z() - neig_v.z();
				ScalarType dist2 = dx * dx + dy * dy + dz * dz;
				ScalarType w = exp( dist2 * iradius16 );
				ScalarType len = sqrt(dist2);
				if(len <= 0.001 * radius_) len = radius_*0.001;
				ScalarType rep = w * pow(1.0/len, rep_power_);

				repulsion_[i] += rep * PointType( dx, dy, dz );
				rep_w_sum_[i] += rep;

			}
	}

	void compute_avg_term( )
	{
		ScalarType radius2 = radius_ * radius_;
		ScalarType iradius16 = - gaussian_para_ / radius2;

		Sample& smp = SampleSet::get_instance()[smp_idx_];

		for (IndexType i = 0; i < smp.num_vertices(); ++i)
		{
			Vertex& v = smp[i];
			const IndexType k = 20;
			IndexType neigs[k];
			smp.neighbours(i,k, neigs);
			for ( IndexType j =0; j<k; j++ )
			{
				Vertex& neig_v = smp[ neigs[j] ];
				ScalarType dx = v.x() - neig_v.x();
				ScalarType dy = v.y() - neig_v.y();
				ScalarType dz = v.z() - neig_v.z();
				ScalarType dist2 = dx * dx + dy * dy + dz * dz;
				ScalarType w = exp( dist2 * iradius16 );
				avg_[i] = w * PointType( v.x(), v.y(), v.z() );
				avg_w_sum_[i] += w;
			}
		}

	}

	void run(int n_iterate)
	{
		for (int i=0; i<n_iterate; i++)
		{
			iterate();
		}
		SampleSet::get_instance()[smp_idx_].build_kdtree();
	}
	void iterate()
	{
		compute_repulsion_term();
		compute_avg_term();
		
		Sample& smp = SampleSet::get_instance()[smp_idx_];

		for (int i=0; i<smp.num_vertices(); i++)
		{
			Vertex& v = smp[i];
			if ( avg_w_sum_[i] > 1e-20 )
			{
				v.set_position( avg_[i] / avg_w_sum_[i] );
			}
			if ( rep_w_sum_[i] > 1e-20 && rep_mu_ >=0 )
			{
				PointType tmp_v( v.x(), v.y(), v.z() );
				v.set_position( tmp_v + rep_mu_*(repulsion_[i]/rep_w_sum_[i]) );
			}

		}
	}

private:
	ScalarType radius_;
	ScalarType rep_mu_;
	ScalarType gaussian_para_;
	ScalarType rep_power_;
	IndexType smp_idx_;
	vector<PointType> avg_;
	vector<ScalarType> avg_w_sum_;
	vector<PointType> repulsion_;
	vector<ScalarType> rep_w_sum_;
};

#endif