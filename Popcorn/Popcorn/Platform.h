#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
class APlatform
{
public:
	APlatform();

	void Init();
	void Redraw(HWND hwnd);
	void Draw(HDC hdc, RECT &paint_area, HPEN bg_pen, HBRUSH bg_brush);

	int X_Pos;
	int Width;
	int X_Step;

private:
	HPEN Highlight_Pen, Platform_Ball_Pen, Platform_Inner_Pen;
	HBRUSH Platform_Ball_Brush, Platform_Inner_Brush;

	RECT Platform_Rect, Prev_Platform_Rect;

	int Inner_Width;

	static const int Height = 7;
	static const int Circle_Size = 7;
};
//------------------------------------------------------------------------------------------------------------
