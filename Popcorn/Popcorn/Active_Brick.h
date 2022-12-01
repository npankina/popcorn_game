﻿#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum EBrick_Type
{
	EBT_None,
	EBT_Red,
	EBT_Blue
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick
{
public:
	AActive_Brick(EBrick_Type brick_type);

	void Draw(HDC hdc, RECT& paint_area);
	void Act();
	static void Setup_Colors();

private:
	static unsigned char Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step);
	static void Get_Fading_Color(const AColor &color, int step, HPEN &pen, HBRUSH &brush); 
	static const int Max_Fade_Step = 80;

	EBrick_Type Brick_Type;
	RECT Brick_Rect;
	int Fade_Step;

	static HPEN Fading_Blue_Brick_Pens[Max_Fade_Step];
	static HBRUSH Fading_Blue_Brick_Brushes[Max_Fade_Step];
	static HPEN Fading_Red_Brick_Pens[Max_Fade_Step];
	static HBRUSH Fading_Red_Brick_Brushes[Max_Fade_Step];
};
//------------------------------------------------------------------------------------------------------------
