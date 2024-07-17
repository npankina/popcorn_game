﻿#pragma once
#include "Common.h"

//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	~AColor();
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);
	AColor(const AColor &color, int pen_size);
	AColor(unsigned char r, unsigned char g, unsigned char b, int pen_size);
	AColor(const AColor &pen_color, const AColor &brush_color, int pen_size);
	void operator=(const AColor &another);

	int Get_RGB() const;
	void Set_As(unsigned char r, unsigned char g, unsigned char b);
	void Select(HDC hdc) const;
	void Select_Pen(HDC hdc) const;
	HBRUSH Get_Brush() const;

	unsigned char R, G, B;

private:
	HPEN Pen;
	HBRUSH Brush;
}; 
//------------------------------------------------------------------------------------------------------------
class AColor_Fade
{
public:
	~AColor_Fade();
	AColor_Fade(const AColor &color, int max_fade_step);
	AColor* Get_Color(int fade_step);

private:
	std::vector<AColor *> Fading_Colors;
};
//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
	static void Throw();

	static bool Level_Has_Floor;
	static int Current_Timer_Tick;


	static const AColor BG_Color, Red_Color, Blue_Color, White_Color, Letter_Color, Laser_Color, Gate_Color;
	static const AColor Unbreakable_Blue_Highlight, Unbreakable_Red_Highlight, Teleport_Portal_Color;
	static const AColor Advertisement_Blue_Table, Advertisement_Red_Table;
	static const AColor Platform_Inner_Color, Platform_Circle_Color, Highlight_Color, Truss_Color, Gun_Color;
	static const AColor Monster_Dark_Red_Color, Monster_Cornea_Color, Monster_Iris_Color, BG_Outcome_Color, Explosion_Red_Color, Explosion_Blue_Color;
	static const AColor Monster_Comet_Tail;
	static const AColor Shadow_Color, Highlight_Panel_Color;
	static HWND Hwnd;

	static const double D_Global_Scale;
	static const double Moving_Step_Size;
	static const double Start_Ball_Y_Pos;
	static const double Ball_Acceleration;
	static const double Normal_Ball_Speed;
	static const double Min_Ball_Angle;

	static const int Global_Scale = 3;
	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;
	static const int Level_Width = 12;  // Ширина уровня в ячейках
	static const int Level_Height = 14;  // Высота уровня в ячейках
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 6;
	static const int Cell_Width = 16;
	static const int Cell_Height = 8;
	static const int Brick_Width = 15;
	static const int Brick_Height = 7;
	static const int Max_X_Pos = Level_X_Offset + Cell_Width * Level_Width;
	static const int Max_Y_Pos = 199;
	static const int Platform_Y_Pos = 185;
	static const int FPS = 20;

	static const int Max_Active_Bricks_Count = 10;
	static const int Hits_Per_Letter = 1;  // Вероятность выбить букву = 1.0 / Hits_Per_Letter
	static const int Max_Falling_Letters_Count = 10;
	static const int Max_Balls_Count = 10;
	static const int Max_Movers_Count = 10;
	static const int Initial_Life_Count = 5;
	static const int Max_Life_Count = 12;
	static const int Floor_Y_Pos = Max_Y_Pos - 1;

	static const int Platform_Normal_Width = 28;
	static const int Platform_Height = 7;
	static const int Platform_Circle_Size = 7;
	static const int Platform_Normal_Inner_Width = Platform_Normal_Width - Platform_Circle_Size;

	static const int Gates_Count = 8;

	static const double Ball_Radius;
};
//------------------------------------------------------------------------------------------------------------
class AsTools
{
public:
	static int Rand(int range);
	static void Round_Rect(HDC hdc, RECT &rect, int corner_radius = 2);
	static void Invalidate_Rect(RECT &rect);
	static void Rect(HDC hdc, RECT &rect, const AColor &color);
	static void Rect(HDC hdc, int x, int y, int width, int height, const AColor &color);
	static void Ellipse(HDC hdc, RECT &rect, const AColor &color);
	static unsigned char Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step, int max_step);
	static AColor* Get_Fading_Color(const AColor &origin_color, int step, int max_step);
	static bool Reflect_On_Circle(double next_x_pos, double next_y_pos, double circle_x, double circle_y, double circle_radius, ABall_Object *ball);
};
//------------------------------------------------------------------------------------------------------------
class AHit_Checker
{
public:
	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball) = 0;
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);
	

	bool Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x);
};
//------------------------------------------------------------------------------------------------------------
class AHit_Checker_List
{
public:
	AHit_Checker_List();
	void Add_Hit_Checker(AHit_Checker *hit_checker);
	bool Check_Hit(double x_pos, double y_pos, ABall_Object *ball);
	bool Check_Hit(double x_pos, double y_pos);
	bool Check_Hit(RECT &rect);

private:
	int Hit_Checkers_Count;
	std::vector<AHit_Checker *> Hit_Checkers;
};
//------------------------------------------------------------------------------------------------------------