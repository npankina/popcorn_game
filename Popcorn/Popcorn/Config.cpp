#include "Config.hpp"

bool AsConfig::Level_Has_Floor = true;
int AsConfig::Current_Timer_Tick = 0;

const AColor AsConfig::BG_Color(6, 30, 82);
const AColor AsConfig::Red_Color(230, 25, 229);
const AColor AsConfig::Blue_Color(0, 255, 255);
const AColor AsConfig::White_Color(255, 255, 255);
const AColor AsConfig::Letter_Color(White_Color, Global_Scale);
const AColor AsConfig::Gate_Color(White_Color, Global_Scale);
const AColor AsConfig::Laser_Color(White_Color, Global_Scale);
const AColor AsConfig::Unbreakable_Blue_Highlight(Blue_Color, Global_Scale);
const AColor AsConfig::Unbreakable_Red_Highlight(Red_Color, 3 * Global_Scale);
const AColor AsConfig::Teleport_Portal_Color(Blue_Color, BG_Color, Global_Scale);
const AColor AsConfig::Advertisement_Blue_Table(0, 159, 159, Global_Scale);
const AColor AsConfig::Advertisement_Red_Table(Red_Color, 2 * Global_Scale);
const AColor AsConfig::Platform_Inner_Color(0, 255, 255);
const AColor AsConfig::Platform_Circle_Color(230, 25, 229);
const AColor AsConfig::Highlight_Color(255, 255, 255);
const AColor AsConfig::Truss_Color(Platform_Inner_Color, Global_Scale);
const AColor AsConfig::Gun_Color(Highlight_Color, Global_Scale);
const AColor AsConfig::Monster_Dark_Red_Color(153, 17, 153);
const AColor AsConfig::Monster_Cornea_Color(BG_Color, White_Color, 2);
const AColor AsConfig::Monster_Iris_Color(BG_Color, Blue_Color, 2);


HWND AsConfig::Hwnd;

const double AsConfig::D_Global_Scale = (double)Global_Scale;
const double AsConfig::Moving_Step_Size = 1.0 / Global_Scale;
const double AsConfig::Start_Ball_Y_Pos = 184.0;
const double AsConfig::Ball_Acceleration = 1.001;
const double AsConfig::Normal_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;

//------------------------------------------------------------------------------------------------------------
void AsConfig::Throw()
{
	throw 22;
}
//------------------------------------------------------------------------------------------------------------
int AsTools::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, .. range - 1]

	return rand() * range / RAND_MAX;
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Round_Rect(HDC hdc, RECT &rect, int corner_radius)
{
	int radius = corner_radius * AsConfig::Global_Scale;
	RoundRect(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1, radius, radius);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Invalidate_Rect(RECT &rect)
{
	InvalidateRect(AsConfig::Hwnd, &rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Rect(HDC hdc, RECT &rect, const AColor color)
{
	color.Select(hdc);
	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Rect(HDC hdc, int x, int y, int width, int height, const AColor color)
{
	RECT rect{};

	const int scale = AsConfig::Global_Scale;

	rect.left = x * scale;
	rect.top = y * scale;
	rect.right = rect.left + width * scale;
	rect.bottom = rect.top + height * scale;

	color.Select(hdc);
	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Ellipse(HDC hdc, RECT &rect, const AColor &color)
{
	color.Select(hdc);
	::Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}