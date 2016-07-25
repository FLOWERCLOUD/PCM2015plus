#include "triangle.h"
#include "vertex.h"
#include "render_types.h"
#include <gl/gl.h>
#include "sample.h"
TriangleType::TriangleType(Sample& s):sample_(s)
{
	i_vertex[0] =i_vertex[1] = i_vertex[2] = 0;
	i_norm[0]= i_norm[1] = i_norm[2] = 0;
}
TriangleType::TriangleType(const TriangleType& s):sample_(s.sample_)
{
	for(int i = 0 ;i<3;++i)
	{
		this->i_vertex[i] = s.i_vertex[i];
		this->i_norm[i]= s.i_norm[i];
	}
	//Logger<<"TriangleType copy"<<std::endl;
	
}
TriangleType& TriangleType::operator=(const TriangleType& s)
{
	for(int i = 0 ;i<3;++i)
	{
		this->i_vertex[i] = s.i_vertex[i];
		this->i_norm[i]= s.i_norm[i];
	}
	//Logger<<"TriangleType assignment"<<std::endl;
	return *this;
}

void TriangleType::draw(RenderMode::WhichColorMode& wcm, RenderMode::RenderType& r,const Matrix44& adjust_matrix, const Vec3& bias)
{
	if (!visible_)
	{
		return;
	}
	std::vector<Vertex*>& vs = sample_.vertices_;
	r = RenderMode::SolidMode;
	switch(r)
	{
		case RenderMode::PointMode:{
			
			break;
		}
	case RenderMode::SolidMode:{

		glBegin(GL_TRIANGLES);
		for( int i = 0 ;i <3 ;++i)
		{
			if(this->i_norm[i]!=-1)
			{
				Vec4	tmpn( vs[this->i_norm[i]]->nx()  ,  vs[this->i_norm[i]]->ny() ,vs[this->i_norm[i]]->nz() ,1.); 
				Vec4	normal_to_show = tmpn;//adjust_matrix * tmpn;


				glNormal3f( 
					normal_to_show(0),
					normal_to_show(1),
					normal_to_show(2));
				//Logger<<"NORMAL: "<<(float)vs[this->i_norm[i]]->nx()<<" "<<
				//	(float)vs[this->i_norm[i]]->ny()<<" "<<
				//	(float)vs[this->i_norm[i]]->nz()<<std::endl;

			}
			glColor3f(	0.5f,0.5f,0.5f
				/*				(GLfloat) vs[this->i_vertex[i]]->r(),
				(GLfloat) vs[this->i_vertex[i]]->g(),
				(GLfloat) vs[this->i_vertex[i]]->b()*/ );
			//Logger<<"COLOR: "<<vs[this->i_vertex[i]]->r()<<" "<<
			//	vs[this->i_vertex[i]]->g()<<" "<<
			//	vs[this->i_vertex[i]]->b()<<std::endl;
			Vec4	tmpv( vs[this->i_vertex[i]]->x()  ,  vs[this->i_vertex[i]]->y() ,vs[this->i_vertex[i]]->z() ,1.);
			Vec4	point_to_show = adjust_matrix * tmpv;
			glVertex3f(	
				point_to_show(0)+ bias(0),
				point_to_show(1)+ bias(1),
				point_to_show(2)+ bias(2) );
			//Logger<<"VERTEX: "<<vs[this->i_vertex[i]]->x()<<" "<<
			//	vs[this->i_vertex[i]]->y()<<" "<<
			//	vs[this->i_vertex[i]]->z()<<std::endl;

		}
		glEnd();
		break;
							   }
	case RenderMode::WireMode:{
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		for( int i = 0 ;i <3 ;++i)
		{
			if(this->i_norm[i]!=-1)
			{
				Vec4	tmpn( vs[this->i_norm[i]]->nx()  ,  vs[this->i_norm[i]]->ny() ,vs[this->i_norm[i]]->nz() ,1.);
				Vec4	normal_to_show = adjust_matrix * tmpn;


				glNormal3f( 
				normal_to_show(0),
				normal_to_show(1),
				normal_to_show(2));
			}
			glColor3f(	0.5f,0.5f,0.5f
				/*(GLfloat) vs[this->i_vertex[i]]->r(),
				(GLfloat) vs[this->i_vertex[i]]->g(),
				(GLfloat) vs[this->i_vertex[i]]->b() */);
			Vec4	tmpv( vs[this->i_vertex[i]]->x()  ,  vs[this->i_vertex[i]]->y() ,vs[this->i_vertex[i]]->z() ,1.);
			Vec4	point_to_show = adjust_matrix * tmpv;
			glVertex3f(	
				point_to_show(0)+ bias(0),
				point_to_show(1)+ bias(1),
				point_to_show(2)+ bias(2) );

		}
		glEnd();

		break;
							  }
	case RenderMode::SmoothMode:{			break;}
	case RenderMode::TextureMode:{			break;}
	case RenderMode::SelectMode:{			break;}	
	default:{}
	}
	
}




