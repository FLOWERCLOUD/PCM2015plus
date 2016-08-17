#include "paint_canvas.h"
#include "sample_set.h"
#include "vertex.h"
#include "triangle.h"
#include "main_window.h"
#include "basic_types.h"
#include "tracer.h"
#include "globals.h"
#include "tracer.h"
#include "render_types.h"
#include "savePLYDialog.h"
#include "file_io.h"
#include <fstream>
using namespace qglviewer;
using namespace RenderMode;
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

extern int REAL_TIME_RENDER;

RenderMode::WhichColorMode	which_color_mode_;
RenderMode::RenderType which_render_mode;
bool isShowNoraml = false;
PaintCanvas::PaintCanvas(const QGLFormat& format, QWidget *parent):
	QGLViewer(format, parent),
	coord_system_region_size_(150),	
	single_operate_tool_(nullptr),
	show_trajectory_(false)
{
	which_color_mode_ =VERTEX_COLOR/*SphereMode*/,
	which_render_mode =RenderMode::PointMode,

	fov = 60;
	clipRatioFar = 1;
	clipRatioNear = 1;
	nearPlane = .2f;
	farPlane = 5.f;
	takeSnapTile=false;
	centerframeNum = 0;
	is_key_l_pressed = false;

	main_window_ = (main_window*)parent;

	camera()->setPosition(qglviewer::Vec(1.0,  1.0, 1.0));	

	camera()->lookAt(sceneCenter());
	camera()->setType(Camera::PERSPECTIVE);
	camera()->showEntireScene();
}

void PaintCanvas::draw()
{
	glEnable(GL_MULTISAMPLE);

	//setView();

	if(!takeSnapTile) drawCornerAxis();

	//tool mode
	if (single_operate_tool_!=nullptr && single_operate_tool_->tool_type()!=Tool::EMPTY_TOOL)
	{
		single_operate_tool_->draw();
		return;
	}

	
	SampleSet& set = SampleSet::get_instance();

	if ( !set.empty() )
	{

		glDisable(GL_MULTISAMPLE);
		for (int i = 0; i <set.size(); i++ )
		{
			if(isShowNoraml)
			{
				glEnable(GL_MULTISAMPLE);
				set[i].drawNormal(Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
				glDisable(GL_MULTISAMPLE);
			}
			//if( i == 0 || i > 9)continue;
			LOCK(set[i]);
			switch (which_color_mode_)
			{
			case VERTEX_COLOR:
				 glEnable(GL_MULTISAMPLE);
				set[i].draw(ColorMode::VertexColorMode(),Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
				glDisable(GL_MULTISAMPLE);
				break;
			case OBJECT_COLOR:
				 glEnable(GL_MULTISAMPLE);
				set[i].draw(ColorMode::ObjectColorMode(), 
					Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
				glDisable(GL_MULTISAMPLE);
				break;
			case LABEL_COLOR:
				 glEnable(GL_MULTISAMPLE);			
				set[i].draw(ColorMode::LabelColorMode(),
						 Paint_Param::g_step_size * (ScalarType)(i-centerframeNum) );

				
				glDisable(GL_MULTISAMPLE);
				break;
			case WrapBoxColorMode:   //added by huayun
				if(show_Graph_WrapBox_){
					const Vec3& bias = Paint_Param::g_step_size * (ScalarType)(i-centerframeNum);
					auto camera_look_at = camera()->viewDirection();




					if(!(set[i].is_visible() ) ){
						break;
					}
//					 std::cout<<"i:"<<i<<"size1:"<<set[i].wrap_box_link_.size()<<std::endl;
//					 std::cout<<"i:"<<i<<"size:"<<set[i].wrap_box_link_[0].size()<<std::endl;
					 std::vector<LinkNode>::iterator bitr = set[i].wrap_box_link_[ 0].begin();
					 std::vector<LinkNode>::iterator eitr = set[i].wrap_box_link_[ 0].end();
		
					 glPointSize( Paint_Param::g_point_size);
					 glEnable(GL_POINT_SMOOTH);
					
					 Matrix44 adjust_matrix = set[i].matrix_to_scene_coord();
					 glEnable(GL_MULTISAMPLE);
					
					 int count = 0;
					 Vec3	sum( 0.0 ,0.0 ,0.0 );

					 QFont textFont("Times", (int)Paint_Param::g_point_size, QFont::Bold);

					 for( ; bitr!= eitr;++ bitr , count = count+2){
						 LinkNode& ln = *bitr;

					//	 renderText(axis_size, 0, 0, "X");

						  glBegin(GL_POINTS);
				
						 ColorType color1 ,color2;
						 color1 = Color_Utility::span_color_from_hy_table(ln.labelH_);
						 color2 = Color_Utility::span_color_from_hy_table(ln.labelL_);
					
						 glNormal3f(-camera_look_at.x,-camera_look_at.y,-camera_look_at.z);
						 glColor4f( color1(0)/255.0f,color1(1)/255.0f,color1(2)/255.0f,color1(3) );
						// glNormal3f( normal_(0), normal_(1), normal_(2));
						 Vec4	tmp1(ln.pointH_.x(), ln.pointH_.y(), ln.pointH_.z(),1.);
						 Vec4	point_to_show1 = adjust_matrix * tmp1;
						 Vec4	tmp2(ln.pointL_.x(), ln.pointL_.y(), ln.pointL_.z(),1.);
						 Vec4	point_to_show2 = adjust_matrix * tmp2;
						 glVertex3f( point_to_show1(0)+bias(0), point_to_show1(1)+bias(1), point_to_show1(2)+bias(2) );
						 glColor4f( color2(0)/255.0f,color2(1)/255.0f,color2(2)/255.0f,color2(3) );
						 glVertex3f( point_to_show2(0)+bias(0), point_to_show2(1)+bias(1), point_to_show2(2)+bias(2) );
						 glEnd();

						 glLineWidth( Paint_Param::g_line_size); 
						 glBegin(GL_LINES);
						 glNormal3f(-camera_look_at.x,-camera_look_at.y,-camera_look_at.z);
						 glColor4f( color2(0)/255.0,color2(1)/255.0,color2(2)/255.0,color2(3) );
						 glVertex3f( point_to_show1(0)+bias(0), point_to_show1(1)+bias(1), point_to_show1(2)+bias(2) );
						 glVertex3f( point_to_show2(0)+bias(0), point_to_show2(1)+bias(1), point_to_show2(2)+bias(2) );
						 glEnd();
						 glColor4f( 0.0,0.0, 0.0, 1 );  //绘制全黑
						 Vec4 textofpoint_to_show1,textofpoint_to_show2;
						 textofpoint_to_show1(0) = point_to_show1(0)*1.1;
						 textofpoint_to_show1(1) = point_to_show1(1)*1.1;
						 textofpoint_to_show1(2) = point_to_show1(2)*1.1;
						 textofpoint_to_show2(0) = point_to_show2(0)*1.1;
						 textofpoint_to_show2(1) = point_to_show2(1)*1.1;
						 textofpoint_to_show2(2) = point_to_show2(2)*1.1;

						 renderText( textofpoint_to_show1(0)+bias(0) , textofpoint_to_show1(1)+bias(1), textofpoint_to_show1(2)+bias(2), QString::number(ln.labelH_), textFont);
						 glLineWidth( Paint_Param::g_line_size*0.2); 
						 glBegin(GL_LINES);
						 glNormal3f(-camera_look_at.x,-camera_look_at.y,-camera_look_at.z);
						 glColor4f( color1(0)/255.0,color1(1)/255.0,color1(2)/255.0,color1(3) );
						 glVertex3f( point_to_show1(0)+bias(0), point_to_show1(1)+bias(1), point_to_show1(2)+bias(2) );
						 glVertex3f( textofpoint_to_show1(0)+bias(0), textofpoint_to_show1(1)+bias(1), textofpoint_to_show1(2)+bias(2) );
						 glEnd();

						 glColor4f( 0.0,0.0, 0.0, 1 );  //绘制全黑
						 renderText( textofpoint_to_show2(0)+bias(0), textofpoint_to_show2(1)+bias(1), textofpoint_to_show2(2)+bias(2), QString::number(ln.labelL_),textFont );
						 glLineWidth( Paint_Param::g_line_size*0.2); 
						 glBegin(GL_LINES);
						 glNormal3f(-camera_look_at.x,-camera_look_at.y,-camera_look_at.z);
						 glColor4f( color2(0)/255.0,color2(1)/255.0,color2(2)/255.0,color2(3) );
						 glVertex3f( point_to_show2(0)+bias(0), point_to_show2(1)+bias(1), point_to_show2(2)+bias(2) );
						 glVertex3f( textofpoint_to_show2(0)+bias(0), textofpoint_to_show2(1)+bias(1), textofpoint_to_show2(2)+bias(2) );
						 glEnd();
						 glColor4f( 0.0,0.0, 0.0, 1 );  //绘制全黑

						 sum(0) += point_to_show1(0) + bias(0) + point_to_show2(0) + bias(0);
						 sum(1) += point_to_show1(1) + bias(1) + point_to_show2(1) + bias(1);
						 sum(2) += point_to_show1(0) + bias(2) + point_to_show2(2) + bias(2);
						 
					 }
					 Vec3 center = sum /count;
					 glColor4f( 0.0,0.0, 0.0, 1 );  //绘制全黑

					 renderText( center.x()*1.1 ,  center.y()*1.1,  center.z()*1.1, QString("frame")+QString::number(i) ,textFont );
					 glLineWidth( Paint_Param::g_line_size*0.3); 
					 glBegin(GL_LINES);
					 glNormal3f(-camera_look_at.x,-camera_look_at.y,-camera_look_at.z);
					 glColor4f( 0.0,0.0, 0.0, 1 );  //绘制全黑
					 glVertex3f(  center.x(),  center.y(), center.z() );
					 glVertex3f(  center.x()*1.1,  center.y()*1.1, center.z()*1.1 );
					 glEnd();
					 glColor4f( 0.0,0.0, 0.0, 1 );  //绘制全黑

					 glDisable(GL_MULTISAMPLE);
				 
					 //绘制原的坐标
					
				}
				break;
			case EdgePointColorMode:   //added by huayun
				glEnable(GL_MULTISAMPLE);
				if(show_EdgeVertexs_){

					if(!(set[i].is_visible() ) ){
						break;
					}
					set[i].draw(ColorMode::EdgePointColorMode(),
					Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
				}
				glDisable(GL_MULTISAMPLE);
				break;
			case SphereMode:
			{   //added by huayun	
				glEnable(GL_MULTISAMPLE);
				//if(show_Graph_WrapBox_){

					if(!(set[i].is_visible() ) ){
						break;
					}
					set[i].draw(ColorMode::SphereMode(),
						Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
				//}
				glDisable(GL_MULTISAMPLE);
				break;
			}
			default:
				break;
			}

			switch(which_render_mode)
			{
				case  RenderMode::PointMode:
				{
					glEnable(GL_MULTISAMPLE);
					RenderMode::RenderType rt = RenderMode::PointMode;
					set[i].draw(which_color_mode_,rt ,Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
					glDisable(GL_MULTISAMPLE);
					break;
				}
				case  RenderMode::FlatMode:
				{
					glEnable(GL_MULTISAMPLE);
					RenderMode::RenderType rt = RenderMode::FlatMode;
					set[i].draw(which_color_mode_,rt ,Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
					glDisable(GL_MULTISAMPLE);
					break;
				}
				case  RenderMode::WireMode:
				{
					glEnable(GL_MULTISAMPLE);
					RenderMode::RenderType rt = RenderMode::WireMode;
					set[i].draw(which_color_mode_, rt ,Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
					glDisable(GL_MULTISAMPLE);
					break;
				}
				case RenderMode::FlatWireMode:
				{
						glEnable(GL_MULTISAMPLE);
						RenderMode::RenderType rt = RenderMode::FlatWireMode;
						set[i].draw(which_color_mode_, rt ,Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
						glDisable(GL_MULTISAMPLE);
				}
				case  RenderMode::SmoothMode:
				{
					glEnable(GL_MULTISAMPLE);
					RenderMode::RenderType rt = RenderMode::SmoothMode;
					set[i].draw(which_color_mode_,rt ,Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
					glDisable(GL_MULTISAMPLE);
					break;
				}
				case  RenderMode::TextureMode:
				{
					glEnable(GL_MULTISAMPLE);
					RenderMode::RenderType rt  = RenderMode::TextureMode;
					set[i].draw(which_color_mode_,rt ,Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
					glDisable(GL_MULTISAMPLE);
					break;
				}
				case  RenderMode::SelectMode:
				{
					glEnable(GL_MULTISAMPLE);
					RenderMode::RenderType rt  = RenderMode::SelectMode;
					set[i].draw(which_color_mode_,rt ,Paint_Param::g_step_size * (ScalarType)(i-centerframeNum));
					glDisable(GL_MULTISAMPLE);
					break;
				}
				default:{}
			}

			UNLOCK(set[i]);
		}
		glEnable(GL_MULTISAMPLE);
	}
	//if(takeSnapTile) pasteTile();
	//draw line tracer
	if (show_trajectory_)
	{
		Tracer::get_instance().setcenterframeNum(centerframeNum);
		Tracer::get_instance().draw();
	}
	
	//std::cout<<"updategl end"<<std::endl;





}

void PaintCanvas::init()
{
	setStateFileName("");
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	QColor background_color = Qt::white;
	setBackgroundColor(background_color);

	//setGridIsDrawn();//参考平面2014-12-16
	//camera()->frame()->setSpinningSensitivity(100.f);
	setMouseTracking(true);
	// light0
	GLfloat	light_position[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Setup light parameters
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); /*GL_FALSE*/
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glEnable(GL_LIGHT0);		// Enable Light 0
	glEnable(GL_LIGHTING);

	//////////////////////////////////////////////////////////////////////////

	/* to use facet color, the GL_COLOR_MATERIAL should be enabled */
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	/* to use material color, the GL_COLOR_MATERIAL should be disabled */
	//glDisable(GL_COLOR_MATERIAL);

	//added by huayun
	setGraph_WrapBoxShowOrNot(false);  
	setEdgeVertexsShowOrNot(false);

}


void PaintCanvas::drawCornerAxis()  
{
	int viewport[4];
	int scissor[4];

	glDisable(GL_LIGHTING);

	// The viewport and the scissor are changed to fit the lower left
	// corner. Original values are saved.
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetIntegerv(GL_SCISSOR_BOX, scissor);

	// Axis viewport size, in pixels
	glViewport(0, 0, coord_system_region_size_, coord_system_region_size_);
	glScissor(0, 0, coord_system_region_size_, coord_system_region_size_);

	// The Z-buffer is cleared to make the axis appear over the
	// original image.
	glClear(GL_DEPTH_BUFFER_BIT);

	// Tune for best line rendering
	glLineWidth(5.0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(camera()->orientation().inverse().matrix());

	float axis_size = 1.2f; // other 0.2 space for drawing the x, y, z labels
	drawAxis(axis_size); 

	// Draw text id
	glDisable(GL_LIGHTING);
	glColor3f(0, 0, 0);
	renderText(axis_size, 0, 0, "X");
	renderText(0, axis_size, 0, "Y");
	renderText(0, 0, axis_size, "Z");

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// The viewport and the scissor are restored.
	glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	glEnable(GL_LIGHTING);
}

void PaintCanvas::mousePressEvent(QMouseEvent *e)
{
	if (single_operate_tool_!=nullptr && 
		single_operate_tool_->tool_type()==Tool::SELECT_TOOL)
	{
		if(!(QApplication::keyboardModifiers() == Qt::AltModifier))
		{

			QGLViewer::mousePressEvent(e);
		}
		else
			single_operate_tool_->press(e);
	}
	else
		QGLViewer::mousePressEvent(e);


}

void PaintCanvas::mouseMoveEvent(QMouseEvent *e)
{
	main_window_->showCoordinateAndIndexUnderMouse( e->pos() );

	if (single_operate_tool_!=nullptr && 
		single_operate_tool_->tool_type()==Tool::SELECT_TOOL)
	{

		if(!(QApplication::keyboardModifiers() == Qt::AltModifier))
		{

			QGLViewer::mouseMoveEvent(e);
		}else
			single_operate_tool_->move(e);
	}
	else
		QGLViewer::mouseMoveEvent(e);


}

void PaintCanvas::mouseReleaseEvent(QMouseEvent *e)
{
	if (single_operate_tool_!=nullptr && 
		single_operate_tool_->tool_type()==Tool::SELECT_TOOL
		)
	{
		if(!(QApplication::keyboardModifiers() == Qt::AltModifier))
		{

			QGLViewer::mouseReleaseEvent(e);
		}else
			single_operate_tool_->release(e);
	}
	else
		QGLViewer::mouseReleaseEvent(e);


}

void PaintCanvas::wheelEvent(QWheelEvent *e)
{

	////added by huayun
	////if (  e->orientation() == Qt::Horizontal){
	//	float numDegrees = e->delta() / 120;//滚动的角度，*8就是鼠标滚动的距离
	//	//int numSteps = numDegrees / 10;//滚动的步数，*15就是鼠标滚动的角度
	//	Paint_Param::g_point_size += numDegrees*0.1;
	//	if (Paint_Param::g_point_size < 0.)
	//	{
	//		Paint_Param::g_point_size = 0.;
	//	}
	//	if (Paint_Param::g_point_size  > 100.)
	//	{
	//		Paint_Param::g_point_size = 100.;
	//	}
	//	


	//	updateGL();
	//}

	//std::cout<<Qt::ControlModifier<<std::endl;
	if (e->modifiers() == Qt::ControlModifier)
	{
		int numDegrees = e->delta() / 120;

		Paint_Param::g_point_size += 1.f * numDegrees;
		if (Paint_Param::g_point_size < 0.)
		{
			Paint_Param::g_point_size = 0.;
		}

		updateGL();
	}
	

	if (e->modifiers() == Qt::AltModifier )
	{
		int numDegrees = e->delta() / 120;

		Paint_Param::g_step_size(2) += 0.1f * numDegrees;
	

		updateGL();
	}
	else if( e->modifiers() == (Qt::AltModifier|Qt::ControlModifier) )
	{
		int numDegrees = e->delta() / 120;

		Paint_Param::g_step_size(1) += 0.1f * numDegrees;


		updateGL();
	}
	else if( e->modifiers() == (Qt::AltModifier|Qt::ControlModifier|Qt::ShiftModifier) )  
	{
		int numDegrees = e->delta() / 120;

		Paint_Param::g_step_size(0) += 0.1f * numDegrees;


		updateGL();
	}
	//change line size
	if (is_key_l_pressed)
	{
		
		int numDegrees = e->delta() / 120;
		
		Paint_Param::g_line_size += 1.f * numDegrees;
		std::cout<<Paint_Param::g_line_size<<std::endl;
		if (Paint_Param::g_line_size < 0.)
		{
			Paint_Param::g_line_size = 0.1;
		}

		updateGL();
		return;
	}
	QGLViewer::wheelEvent(e);
}

//void MyWidget::wheelEvent(QWheelEvent *event)
//{
//	int numDegrees = event->delta() / 8;//滚动的角度，*8就是鼠标滚动的距离
//	int numSteps = numDegrees / 15;//滚动的步数，*15就是鼠标滚动的角度
//	if (event->orientation() == Qt::Horizontal) {       
//		scrollHorizontally(numSteps);       //水平滚动
//	} else {
//		scrollVertically(numSteps);       //垂直滚动
//	}
//	event->accept();      //接收该事件
//}


void PaintCanvas::keyPressEvent(QKeyEvent * e)
{
	if ( e->key() ==Qt::Key_Delete )
	{
		if (single_operate_tool_!=nullptr && 
			single_operate_tool_->tool_type()==Tool::SELECT_TOOL
			)
		{
			SelectTool*	select_tool = dynamic_cast<SelectTool*>(single_operate_tool_);

			const std::vector<IndexType>& selected_items =  select_tool->get_selected_vertex_idx();
			
			IndexType cur_selected_sample_idx = select_tool->cur_sample_to_operate();
			Sample& smp = SampleSet::get_instance()[cur_selected_sample_idx];

			smp.lock();
			smp.delete_vertex_group( selected_items);
			smp.unlock();
			
			//reset tree widget
			main_window_->createTreeWidgetItems();
			updateGL();
		}
	}
	if( e->key() == Qt::Key_Right || e->key() == Qt::Key_Left ||e->key() == Qt::Key_Up ||e->key() == Qt::Key_Down)
	{
		SampleSet& ss = SampleSet::get_instance();
		std::vector<IndexType> visbleindex;
		for( int i = 0 ;i < ss.size() ; ++i)
		{
			if(ss[i].is_visible())visbleindex.push_back(i);
		}
		IndexType max_index ;
		IndexType next_index;
		switch (e->key())
		{

		case Qt::Key_Right :
			{
				if(!visbleindex.size())return;
				else max_index = visbleindex[visbleindex.size() -1];
				if( max_index + 1 < ss.size())
				{
					next_index = max_index + 1;
				}else next_index = 0;
				ss[max_index].set_visble(false);
				ss[next_index].set_visble(true);
			}

			break ;
		case Qt::Key_Up :
			{
				if(!visbleindex.size())return;
				else max_index = visbleindex[0];
				if( max_index  > 0)
				{
					next_index = max_index - 1;
				}else next_index = ss.size() -1;
				ss[max_index].set_visble(false);
				ss[next_index].set_visble(true);
			}

			break ;
		case Qt::Key_Left :
			{
				if(!visbleindex.size())return;
				else max_index = visbleindex[0];
				if( max_index  > 0)
				{
					next_index = max_index - 1;
				}else next_index = ss.size() -1;
				ss[max_index].set_visble(false);
				ss[next_index].set_visble(true);
			}

			break ;
		case Qt::Key_Down :
			{
				if(!visbleindex.size())return;
				else max_index = visbleindex[visbleindex.size() -1];
				if( max_index + 1 < ss.size())
				{
					next_index = max_index + 1;
				}else next_index = 0;
				ss[max_index].set_visble(false);
				ss[next_index].set_visble(true);
			}
			break ;
		default :
			break ;
		}
		updateGL();	
		main_window_->getLayerdialog()->updateTable(0);
	}
	
	if (single_operate_tool_!=nullptr && 
		single_operate_tool_->tool_type()==Tool::SELECT_TOOL
		)
	{
		IndexType target_label = 0;
		switch(e->key() )
		{
			case Qt::Key_0 :
				{
					target_label = 0;
					Logger<<"0";
					break;
				}
			case Qt::Key_1 :
				{
					target_label = 1;
					break;
				}
			case Qt::Key_2 :
				{
					target_label = 2;
					break;

				}
			case Qt::Key_3 :
				{
					target_label = 3;
					break;
				}
			case Qt::Key_4 :
				{
					target_label = 4;
					break;
				}
			case Qt::Key_5:
				{
					target_label = 5;
					break;
				}
			case Qt::Key_6 :
				{
					target_label = 6;
					break;
				}
			case Qt::Key_7 :
				{
					target_label = 7;
					break;
				}
			case Qt::Key_8 :
				{
					target_label = 8;
					break;
				}
			case Qt::Key_9 :
				{
					target_label = 9;
					break;
				}
			case Qt::Key_A :
				{
					target_label = 10;
					Logger<<"A";
					break;
				}
			case Qt::Key_B :
				{
					target_label = 11;
					break;
				}
			case Qt::Key_C :
				{
					target_label = 12;
					break;
				}
			case Qt::Key_D :
				{
					target_label = 13;
					break;
				}
			case Qt::Key_E:
				{
					target_label = 14;
					break;
				}
			case Qt::Key_F :
				{
					target_label = 15;
					break;
				}
			default:
				{
					target_label = 0;
				}
		}
		SelectTool*	select_tool = dynamic_cast<SelectTool*>(single_operate_tool_);

		const std::vector<IndexType>& selected_items =  select_tool->get_selected_vertex_idx();

		IndexType cur_selected_sample_idx = select_tool->cur_sample_to_operate();
		Sample& smp = SampleSet::get_instance()[cur_selected_sample_idx];

		smp.lock();
		Logger<<"selected "<<selected_items.size()<<"  "<<" label"<<target_label;
		smp.set_vertex_label(selected_items ,target_label);
		smp.unlock();

		//reset tree widget
		main_window_->createTreeWidgetItems();
		updateGL();	

	}

	
	if ( e->key() == Qt::Key_L &&!e->isAutoRepeat())
	{
		
		std::cout<<"key_l_pressed"<<std::endl;
		is_key_l_pressed = true;
	}
}
void PaintCanvas::keyReleaseEvent(QKeyEvent * e)
{
	//if(e->isAutoRepeat())
	//{
	//	e->ignore();
	//	return;
	//}
	if ( e->key() == Qt::Key_L&& !e->isAutoRepeat())
	{
		std::cout<<"key_l_release"<<std::endl;
		is_key_l_pressed = false;
	}
}

void PaintCanvas::showSelectedTraj()
{
	if (single_operate_tool_!=nullptr /*&& 
									  Register_Param::g_is_traj_compute == true*/)
	{
		SelectTool*	select_tool = dynamic_cast<SelectTool*>(single_operate_tool_);
		const std::vector<IndexType>& selected_items =  select_tool->get_selected_vertex_idx();

		Tracer& tracer = Tracer::get_instance();
		tracer.clear_records();

		IndexType m = (SampleSet::get_instance()).size(); 
		IndexType n = (SampleSet::get_instance()[0]).num_vertices();
		MatrixXXi mat( n,m );
		for (IndexType i =0; i < m; i++)
		{
			for (IndexType j=0; j<n; j++)
			{
				mat(j,i) = j;
			}
		}

		Register_Param::g_traj_matrix = mat;

		for ( IndexType i=0; i<selected_items.size(); i++ )
		{
			IndexType selected_idx = selected_items[i];
			
			auto traj = Register_Param::g_traj_matrix.row(selected_idx);
			IndexType traj_num = Register_Param::g_traj_matrix.cols() -1;

			for ( IndexType j=0; j<traj_num; j++ )
			{
				tracer.add_record(  j, traj(j) , j+1, traj(j+1) );
			}
		}
	}
	this->show_trajectory_ = true;
}
void PaintCanvas::showSelectedlabelTraj(std::vector<int>& _selectedlabeltraj)
{
	/*Tracer& tracer = Tracer::get_instance();
	tracer.clear_records();

	IndexType m = (SampleSet::get_instance()).size(); 
	IndexType n = (SampleSet::get_instance()[0]).num_vertices();

	for ( IndexType i=0; i<selected_items.size(); i++ )
	{
	IndexType selected_idx = selected_items[i];

	for ( IndexType j=0; j<traj_num; j++ )
	{
	tracer.add_record(  j, traj(j) , j+1, traj(j+1) );
	}
	}*/

}

void PaintCanvas::pasteTile()
{
	QString outfile;

	glPushAttrib(GL_ENABLE_BIT);
	QImage tileBuffer=grabFrameBuffer(true).mirrored(false,true);
	if(ss.tiledSave)
	{
		outfile=QString("%1/%2_%3-%4.png")
			.arg(ss.outdir)
			.arg(ss.basename)
			.arg(tileCol,2,10,QChar('0'))
			.arg(tileRow,2,10,QChar('0'));
		tileBuffer.mirrored(false,true).save(outfile,"PNG");
	}
	else
	{
		if (snapBuffer.isNull())
			snapBuffer = QImage(tileBuffer.width() * ss.resolution, tileBuffer.height() * ss.resolution, tileBuffer.format());

		uchar *snapPtr = snapBuffer.bits() + (tileBuffer.bytesPerLine() * tileCol) + ((totalCols * tileRow) * tileBuffer.byteCount());
		uchar *tilePtr = tileBuffer.bits();

		for (int y=0; y < tileBuffer.height(); y++)
		{
			memcpy((void*) snapPtr, (void*) tilePtr, tileBuffer.bytesPerLine());
			snapPtr+=tileBuffer.bytesPerLine() * totalCols;
			tilePtr+=tileBuffer.bytesPerLine();
		}
	}
	tileCol++;
	if (tileCol >= totalCols)
	{
		tileCol=0;
		tileRow++;

		if (tileRow >= totalRows)
		{
			if(ss.snapAllLayers)
			{
				outfile=QString("%1/%2%3_L%4.png")
					.arg(ss.outdir).arg(ss.basename)
					.arg(ss.counter,2,10,QChar('0'))
					.arg(currSnapLayer,2,10,QChar('0'));
			} else {
				outfile=QString("%1/%2%3.png")
					.arg(ss.outdir).arg(ss.basename)
					.arg(ss.counter++,2,10,QChar('0'));
			}

			if(!ss.tiledSave)
			{
				bool ret = (snapBuffer.mirrored(false,true)).save(outfile,"PNG");
				if (ret)
				{
					/*this->Logf(GLLogStream::SYSTEM, "Snapshot saved to %s",outfile.toLocal8Bit().constData());*/
				}
				else
				{
					/*Logf(GLLogStream::WARNING,"Error saving %s",outfile.toLocal8Bit().constData());*/
				}
			}
			takeSnapTile=false;
			snapBuffer=QImage();
		}
	}
	updateGL();
	glPopAttrib();
	
}

void PaintCanvas::saveSnapshot()
{
	//snap all layers
	currSnapLayer = 0;
	//number of subparts
	totalCols = totalRows = ss.resolution;
	tileRow = tileCol = 0;
	if(ss.snapAllLayers)
	{
		while(currSnapLayer < SampleSet::get_instance().size())
		{
			tileRow = tileCol = 0;
			//SET CURRMESH()

		}
	}else
	{
		takeSnapTile = true;
		saveSnapshotImp(ss);
		updateGL();
	}
}
void PaintCanvas::saveSnapshotImp(SnapshotSetting& _ss)
{


	//glPushAttrib(GL_ENABLE_BIT);
	QImage tileBuffer;
	tileBuffer =grabFrameBuffer(true).mirrored(false,true);
	glPushAttrib(GL_ENABLE_BIT);
	totalCols = totalRows = _ss.resolution;
	tileRow = tileCol = 0;
	QString outfile;


	//if (snapBuffer.isNull())
		snapBuffer = QImage( tileBuffer.width() * _ss.resolution, tileBuffer.height() * _ss.resolution, tileBuffer.format());

	
		for( int tileRow = 0 ; tileRow < totalRows ; ++tileRow)
		{
			for( int tileCol = 0 ; tileCol < totalCols ;++tileCol)
			{
				preDraw();
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				setTileView( totalCols ,totalRows ,tileCol ,tileRow);
				glMatrixMode(GL_MODELVIEW);
				draw();
				postDraw();
				tileBuffer =grabFrameBuffer(true).mirrored(false,true);

				uchar *snapPtr = snapBuffer.bits() + (tileBuffer.bytesPerLine() * tileCol) + ((totalCols * tileRow) * tileBuffer.byteCount());
				uchar *tilePtr = tileBuffer.bits();

				for (int y=0; y < tileBuffer.height(); y++)
				{
					memcpy((void*) snapPtr, (void*) tilePtr, tileBuffer.bytesPerLine());
					snapPtr+=tileBuffer.bytesPerLine() * totalCols;
					tilePtr+=tileBuffer.bytesPerLine();
				}
			
			}

		}

	outfile=QString("%1/%2%3.png")
		.arg(_ss.outdir).arg(_ss.basename)
		.arg(_ss.counter++,2,10,QChar('0'));
	bool ret = (snapBuffer.mirrored(false,true)).save(outfile,"PNG");
	takeSnapTile = false;
	updateGL();
	glPopAttrib();


}


void PaintCanvas::setTileView( IndexType totalCols  , IndexType totalRows ,IndexType tileCol ,IndexType tileRow )
{
	glViewport(0 ,0 , this->width() ,this->height());
	GLfloat fApspect = (GLfloat)width()/height();
	nearPlane =  camera()->zNear();
	farPlane = camera()->zFar();
	
	ScalarType deltaY = 2*nearPlane * tan(camera()->fieldOfView() / 2.0) /totalRows;
	ScalarType deltaX = deltaY* fApspect;
	//ScalarType xMin = -this->width()/2.0;
	//ScalarType yMin = -this->height()/2.0;
	ScalarType yMin = - nearPlane * tan(camera()->fieldOfView() / 2.0);
	ScalarType xMin =  yMin* fApspect;



	if (camera()->type() == qglviewer::Camera::PERSPECTIVE)
		glFrustum(xMin + tileCol*deltaX, xMin + (tileCol+1)*deltaX, yMin + (tileRow)*deltaY, yMin + (tileRow+1)*deltaY, nearPlane, farPlane);
	else glOrtho(xMin + tileCol*deltaX, xMin + (tileCol+1)*deltaX, yMin + (tileRow)*deltaY, yMin + (tileRow+1)*deltaY, nearPlane, farPlane);
	
}
void PaintCanvas::setView()
{
	glViewport(0 ,0 , this->width() ,this->height());
	GLfloat fApspect = (GLfloat)width()/height();
	glMatrixMode(GL_PROJECTION);
	/*glLoadIdentity();*/

	float viewRatio  =1.75f;
	float cameraDist = viewRatio /tanf( (float)PI * (fov* .5f) /180.0f);
	if(fov <5)cameraDist =1000;  //small hack for othographic projection where camera distance is rather meaningless..
	nearPlane = cameraDist - 2.f* clipRatioNear;
	farPlane = cameraDist + 10.f* clipRatioFar;
	if(nearPlane <cameraDist*.1f)nearPlane = cameraDist*.1f;
	if(!takeSnapTile)
	{
		if(fov ==5) glOrtho( -viewRatio*fApspect ,viewRatio* fApspect , -viewRatio ,viewRatio,cameraDist-2.f*clipRatioNear ,cameraDist+2.f*clipRatioFar);
		/*else gluPerspective(fov , fApspect , nearPlane ,farPlane);*/
		else 
		{
			/*camera()->lookAt( qglviewer::Vec())*/
		}

	}
	else setTiledView( fov, viewRatio , fApspect ,nearPlane ,farPlane , cameraDist);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity;
	//gluLookAt(0 ,0 , cameraDist , 0 ,0 ,0 ,0 ,1 ,0);

}

void PaintCanvas::setTiledView(GLdouble fovY , float viewRatio , float fAspect , GLdouble zNear ,GLdouble zFar , float cameraDist)
{
	if(fovY <=5)
	{
		/*GLdouble fLeft = -viewRatio*fAspect;
		GLdouble fright = viewRatio*fAspect;
		GLdouble fboo*/

	}else
	{
		GLdouble fTop = zNear * tan((float)PI * (fov* .5f) /180.0f);
		GLdouble fRight = fTop* fAspect;
		GLdouble fBottom = -fTop;
		GLdouble fLeft = - fRight;
		//tile Dimension
		GLdouble tDimX = fabs( fRight -fLeft)/ totalCols;
		GLdouble tDimY = fabs(fTop - fBottom)/ totalRows;
	/*	glFrustum( fLeft + tDimX * tileCol ,fLeft + tDimX*(tileCol+1), 
			fBottom + tDimY * tileRow , fBottom + tDimY * (tileRow+1) , zNear ,zFar);*/

	}
}

void PaintCanvas::Logf(int level ,const char* f){}

void PaintCanvas::savePLY(SavePlySetting& ss)
{
	char fullPath[250];
	char format[30];
	FileIO::FILE_TYPE ftype;
	if( "ply" == ss.format)
	{
		ftype = FileIO::PLY;
	}else if( "obj" == ss.format)
	{
		ftype = FileIO::OBJ;
	}else if( "off" == ss.format)
	{
		ftype = FileIO::OFF;
	}else
	{

	}
	for( int i = ss.startframe ; i <= ss.endframe;++i)
	{
		sprintf( fullPath ,"%s%s%s%.3d%s",ss.outdir.toStdString().c_str() ,"/",ss.basename.toStdString().c_str(), i ,".ply");  
		FileIO::saveFile(std::string(fullPath),ftype , i);
	}
	
	

	//QString outfile;
	/*outfile=QString("%1/%2%3.png")
	.arg(ss.outdir).arg(ss.basename)
	.arg(ss.counter++,2,10,QChar('0'));*/
	//char* output_file_path_ = (char*)outfile.toStdString().c_str();
	
	

}
void PaintCanvas::saveLabelFile(std::string filename ,IndexType selected_frame_idx)
{
	if(selected_frame_idx <0||selected_frame_idx +1>SampleSet::get_instance().size())return;
	//char fullPath[250];
	//sprintf( fullPath ,"%s","./squat2_edit.seg");     //必须加入.3d ，使得文件排序正常
	std::ofstream outfile( filename , std::ofstream::out);
	SampleSet& smpset =  SampleSet::get_instance();
	smpset[selected_frame_idx].set_visble(true);
	for( auto  vtxbitr = smpset[selected_frame_idx].begin() ; vtxbitr != smpset[selected_frame_idx].end() ;++vtxbitr ){
		Vertex& vtx = **vtxbitr;
		outfile<<vtx.label()<<std::endl;

	}

	outfile.close();

}
void PaintCanvas::getLabelFromFile(std::string filename ,IndexType selected_frame_idx)
{
	if(selected_frame_idx <0||selected_frame_idx +1>SampleSet::get_instance().size())return;
	//char fullPath[250];
	//sprintf( fullPath ,"%s","D:/zzb/zzb_lowrank/new_STED/original/squat2/11label/squat2_edit.seg");     //必须加入.3d ，使得文件排序正常
	std::ifstream infile( filename , std::ofstream::in);
	SampleSet& smpset =  SampleSet::get_instance();
	smpset[selected_frame_idx].set_visble(true);
	for( auto  vtxbitr = smpset[selected_frame_idx].begin() ; vtxbitr != smpset[selected_frame_idx].end() ;++vtxbitr ){
		int label = 0;
		infile>>label;
		Vertex& vtx = **vtxbitr;
		vtx.set_label(label);

	}

	infile.close();
}