﻿#pragma once
#include "Config.h"

class AMop_Cylinder : public AGraphics_Object
{
public:
	AMop_Cylinder(int x_pos, int y_pos, int width, int max_height);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Set_Height_For(double ratio);
	int Get_Height();

	void Set_Y_Pos(int y_pos);

	static const int Max_Cylinder_Height[4];

private:
	int X_Pos, Y_Pos;
	int Width, Height;
	int Max_Height;
	RECT Cylinder_Rect, Prev_Cylinder_Rect;

	static const int Binding_Height = 4;
	static const int Min_Height = Binding_Height + 1;

	static const int scale_ = AsConfig::Global_Scale;
};
//------------------------------------------------------------------------------------------------------------
