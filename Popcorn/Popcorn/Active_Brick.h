﻿#pragma once

#include "Config.h"

class AActive_Brick
{
public:
	AActive_Brick();

	void Draw(HDC hdc, RECT &paint_area);
	void Act(HWND hwnd);

	RECT Brick_Rect;
	int Fade_Step;

	static const int Max_Fade_Step = 80;
};
//------------------------------------------------------------------------------------------------------------
