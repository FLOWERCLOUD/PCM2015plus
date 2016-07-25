#ifndef _SAMPLE_PROPERITY_H
#define _SAMPLE_PROPERITY_H
#include "basic_types.h"
#include "octree.h"
class Sample;
namespace SampleManipulation{
	void compute_normal_all(NormalType& baseline);
	void compute_normal( IndexType selected_smp, NormalType& baseline );

	void generate_hypothesis( IndexType seed_vtx_idx, IndexType sample_vtx_idx, IndexType sample_size, ScalarType sigma );
	
	void computerMinMax(IndexType selected_smp);


	class SubSampleSolution
	{

	public:
		SubSampleSolution( Sample& s, Sample& t, IndexType r );
		~SubSampleSolution();
		vector<IndexType> compute();
		void build_subSample( vector<vector<IndexType> >& groups,vector<IndexType>& vtx_map);

		inline ScalarType l1_distance( PointType& p1, PointType& p2 )
		{
			return fabs(p1(0)-p2(0))+fabs(p1(1)-p2(1))+fabs(p1(2)-p2(2));
		}

		inline ScalarType l2_distance( PointType& p1, PointType& p2 )
		{

			ScalarType dx = p1(0)-p2(0);
			ScalarType dy = p1(1)-p2(1);
			ScalarType dz = p1(2)-p2(2);
			return dx*dx + dy*dy +dz*dz;
		}

	private:
		Sample& source_sample_;
		Sample& target_sample_;
		IndexType& resolv_;
		Octree<ScalarType,IndexType> otree_;

	};

	ScalarType minVal;
	ScalarType maxVal;
}

#endif