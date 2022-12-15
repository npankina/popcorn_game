﻿#pragma once
#include <Windows.h> // имена переменных Windows
#define _USE_MATH_DEFINES // объявляет имя как существующее
#include <cmath>

//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	AColor(unsigned char r, unsigned char g, unsigned char b);

	int Get_RGB() const;

	unsigned char R, G, B;
};
//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
	static void Setup_Colors();
	static void Create_Pen_Brush(const AColor &color, HPEN &pen, HBRUSH &brush);
	static void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush);
	static int Rand(int range);


	
	static int Current_Timer_Tick;
	static bool Level_Has_Floor;
	static const AColor BG_Color, Red_Brick_Color, Blue_Brick_Color;
	static HPEN BG_Pen, Brick_Red_Pen, Brick_Blue_Pen, Letter_Pen;
	static HBRUSH BG_Brush, Brick_Red_Brush, Brick_Blue_Brush;
	static HWND Hwnd;

	static const double Moving_Step_Size;

	static const int Global_Scale = 3;
	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;
	static const int Level_Width = 12;
	static const int Level_Height = 14;
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 6;
	static const int Cell_Width = 16;
	static const int Cell_Height = 8;
	static const int Max_X_Pos = Level_X_Offset + Cell_Width * Level_Width;
	static const int Max_Y_Pos = 199;
	static const int Platform_Y_Pos = 185;
	static const int FPS = 20;
	static const int Brick_Width = 15;
	static const int Brick_Height = 7;
	static const int Max_Active_Bricks_Count = 10;
	static const int Max_Falling_Letters_Count = 10;
	static const int Hits_Per_Letter = 1; // Вероятность выбить букву из кирпича = 1.0 / Hits_Per_Letter
};
//------------------------------------------------------------------------------------------------------------