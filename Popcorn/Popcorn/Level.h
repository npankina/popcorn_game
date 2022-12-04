#pragma once
#include "Ball.h"
#include "Active_Brick.h"


//------------------------------------------------------------------------------------------------------------
enum ELetter_Type
{
	ELT_None,
	ELT_O
};
//------------------------------------------------------------------------------------------------------------
class ALevel : public AHit_Checker
{
public:
	ALevel();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);
	
	void Init();
	void Draw(HWND hwnd,HDC hdc, RECT &paint_area);

	AActive_Brick Active_Brick;

	HPEN Brick_Red_Pen, Brick_Blue_Pen;
	HBRUSH Brick_Red_Brush, Brick_Blue_Brush;


private:
	bool Hit_Circle_On_line(double y, double next_x_pos, double left_x, double right_x, double radius);
	void Set_Brick_Letter_Color(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush);
	void Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step);
	void Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type);


	RECT Level_Rect;
	HPEN Letter_Pen;

	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
};
//------------------------------------------------------------------------------------------------------------
