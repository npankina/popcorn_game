#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum EPlatform_State
{
	EPS_Missing,
	EPS_Normal,
	EPS_Meltdown
};
//------------------------------------------------------------------------------------------------------------
class APlatform
{
public:
	APlatform();

	void Init();
	void Redraw(HWND hwnd);
	void Act(HWND hwnd);
	void Draw(HDC hdc, RECT &paint_area);
	

	int X_Pos;
	int X_Step;
	int Width;


private:
	void Draw_Normal_State(HDC hdc, RECT &paint_area);
	void Draw_Meltdown_State(HDC hdc, RECT &paint_area);

	EPlatform_State Platform_State;
	HPEN Highlight_Pen, Platform_Ball_Pen, Platform_Inner_Pen;
	HBRUSH Platform_Ball_Brush, Platform_Inner_Brush;

	RECT Platform_Rect, Prev_Platform_Rect;

	static const int Normal_Width = 28;

	int Inner_Width;
	int Meltdown_Platform_Y_Pos[Normal_Width * AsConfig::Global_Scale];


	static const int Height = 7;
	static const int Circle_Size = 7;
	static const int Meltdown_Speed = 3;
};
//------------------------------------------------------------------------------------------------------------
