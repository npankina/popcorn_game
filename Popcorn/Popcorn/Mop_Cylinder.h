#pragma once
#include "Config.h"

class AMop_Cylinder : public AGraphics_Object
{
public:
	AMop_Cylinder(int x, int y, int width, int max_heiht);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Set_Y_Pos(int y);
	void Set_Height_For(double ratio);
	int Get_Height() { return Height; }

	static const int Max_Cylinders_Height[4];

private:
	int X_Pos, Y_Pos;
	int Width, Height, Max_Height;
	RECT Cylinder_Rect, Prev_Cylinder_Rect;

	static const int scale_ = AsConfig::Global_Scale;
	static const int Binding_Height = 4;
	static const int Min_Height = Binding_Height + 1;
};