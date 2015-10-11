#ifndef _SELECT_TOOL_H
#define _SELECT_TOOL_H
#include "tool.h"
#include "basic_types.h"
#include <vector>

class PaintCanvas;

/*	Rectangle Select Tool	*/
class SelectTool : public Tool
{
public:


	SelectTool(PaintCanvas* canvas):Tool(canvas),
							select_buffer_(nullptr),
							select_buffer_size_(0){}
	~SelectTool(){}

	virtual void move(QMouseEvent *e);
	virtual void drag(QMouseEvent *e);
	virtual void release(QMouseEvent *e);
	virtual void press(QMouseEvent* e);
	virtual void draw();
	
	const std::vector<IndexType>& get_selected_vertex_idx()
	{
		return selected_vertex_indices_;
	}


protected:
	inline void select();
	inline void begin_select();
	inline void end_select();

private:
	void draw_rectangle();
	inline void initialize_select_buffer();

private:

	QPoint	mouse_pressed_pos_;
	QPoint	mouse_move_pos_;

	std::vector<IndexType> selected_vertex_indices_;

	int		select_buffer_size_;
	unsigned int*	select_buffer_;


	QRect	rectangle_;


};


#endif