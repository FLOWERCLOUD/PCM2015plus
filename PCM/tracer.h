#ifndef _TRACER_H
#define _TRACER_H

#include <vector>
#include <map>
#include "basic_types.h"
#include "sample_set.h"
#include "sample.h"
#include "vertex.h"
#include "globals.h"
#include "paint_canvas.h"

using namespace std;

class Tracer
{
public:

	typedef pair<IndexType, IndexType>	Sample_Vertex_Index;


	static Tracer& get_instance()
	{
		static Tracer	instance;
		return instance;
	}

	void add_record( IndexType sample0_idx,
					IndexType sample0_vtx_idx,
					IndexType sample1_idx,
					IndexType sample1_vtx_idx)
	{
		head_.push_back( make_pair( sample0_idx, sample0_vtx_idx ) );
		tail_.push_back( make_pair(sample1_idx, sample1_vtx_idx) );
	}

	void clear_records()
	{
		head_.clear();
		tail_.clear();
	}

	void add_percentCor( IndexType srGraph,IndexType tgGraph,MatrixXXi & cor,
		                 IndexType percent,IndexType srgraphVtxNum)
	{
		assert(percent >= 1 && srgraphVtxNum > 0);
        for (IndexType vtx_idx = 0; vtx_idx < srgraphVtxNum; vtx_idx += percent)
        {
			add_record(srGraph,vtx_idx,tgGraph,cor(0,vtx_idx));
        }        
	}

	void add_percentCor_SE( IndexType srGraph,IndexType tgGraph,MatrixXXi & start,MatrixXXi & end,
		IndexType percent,IndexType srgraphVtxNum)
	{
		assert(percent >= 1 && srgraphVtxNum > 0);
		for (IndexType vtx_idx = 0; vtx_idx < srgraphVtxNum; vtx_idx += percent)
		{
			add_record(srGraph,start(0,vtx_idx),tgGraph,end(0,vtx_idx));
		}        
	}
	void setcenterframeNum(IndexType centerframe)
	{
		centerframenum_= centerframe;
	}
	void draw()
	{
		IndexType	num_record = head_.size();
		assert( num_record == tail_.size() );

		if (num_record==0)
		{
			return;
		}

		SampleSet&	set = SampleSet::get_instance();

		glColor4f(1.,0.,0.,1.);
		glLineWidth( Paint_Param::g_line_size );
		glBegin(GL_LINES);
		for ( int i=0; i < num_record; i++ )
		{
			IndexType	sample0_idx = head_[i].first;
			IndexType	sample0_vtx_idx = head_[i].second;
			IndexType	sample1_idx = tail_[i].first;
			IndexType	sample1_vtx_idx = tail_[i].second;
			Vertex&		sample0_vtx = set[ sample0_idx ][ sample0_vtx_idx ];
			Vertex&		sample1_vtx = set[ sample1_idx ][ sample1_vtx_idx ];
			
            Vec4	first_v	= set[sample0_idx].matrix_to_scene_coord() *
				Vec4(sample0_vtx.x(), sample0_vtx.y(),sample0_vtx.z(),1.0);
			Vec4	second_v	= set[sample1_idx].matrix_to_scene_coord() *
				Vec4(sample1_vtx.x(), sample1_vtx.y(),sample1_vtx.z(),1.0);
			Vec3 bias0 = Paint_Param::g_step_size * (ScalarType)((int)sample0_idx - centerframenum_ );
			Vec3 bias1 = Paint_Param::g_step_size * (ScalarType)((int)sample1_idx -centerframenum_);
			glColor4f( 1,0,0,1 );
			glVertex3f( first_v(0)+bias0(0),first_v(1)+bias0(1),first_v(2)+bias0(2) );
			glVertex3f( second_v(0)+bias1(0), second_v(1)+bias1(1),second_v(2) + bias1(2) );



		}
		glEnd();
	}

	//-------draw a  plan
	void drawPlan(PointType center,NormalType planNorm)
	{
		ScalarType pz_1,pz_2,pz_3,pz_4;
		pz_1 = center(2)  + ( (center(0) - 0.)*planNorm(0) + (center(1) - 0.)*planNorm(1) )/planNorm(2);
		pz_2 = center(2)  + ( (center(0) - 0.)*planNorm(0) + (center(1) - 1000.)*planNorm(1) )/planNorm(2);
		pz_3 = center(2)  + ( (center(0) - 1000.)*planNorm(0) + (center(1) - 0.)*planNorm(1) )/planNorm(2);
		pz_4 = center(2)  + ( (center(0) - 1000.)*planNorm(0) + (center(1) - 1000.)*planNorm(1) )/planNorm(2);

		Logger<<"point_1"<<pz_1<<endl;
		Logger<<"point_2"<<pz_2<<endl;
		Logger<<"point_3"<<pz_3<<endl;
		Logger<<"point_4"<<pz_4<<endl;

		glColor4f(1.,0.,0.,1.);
		glLineWidth( Paint_Param::g_point_size );
		glBegin(GL_LINE_LOOP);
			glColor4f( 1,0,0,1 );
			glVertex3f(0.,0.,pz_1);
			glVertex3f(0.,1000.,pz_2);
			glVertex3f(1000.,1000.,pz_4);
			glVertex3f(1000.,0.,pz_3);

		glEnd();
	}

private:
	Tracer(){
		centerframenum_ = 0;
	}
	~Tracer(){};
	Tracer( const Tracer&);
	void operator=(const Tracer&);

private:
	vector< Sample_Vertex_Index >		head_;
	vector< Sample_Vertex_Index >	tail_;
	IndexType centerframenum_;

};

#endif