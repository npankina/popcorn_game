#include "Mop_Cylinder.h"

// class AMop_Cylinder
const int AMop_Cylinder::Max_Cylinder_Height[4] = { 88, 47, 37, 15 };
//------------------------------------------------------------------------------------------------------------
AMop_Cylinder::AMop_Cylinder(int x_pos, int y_pos, int width, int max_height)
	: X_Pos(x_pos), Y_Pos(y_pos), Width(width), Height(Min_Height), Max_Height(max_height)
{}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinder::Act()
{}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinder::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect{};

	if (! IntersectRect(&intersection_rect, &paint_area, &Prev_Cylinder_Rect) )
		return;

	AsTools::Rect(hdc, Prev_Cylinder_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinder::Draw(HDC hdc, RECT &paint_area)
{
	RECT rect{};
	RECT intersection_rect{};

	if (! IntersectRect(&intersection_rect, &paint_area, &Cylinder_Rect) )
		return;

	rect.left = X_Pos * scale_;
	rect.top = Y_Pos * scale_;
	rect.right = rect.left + Width * scale_;
	rect.bottom = rect.top + 4 * scale_;

	// 1. Крепление цилиндра
	AsConfig::Red_Color.Select(hdc);
	AsTools::Round_Rect(hdc, rect);

	AsTools::Rect(hdc, X_Pos + 2, Y_Pos + 1, 1, Binding_Height - 2, AsConfig::BG_Color);
	AsTools::Rect(hdc, X_Pos + 4, Y_Pos + 1, 1, Binding_Height - 2, AsConfig::BG_Color);
	AsTools::Rect(hdc, X_Pos + Width - 5, Y_Pos + 1, 1, Binding_Height - 2, AsConfig::BG_Color);
	AsTools::Rect(hdc, X_Pos + Width - 3, Y_Pos + 1, 1, Binding_Height - 2, AsConfig::BG_Color);


	// 2. Цилиндр
	AsTools::Rect(hdc, X_Pos + 2, Y_Pos + Binding_Height, 2, Height - Binding_Height, AsConfig::White_Color);
	AsTools::Rect(hdc, X_Pos + 4, Y_Pos + Binding_Height, 1, Height - Binding_Height, AsConfig::Blue_Color);
	AsTools::Rect(hdc, X_Pos + 5, Y_Pos + Binding_Height, 1, Height - Binding_Height, AsConfig::White_Color);
	AsTools::Rect(hdc, X_Pos + 6, Y_Pos + Binding_Height, Width - 8, Height - Binding_Height, AsConfig::Blue_Color);
}
//------------------------------------------------------------------------------------------------------------
bool AMop_Cylinder::Is_Finished()
{
	return false;  //!!! Надо сделать!
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinder::Set_Height_For(double ratio)
{
	Height = (int)( (double)Max_Height * ratio);

	if (Height < Min_Height)
		Height = Min_Height;
	else
		if (Height > Max_Height)
			Height = Max_Height;
}
//------------------------------------------------------------------------------------------------------------
int AMop_Cylinder::Get_Height()
{
	return Height;
}
//------------------------------------------------------------------------------------------------------------
void AMop_Cylinder::Set_Y_Pos(int y_pos)
{
	const int scale = AsConfig::Global_Scale;

	Y_Pos = y_pos;

	Prev_Cylinder_Rect = Cylinder_Rect;

	Cylinder_Rect.left = X_Pos * scale;
	Cylinder_Rect.top = Y_Pos * scale;
	Cylinder_Rect.right = Cylinder_Rect.left + Width * scale;
	Cylinder_Rect.bottom = Cylinder_Rect.top + Height * scale;

	AsTools::Invalidate_Rect(Cylinder_Rect);
	AsTools::Invalidate_Rect(Prev_Cylinder_Rect);
}
//------------------------------------------------------------------------------------------------------------
