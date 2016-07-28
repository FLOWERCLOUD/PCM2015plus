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
	inline void set_i_vetex(IndexType which_vertex ,IndexType index )
	{
		i_vertex[which_vertex] = index;
	}
	inline void set_i_normal(IndexType which_normal ,IndexType index )
	{
		i_norm[which_normal] = index;
	}
	inline IndexType get_i_vertex(IndexType which_vertex )
	{
		return i_vertex[which_vertex];
	}
	inline IndexType get_i_normal(IndexType which_normal )
	{
		return i_norm[which_normal];
	}
	void draw(RenderMode::WhichColorMode& wcm, RenderMode::RenderType& r,
		const Matrix44& adjust_matrix, const Vec3& bias);
private:
	Sample& sample_;

	IndexType i_vertex[3];
	IndexType i_norm[3];
};
#endif

