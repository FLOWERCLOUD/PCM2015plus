#include "GlobalObject.h"
#include "main_window.h"
#include "paint_canvas.h"



void setGlobalWindow(main_window* _window)
{
	Global_Window = _window;
}

void setGlobalCanvas(PaintCanvas* _canvas)
{
	Global_Canvas = _canvas;
}
