#include "Config.h"

//AColor
//------------------------------------------------------------------------------------------------------------
AColor::AColor(unsigned char r, unsigned char g, unsigned char b)
: R(r), G(g), B(b)
{}
//------------------------------------------------------------------------------------------------------------




//AsConfig
int AsConfig::Current_Timer_Tick = 0;
bool AsConfig::Level_Has_Floor = true; // false

const AColor AsConfig::BG_Color(6, 30, 82);
const AColor AsConfig::Red_Brick_Color(230, 25, 229);
const AColor AsConfig::Blue_Brick_Color(0, 255, 255);

HPEN AsConfig::BG_Pen;
HBRUSH AsConfig::BG_Brush;
HWND AsConfig::Hwnd;

const double AsConfig::Moving_Step_Size = 1.0 / AsConfig::Global_Scale;
//------------------------------------------------------------------------------------------------------------
void AsConfig::Setup_Colors()
{
	Create_Pen_Brush(BG_Color, BG_Pen, BG_Brush);

}
//------------------------------------------------------------------------------------------------------------
void AsConfig::Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush)
{
	pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
	brush = CreateSolidBrush(RGB(r, g, b));
}
//------------------------------------------------------------------------------------------------------------
void AsConfig::Create_Pen_Brush(const AColor &color, HPEN &pen, HBRUSH &brush)
{
	pen = CreatePen(PS_SOLID, 0, RGB(color.R, color.G, color.B));
	brush = CreateSolidBrush(RGB(color.R, color.G, color.B));
}
//------------------------------------------------------------------------------------------------------------
int AsConfig::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, ... range - 1]
	return rand() * range / RAND_MAX;
}
//------------------------------------------------------------------------------------------------------------