#pragma once
#include "Config.h"
#include "Active_Brick.h"

#define _USE_MATH_DEFINES // объявляет имя как существующее
#include <cmath>

//------------------------------------------------------------------------------------------------------------
enum ELetter_Type
{
	ELT_None,
	ELT_O
};
//------------------------------------------------------------------------------------------------------------
enum EBrick_Type
{
	EBT_None,
	EBT_Red,
	EBT_Blue
};
//------------------------------------------------------------------------------------------------------------
class ALevel
{
public:
	ALevel();

	void Init();
	void Draw(HWND hwnd,HDC hdc, RECT &paint_area);
	void Check_Level_Brick_Hit(int &next_y_pos, double &ball_direction);

	AActive_Brick Active_Brick;

	HPEN Brick_Red_Pen, Brick_Blue_Pen;
	HBRUSH Brick_Red_Brush, Brick_Blue_Brush;


private:
	void Set_Brick_Letter_Color(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush);
	void Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step);
	void Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type);


	RECT Level_Rect;
	HPEN Letter_Pen;

	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
};
//------------------------------------------------------------------------------------------------------------
