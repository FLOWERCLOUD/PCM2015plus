#ifndef _MATH_FUN_H
#define _MATH_FUN_H
#include "basic_types.h"
namespace Math_Utility{
	void rotation_matrix2quat(Matrix33 &m,Vec4& quat)
	{
		ScalarType tr  = m(0,0) + m(1,1) + m(2,2);
		if ( tr > 0. )
		{
			ScalarType s = sqrt(tr + 1. )*2.0;
			quat(3) = 0.25 * s;
			quat(0) = ( m(2,1) - m(1,2) ) / s;
			quat(1) = ( m(0,2) - m(2,0) ) / s;
			quat(2) = ( m(1,0) - m(0,1) ) / s;
		}
		else if ( ( m(0,0)>m(1,1) )&&( m(0,0)>m(2,2) ) )
		{
			ScalarType s = sqrt( 1. + m(0,0) - m(1,1) - m(2,2) )*2.;
			quat(3) = ( m(2,1)-m(1,2) ) / s;
			quat(0) = 0.25 * s;
			quat(1) = ( m(0,1)+m(1,0) ) / s;
			quat(2) = ( m(0,2)+m(2,0) ) / s;
		}
		else if( m(1,1)>m(2,2) )
		{
			ScalarType s = sqrt( 1. + m(1,1) - m(0,0) - m(2,2) ) * 2.;
			quat(3) = ( m(0,2) - m(2,0) ) / s;
			quat(0) = ( m(0,1) + m(1,0) ) / s;
			quat(1) = 0.25 * s;
			quat(2) = ( m(1,2) + m(2,1) ) / s;
		}
		else
		{
			ScalarType s =sqrt(1. + m(2,2)-m(0,0)-m(1,1) ) * 2.;
			quat(3) = ( m(1,0)-m(0,1) ) / s;
			quat(0) = ( m(0,2) + m(2,0) ) /s;
			quat(1) = ( m(1,2) + m(2, 1) ) / s;
			quat(2) = 0.25 * s;

		}
	}
}

#endif