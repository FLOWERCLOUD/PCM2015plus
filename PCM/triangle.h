#ifndef _TRIANGLE_
#define _TRIANGLE_

#include "basic_types.h"
#include "selectable_item.h"
namespace RenderMode
{
	enum WhichColorMode;
	enum RenderType;
}
class Sample;
class TriangleType:public SelectableItem
{
public:
	TriangleType(Sample& s);
	TriangleType(const TriangleType& s);
	TriangleType& operator = (const TriangleType& s);
	void draw(RenderMode::WhichColorMode& wcm, RenderMode::RenderType& r,
		const Matrix44& adjust_matrix, const Vec3& bias);
	Sample& sample_;
	IndexType i_vertex[3];
	IndexType i_norm[3];
};
#endif

