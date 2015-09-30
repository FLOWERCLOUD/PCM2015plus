#pragma  once

#include "main_window.h"

main_window* Global_Window;

PaintCanvas* Global_Canvas;
void setGlobalWindow( main_window* _window){
	Global_Window = _window;
}
void setGlobalCanvas( PaintCanvas* _canvas){
	Global_Canvas = _canvas;
}