﻿#pragma once
#include "Platform_State.h"

class AsPlatform_Expanding
{
public:
	AsPlatform_Expanding(AsPlatform_State &platform_state);
	bool Act(double &x_pos, EPlatform_State &next_state, bool &correct_pos);
	void Draw_State(HDC hdc, double x);
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Reset();

	double Expanding_Platform_Width;

private:
	void Draw_Expanding_Platform_Ball(HDC hdc, double x, bool is_left);
	void Draw_Expanding_Truss(HDC hdc, RECT &inner_rect, bool is_left);

	AsPlatform_State *Platform_State;

	static const int Expanding_Platform_Inner_Width = 12;
	static const double Max_Expanding_Platform_Width, Min_Expanding_Platform_Width, Expanding_Platform_Width_Step;
};