#pragma once
#include <Windows.h> // имена переменных Windows
#define _USE_MATH_DEFINES // объ€вл€ет им€ как существующее
#include <cmath>

#include "Border.h"

//------------------------------------------------------------------------------------------------------------
// Global Variables:
const int Timer_ID = WM_USER + 1;

//------------------------------------------------------------------------------------------------------------
enum EKey_Type
{
	EKT_Left,
	EKT_Right,
	EKT_Space
};

enum EBrick_Type
{
	EBT_None,
	EBT_Red,
	EBT_Blue
};

enum ELetter_Type
{
	ELT_None,
	ELT_O
};
//------------------------------------------------------------------------------------------------------------
class AsEngine;
class ALevel;
class APlatform;
//------------------------------------------------------------------------------------------------------------
class ABall
{
public:
	ABall();

	void Init();
	void Draw(HDC hdc, RECT &paint_area, AsEngine *engine);
	void Move(AsEngine *Engine, ALevel *level, APlatform *platform);

	HPEN Ball_Pen;
	HBRUSH Ball_Brush;
	double Ball_Direction = M_PI - M_PI_4;

	static const int Ball_Size = 4;

private:
	RECT Ball_Rect, Prev_Ball_Rect;

	int Ball_X_Pos, Ball_Y_Pos;
	double Ball_Speed = 3.0;
};
//------------------------------------------------------------------------------------------------------------
class ALevel
{
public:
	ALevel();

	void Init();
	void Draw(HDC hdc, RECT &paint_area);
	void Check_Level_Brick_Hit(int &next_y_pos, double &ball_direction);
	

	HPEN Brick_Red_Pen, Brick_Blue_Pen;
	HBRUSH Brick_Red_Brush, Brick_Blue_Brush;

	static const int Level_Width = 12; // ширина уровн€ в €чейках
	static const int Level_Height = 14; // высота уровн€ в €чейках
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 6;
	static const int Cell_Width = 16;
	static const int Cell_Height = 8;

private:
	void Set_Brick_Letter_Color(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush);
	void Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step);
	void Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type);

	
	RECT Level_Rect;
	HPEN Letter_Pen;
	
	static const int Brick_Width = 15;
	static const int Brick_Height = 7;
};
//------------------------------------------------------------------------------------------------------------
class APlatform
{
public:
	APlatform();

	void Init();
	void Redraw(AsEngine *engine);
	void Draw(HDC hdc, RECT &paint_area, AsEngine *engine, ALevel *level);

	int X_Pos;
	int Width;
	int X_Step;

	static const int Y_Pos = 185;

private:
	HPEN Highlight_Pen;

	RECT Platform_Rect, Prev_Platform_Rect;

	int Inner_Width;

	static const int Height = 7;
	static const int Circle_Size = 7;
};
//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
	AsEngine();

	void Init_Engine(HWND hWnd);
	void Draw_Frame(HDC hdc, RECT &paint_area);
	int On_Key_Down(EKey_Type key_type);
	int On_Timer();

	
	static const int Max_X_Pos = ALevel::Level_X_Offset + ALevel::Cell_Width * ALevel::Level_Width;
	static const int Max_Y_Pos = 199 - ABall::Ball_Size;
	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;

	HWND Hwnd;
	HPEN BG_Pen;
	HBRUSH BG_Brush;

private:
	ABall Ball;
	ALevel Level;
	APlatform Platform;
	AsBorder Border;
};
//------------------------------------------------------------------------------------------------------------
