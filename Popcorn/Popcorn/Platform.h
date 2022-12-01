#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum EPlatform_State
{
	EPS_Missing,
	EPS_Normal,
	EPS_Meltdown,
	EPS_Roll_In,
	EPS_Expand_Roll_In
};
//------------------------------------------------------------------------------------------------------------
class APlatform
{
public:
	APlatform();

	void Init();
	void Redraw_Platform();
	void Act();
	void Set_State(EPlatform_State new_state);
	void Draw(HDC hdc, RECT &paint_area);
	

	int X_Pos;
	int X_Step;
	int Width;


private:
	void Clear_BG(HDC hdc);
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Draw_Normal_State(HDC hdc, RECT &paint_area);
	void Draw_Meltdown_State(HDC hdc, RECT &paint_area);
	void Draw_Roll_In_State(HDC hdc, RECT &paint_area);
	void Draw_Expanding_Roll_In_State(HDC hdc, RECT &paint_area);


	EPlatform_State Platform_State;
	HPEN Highlight_Pen, Platform_Ball_Pen, Platform_Inner_Pen;
	HBRUSH Platform_Ball_Brush, Platform_Inner_Brush;

	RECT Platform_Rect, Prev_Platform_Rect;

	static const int Normal_Width = 28;

	int Inner_Width;
	int Rolling_Step;
	int Meltdown_Platform_Y_Pos[Normal_Width * AsConfig::Global_Scale];


	static const int Height = 7;
	static const int Circle_Size = 7;
	static const int Normal_Platform_Inner_Width = Normal_Width - Circle_Size;
	static const int Meltdown_Speed = 3;
	static const int Max_Rolling_Step = 8;
	static const int Rolling_Platform_Speed = 3;
	static const int Roll_In_Platform_End_X_Pos = 99;
	
};
//------------------------------------------------------------------------------------------------------------
