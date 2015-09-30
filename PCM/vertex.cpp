#include "vertex.h"
#include <iostream>
#include "globals.h"
#include "color_table.h"
#include "glut.h"
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


	//static const IndexType color_step = 47;
	//IndexType	label_color = (label_ * color_step) % 255;
	//ColorType color = Color_Utility::color_from_table(label_color);
	ColorType color = Color_Utility::span_color_from_table(label_); 


	glColor4f( color(0)/255.0,color(1)/255.0,color(2)/255.0,color(3) );
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


	//static const IndexType color_step = 47;
	//IndexType	label_color = (label_ * color_step) % 255;
	//ColorType color = Color_Utility::color_from_table(label_color);
//	ColorType color = Color_Utility::span_color_from_table(label_);//可视化标签 7-28 
	ColorType color = Color_Utility::span_color_from_hy_table(label_);
	//ColorType color = Color_Utility::color_map_one(val_);//可视化曲率

	glColor4f( color(0)/255.0,color(1)/255.0,color(2)/255.0,color(3) );
	glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glVertex3f( point_to_show(0)+bias(0), point_to_show(1)+bias(1), point_to_show(2)+bias(2) );

}
// add by hua yun
void Vertex::draw_with_Graph_wrapbox( const Matrix44& adjust_matrix )
{
	

}
void Vertex::draw_with_Graph_wrapbox( const Matrix44& adjust_matrix ,const Vec3& bias)
{


}

void Vertex::draw_with_edgepoints( const Matrix44& adjust_matrix )
{
	if (!visible_)
	{
		return;
	}


	//static const IndexType color_step = 47;
	//IndexType	label_color = (label_ * color_step) % 255;
	//ColorType color = Color_Utility::color_from_table(label_color);
	ColorType color;
	if( is_edge_points_){
		color = Color_Utility::span_color_from_table_with_edge(label_);
	}else{
		color = Color_Utility::span_color_from_table(label_); 
	}


	glColor4f( color(0),color(1),color(2),color(3) );
	glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glVertex3f( point_to_show(0), point_to_show(1), point_to_show(2) );

}
void Vertex::draw_with_edgepoints( const Matrix44& adjust_matrix , const Vec3& bias)
{
	if (!visible_)
	{
		return;
	}


	//static const IndexType color_step = 47;
	//IndexType	label_color = (label_ * color_step) % 255;
	//ColorType color = Color_Utility::color_from_table(label_color);
	ColorType color;
	if( is_edge_points_){
		if(is_edgePointWithSmallLabel_){
			ScalarType r = 0.0f;
			ScalarType g = 1.0f;
			ScalarType b = 0.0f;
			color = ColorType(r,g,b,1.);
		}else{
			ScalarType r = 0.0f;
			ScalarType g = 0.0f;
			ScalarType b = 1.0f;
			color = ColorType(r,g,b,1.);
		}
		
	}else{
		color = Color_Utility::span_color_from_table(label_); 
	}


	glColor4f( color(0),color(1),color(2),color(3) );
	glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	glVertex3f( point_to_show(0)+bias(0), point_to_show(1)+bias(1), point_to_show(2)+bias(2) );

}

void Vertex::draw_with_sphere( const Matrix44& adjust_matrix , const Vec3& bias)
{
	if (!visible_)
	{
		return;
	}
	ColorType color;
	ScalarType r = 0.0f;
	ScalarType g = 1.0f;
	ScalarType b = 0.0f;
	color = ColorType(r,g,b,1.);



	// Set material properties
	GLfloat qaBlack[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat qaGreen[] = {0.0, 1.0, 0.0, 1.0};
	GLfloat qaRed[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat qaBlue[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat qaLowAmbient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat qaFullAmbient[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat emission[] = {0.0 ,0.3 ,0.3 ,1.0};
	//Set, ambient, diffuse and specular lighting. Set ambient to 20%.
	//glMaterialfv(GL_FRONT, GL_AMBIENT, qaRed);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, qaRed);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, qaWhite);
	//glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, qaLowAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, qaRed);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, qaRed);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, qaWhite);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, qaLowAmbient);
	glMaterialfv(GL_FRONT_AND_BACK ,GL_EMISSION , emission);
	//glColor4f( color(0),color(1),color(2),color(3) );
	//glNormal3f( normal_(0), normal_(1), normal_(2));
	Vec4	tmp(position_(0), position_(1), position_(2),1.);
	Vec4	point_to_show = adjust_matrix * tmp;
	//glVertex3f( point_to_show(0)+bias(0), point_to_show(1)+bias(1), point_to_show(2)+bias(2) );
	
	

	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glPushMatrix();
	glTranslatef(point_to_show(0)+bias(0),point_to_show(1)+bias(1), point_to_show(2)+bias(2));

	glColor3f( 1.0  ,0.0 ,0.0);
	glutSolidSphere(0.001* Paint_Param::g_point_size, 10, 10);
	glPopMatrix();

	


}