#include "Tools.hpp"

// AHit_Checker
//------------------------------------------------------------------------------------------------------------
bool AHit_Checker::Check_Hit(double next_x_pos, double next_y_pos)
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AHit_Checker::Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x)
{// Проверяет пересечение горизонтального отрезка (проходящего от left_x до right_x через y) с окружностью радиусом radius

	double min_x, max_x;

	// x * x + y * y = R * R
	// x = sqrt(R * R - y * y)
	// y = sqrt(R * R - x * x)

	if (y > radius)
		return false;

	x = sqrt(radius * radius - y * y);

	max_x = next_x_pos + x;
	min_x = next_x_pos - x;

	if (max_x >= left_x && max_x <= right_x  ||  min_x >= left_x && min_x <= right_x)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------




// AMover
//------------------------------------------------------------------------------------------------------------
AMover::~AMover()
{}
//------------------------------------------------------------------------------------------------------------




// AGraphics_Object
//------------------------------------------------------------------------------------------------------------
AGraphics_Object::~AGraphics_Object()
{}
//------------------------------------------------------------------------------------------------------------




// AColor
//------------------------------------------------------------------------------------------------------------
AColor::AColor()
: R(0), G(0), B(0), Pen(0), Brush(0)
{}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(unsigned char r, unsigned char g, unsigned char b)
	: R(r), G(g), B(b), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, 0, RGB(r, g, b) );
	Brush = CreateSolidBrush(RGB(r, g, b) );
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(const AColor &color, int pen_size)
	: R(color.R), G(color.G), B(color.B), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, color.Get_RGB() );
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(unsigned char r, unsigned char g, unsigned char b, int pen_size)
	: R(r), G(g), B(b), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, RGB(r, g, b) );
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(const AColor &pen_color, const AColor &brush_color, int pen_size)
	: R(0), G(0), B(0), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, pen_color.Get_RGB() );
	Brush = CreateSolidBrush(brush_color.Get_RGB() );
}
//------------------------------------------------------------------------------------------------------------
int AColor::Get_RGB() const
{
	return RGB(R, G, B);
}
//------------------------------------------------------------------------------------------------------------
void AColor::Select(HDC hdc) const
{
	SelectObject(hdc, Pen);
	SelectObject(hdc, Brush);
}
//------------------------------------------------------------------------------------------------------------
void AColor::Select_Pen(HDC hdc) const
{
	SelectObject(hdc, Pen);
}
//------------------------------------------------------------------------------------------------------------
HBRUSH AColor::Get_Brush() const
{
	return Brush;
}