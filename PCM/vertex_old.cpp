#include "vertex.h"
#include <iostream>
#include "globals.h"
#include "color_table.h"
void Vertex::draw(){
	if (!visible_)
	{
		return;
	}
	glColor4f( color_(0), color_(1), color_(2), color_(3) );
	draw_without_color();
};

void Vertex::draw_without_color()
{
	if (!visible_)
	{
		return;
	}

	glNormal3f( normal_(0), normal_(1), normal_(2));
	glVertex3f(position_(0), position_(1), position_(2));
}

void  Vertex::draw( const Matrix44& adjust_matrix )
{
	if (!visible_)
	{
		return;
	}
	glColor4f( color_(0), color_(1), color_(2), color_(3) );
	draw_without_color(adjust_matrix);
}

void  Vertex::draw_without_color( const Matrix44& adjust_matrix)
{
	if (!visible_)
	{
		return;
	}

	glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glVertex3f( point_to_show(0), point_to_show(1), point_to_show(2) );
}

void Vertex::draw( const Matrix44& adjust_matrix, const Vec3& bias )
{
	if (!visible_)
	{
		return;
	}
	glColor4f( color_(0), color_(1), color_(2), color_(3) );
	draw_without_color(adjust_matrix, bias);
}

void Vertex::draw_without_color(const Matrix44& adjust_matrix, const Vec3& bias)
{
	if (!visible_)
	{
		return;
	}

	glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glVertex3f( point_to_show(0) + bias(0), 
		point_to_show(1) + bias(1), 
		point_to_show(2) + bias(2));
}

void Vertex::draw_with_name(unsigned int idx, const Matrix44& adjust_matrix)
{
	glPushName(idx);
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glRasterPos3f(point_to_show(0), point_to_show(1), point_to_show(2));
	glPopName();
}


void Vertex::draw_with_label( const Matrix44& adjust_matrix )
{
	if (!visible_)
	{
		return;
	}


	static const IndexType color_step = 47;
	IndexType	label_color = (label_ * color_step) % 255;
	ColorType color = Color_Utility::color_from_table(label_color);


	glColor4f( color(0),color(1),color(2),color(3) );
	glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glVertex3f( point_to_show(0), point_to_show(1), point_to_show(2) );

}

void Vertex::draw_with_label( const Matrix44& adjust_matrix, const Vec3& bias )
{
	if (!visible_)
	{
		return;
	}


	static const IndexType color_step = 47;
	IndexType	label_color = (label_ * color_step) % 255;
	ColorType color = Color_Utility::color_from_table(label_color);


	glColor4f( color(0),color(1),color(2),color(3) );
	glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glVertex3f( point_to_show(0)+bias(0), point_to_show(1)+bias(1), point_to_show(2)+bias(2) );

}