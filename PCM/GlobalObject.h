#pragma  once

class main_window;
class PaintCanvas;
main_window* Global_Window;

PaintCanvas* Global_Canvas;
void setGlobalWindow( main_window* _window);
void setGlobalCanvas( PaintCanvas* _canvas);