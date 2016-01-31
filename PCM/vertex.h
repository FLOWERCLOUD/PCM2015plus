#ifndef _VERTEX_H
#define _VERTEX_H
#include "windows.h"
#include <gl/gl.h>
#include "selectable_item.h"
#include <iostream>

class Vertex : public SelectableItem
{
public:
	Vertex():position_(NULL_POINT),
		normal_(NULL_NORMAL),label_(0),val_(0),is_edge_points_(0),is_wrapbox_(0) ,is_edgePointWithSmallLabel_(-1){};
	~Vertex(){};

	void set_idx(IndexType _idx)
	{
		idx_ = _idx;
	}
	void set_position( const PointType& pos )
	{
		position_ = pos;
	}

	void set_normal( const NormalType& n )
	{
		normal_ = n;
	}

	void set_label( IndexType l ){ label_ = l; }

	void set_value(ScalarType v_){ val_ = v_;}

	// added by huayun
	void set_edge_point(IndexType l){is_edge_points_ = l;}
	void set_edgePointWithSmallLabel(IndexType l){ is_edgePointWithSmallLabel_ = l;}

	void set_wrapbox(IndexType l){is_wrapbox_ = l;}

	ScalarType x() const { return position_(0); }
	ScalarType y() const { return position_(1); }
	ScalarType z() const { return position_(2); }
	ScalarType nx() const { return normal_(0); }
	ScalarType ny() const { return normal_(1); }
	ScalarType nz() const { return normal_(2); }
	ScalarType r() const { return color_(0); }
	ScalarType g() const { return color_(1); }
	ScalarType b() const { return color_(2); }
	ScalarType alpha() const { return color_(3); }
	IndexType label() const {return label_;}
	ScalarType value_() const {return val_;} 
	/*
		Without adjust_matrix is not recommend,
		which would display sample's original position,
		With adjust matrix make sure sample can be saw
		in the origin.
	*/
	void draw();
	void draw( const Matrix44& adjust_matrix );
	void draw( const Matrix44& adjust_matrix, const Vec3& bias );
	
	void draw_with_label( const Matrix44& adjust_matrix );
	void draw_with_label( const Matrix44& adjust_matrix, const Vec3& bias );
	void draw_with_Graph_wrapbox( const Matrix44& adjust_matrix );
	void draw_with_Graph_wrapbox( const Matrix44& adjust_matrix ,const Vec3& bias);

	void draw_with_edgepoints( const Matrix44& adjust_matrix );	
	void draw_with_edgepoints( const Matrix44& adjust_matrix,const Vec3& bias );

	inline void draw_without_color();
	inline void draw_without_color(const Matrix44& adjust_matrix);
	inline void draw_without_color(const Matrix44& adjust_matrix, const Vec3& bias);
	void draw_with_name(unsigned int idx, const Matrix44& adjust_matrix);
	void draw_with_sphere( const Matrix44& adjust_matrix , const Vec3& bias);
protected:
	PointType position_;
	NormalType normal_;

	IndexType	label_;
	ScalarType  val_;
	IndexType	is_edge_points_;
	IndexType   is_edgePointWithSmallLabel_;
	IndexType	is_wrapbox_;
	IndexType   idx_;  //the idx in vertices ,may make mistakes when erased

};



#endif