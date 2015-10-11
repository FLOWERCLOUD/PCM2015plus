#ifndef _BOX_H
#define _BOX_H
#include "basic_types.h"

class Box
{
public:
	Box():first_expand_(true){}
	~Box(){}

	void expand( const PointType&  p )
	{
		diag_dirty_ = true;
		if (first_expand_)
		{
			low_corner_ = p;
			high_corner_ = p;
			first_expand_ = false;
			return;
		}

		for (int dim=0; dim <3; dim++)
		{
			if (p(dim) < low_corner_(dim) )
				low_corner_(dim) = p(dim);
			else if( p(dim) > high_corner_(dim) )
				high_corner_(dim) = p(dim);
		}
	}

	void reset(){ first_expand_ = true;diag_dirty_=true; }

	const ScalarType diag()
	{
		if (first_expand_)
		{
			return 0.;
		}

		if (!diag_dirty_)
		{
			return diag_;
		}

		ScalarType d[3];
		for (int dim = 0; dim < 3; dim++ )
		{
			d[dim] = fabs( low_corner_(dim) - high_corner_(dim) );
		}

		diag_dirty_ = false;
		diag_ = sqrt( d[0]*d[0] + d[1]*d[1] + d[2]*d[2] );

		return diag_;

	}

	const PointType center() const
	{
		return ( low_corner_ + high_corner_ )/2;
	}
	const PointType low_corner() const{return low_corner_;}
	const PointType high_corner()const{return high_corner_;}

private:
	PointType low_corner_;
	PointType high_corner_;

	//Because sqrt is expensive, we record dirty flag to update diag
	bool diag_dirty_;
	ScalarType diag_;
	bool first_expand_;
};

#endif