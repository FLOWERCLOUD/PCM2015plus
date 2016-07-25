#ifndef _TRACER_H
#define _TRACER_H

#include <vector>
#include <map>
#include "basic_types.h"
#include "globals.h"


using namespace std;

class Tracer
{
public:

	typedef pair<IndexType, IndexType>	Sample_Vertex_Index;
	typedef pair<IndexType, IndexType>  Sample_line_label;  //line 的起点作为label号


	static Tracer& get_instance();

	void add_record( IndexType sample0_idx,
					IndexType sample0_vtx_idx,
					IndexType sample1_idx,
					IndexType sample1_vtx_idx,
					IndexType _label = 0);

	void clear_records();

	void add_percentCor( IndexType srGraph,IndexType tgGraph,MatrixXXi & cor,
		                 IndexType percent,IndexType srgraphVtxNum);

	void add_percentCor_SE( IndexType srGraph,IndexType tgGraph,MatrixXXi & start,MatrixXXi & end,
		IndexType percent,IndexType srgraphVtxNum);
	void setcenterframeNum(IndexType centerframe);
	void draw();

	//-------draw a  plan
	void drawPlan(PointType center,NormalType planNorm);

private:
	Tracer();
	~Tracer();;
	Tracer( const Tracer&);
	void operator=(const Tracer&);

private:
	vector< Sample_Vertex_Index >		head_;
	vector< Sample_Vertex_Index >	tail_;
	vector<Sample_line_label>		line_;
	IndexType centerframenum_;

};

#endif