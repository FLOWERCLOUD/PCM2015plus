#include "select_tool.h"
#include "sample_set.h"
#include "vertex.h"
#include "windows.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "paint_canvas.h"
#include "globals.h"
#include "color_table.h"
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
void SelectTool::move(QMouseEvent *e)
{
	if (left_mouse_button_ == true)
	{
			drag(e);
	}

}

void SelectTool::drag(QMouseEvent *e)
{

	rectangle_.setBottomRight( e->pos() );
	canvas_->updateGL();

}

void SelectTool::release(QMouseEvent *e)
{
	if (left_mouse_button_ == true)
	{
		// Possibly swap left/right and top/bottom to make rectangle_ valid.
		rectangle_ = rectangle_.normalized();

		select();

		rectangle_ = QRect(e->pos(), e->pos());
		canvas_->updateGL();
		left_mouse_button_ = false;
	}

}

void SelectTool::press(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		left_mouse_button_ = true;
		rectangle_ = QRect(e->pos(), e->pos());
		canvas_->updateGL();
	}

}

void SelectTool::draw()
{
	draw_rectangle();
	
	//draw hightlight vertex
	Sample& sample = SampleSet::get_instance()[cur_sample_to_operate_];
	LOCK(sample);
	Matrix44 adjust_mat = sample.matrix_to_scene_coord();
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glPointSize(Paint_Param::g_point_size);
	glBegin(GL_POINTS);
	for ( IndexType v_idx = 0; v_idx < sample.num_vertices(); v_idx++ )
	{
		ColorType c;
		if (sample[v_idx].is_selected() == true )
		{
			c = SELECTED_COLOR;
		}
		else
		{
			ColorType color2 = Color_Utility::span_color_from_table( sample[v_idx].label()); 
			c = color2;
		}
		
		glColor4f(  c(0), c(1), c(2),c(3) );
		sample[v_idx].draw_without_color(adjust_mat);
	}
	glEnd();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_MULTISAMPLE);
	UNLOCK(sample);

}

void SelectTool::draw_rectangle()
{
	canvas_->startScreenCoordinatesSystem();

	glDisable(GL_LIGHTING);

	glLineWidth(2.0);
	glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(rectangle_.left(),  rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.bottom());
	glVertex2i(rectangle_.left(),  rectangle_.bottom());
	glEnd();	

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glColor4f(0.0, 0.0, 0.4f, 0.3f);
	glBegin(GL_QUADS);
	glVertex2i(rectangle_.left(),  rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.top());
	glVertex2i(rectangle_.right(), rectangle_.bottom());
	glVertex2i(rectangle_.left(),  rectangle_.bottom());
	glEnd();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glEnable(GL_LIGHTING);
	canvas_->stopScreenCoordinatesSystem();
}

void SelectTool::begin_select()
{
	
	canvas_->makeCurrent();
	initialize_select_buffer();

	if ( select_buffer_==nullptr )
	{
		return;
	}

	glSelectBuffer( select_buffer_size_, select_buffer_ );
	glRenderMode(GL_SELECT);
	glInitNames();

	// Loads the matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	static GLint viewport[4];
	canvas_->camera()->getViewport(viewport);
	gluPickMatrix(rectangle_.center().x(), rectangle_.center().y(), 
		rectangle_.width(), rectangle_.height(), viewport);

	// loadProjectionMatrix() first resets the GL_PROJECTION matrix with a glLoadIdentity().
	// The false parameter prevents this and hence multiplies the matrices.
	canvas_->camera()->loadProjectionMatrix(false);
	// Reset the original (world coordinates) modelview matrix
	canvas_->camera()->loadModelViewMatrix();
}

void SelectTool::end_select()
{
	
	glFlush();
	GLint nbHits = glRenderMode(GL_RENDER);

	SampleSet& set = SampleSet::get_instance();
	
	//reset selected vertex
	Sample& smp = set[cur_sample_to_operate_];
	LOCK(smp);
	for (IndexType i = 0; i < smp.num_vertices(); i++)
	{
		smp[i].set_selected(false);
	}

	UNLOCK(smp);

	// Interpret results : each object created 4 values in the selectBuffer().
	// (selectBuffer())[4*i+3] is the id pushed on the stack.
	selected_vertex_indices_.clear();
	for (int i=0; i<nbHits; ++i)
		selected_vertex_indices_.push_back(select_buffer_[4*i+3]);
	
	std::sort( selected_vertex_indices_.begin(), selected_vertex_indices_.end() );

	LOCK(set[cur_sample_to_operate_]);
		
		for (IndexType i = 0; i < selected_vertex_indices_.size(); i++)
		{
			set(cur_sample_to_operate_, selected_vertex_indices_[i]).set_selected(true);
		}

	UNLOCK(set[cur_sample_to_operate_]);


	delete [] select_buffer_;
	select_buffer_ = nullptr;
	select_buffer_size_ = 0;
	return;
}

void SelectTool::select()
{
	begin_select();
	
	SampleSet& set = SampleSet::get_instance();
	LOCK(set[cur_sample_to_operate_]);
	
		set[cur_sample_to_operate_].draw_with_name();	

	UNLOCK(set[cur_sample_to_operate_]);
	
	end_select();
}



void SelectTool::initialize_select_buffer()
{
	if ( select_buffer_!=nullptr )
	{
		delete [] select_buffer_;
	}

	SampleSet& set = SampleSet::get_instance();
	LOCK(set[cur_sample_to_operate_]);
		select_buffer_size_ = (set[cur_sample_to_operate_].num_vertices())*4;
	UNLOCK(set[cur_sample_to_operate_]);


	select_buffer_ = new unsigned int[select_buffer_size_];
}