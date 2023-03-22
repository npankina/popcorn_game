#include "Monster.hpp"


AMonster::AMonster()
: X_Pos(0), Y_Pos(0), Cornea_Height(Max_Cornea_Height), Is_Active(false), Monster_Rect{}
{}
//------------------------------------------------------------------------------------------------------------
void AMonster::Begin_Movement()
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Finish_Movement()
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Advance(double max_speed)
{
}
//------------------------------------------------------------------------------------------------------------
double AMonster::Get_Speed()
{
	return 0.0;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act()
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT &paint_area)
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	RECT intersection_rect{}, rect{};
	HRGN region{};

	if (!Is_Active)
		return;

	if (! IntersectRect(&intersection_rect, &paint_area, &Monster_Rect) )
		return;

	// 1. Рисуем фон
	// 1.1. Огриничение вывода фона
	rect = Monster_Rect;
	++rect.right;
	++rect.bottom;

	// 1.2. Темный фон
	AsTools::Ellipse(hdc, Monster_Rect, AsConfig::Monster_Dark_Red_Color);

 	region = CreateEllipticRgnIndirect(&rect);
	SelectClipRgn(hdc, region); // назначение региона обрезки

	// 1.3. Посветлее
	rect.left -= 2 * scale;
	rect.top -= 3 * scale;
	rect.right -= 2 * scale;
	rect.bottom -= 3 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	// Рисуем глаз
	// 2.1. Роговица
	rect = Monster_Rect;

	rect.left += scale;
	rect.top += 2 * scale;
	rect.right -= scale;
	rect.bottom = rect.top + Cornea_Height * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Monster_Cornea_Color);
	
	// 2.2. Радужка
	rect = Monster_Rect;

	rect.left += 4 * scale;
	rect.top += 4 * scale;
	rect.right -= 4 * scale;
	rect.bottom -= 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Monster_Iris_Color);

	// 2.3. Зрачок
	rect = Monster_Rect;

	rect.left += 7 * scale;
	rect.top += 6 * scale;
	rect.right = rect.left + 2 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsTools::Rect(hdc, rect, AsConfig::BG_Color);

	SelectClipRgn(hdc, 0); // снятие региона обрезки
	DeleteObject(region); // удаление региона обрезки
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Activate(int x_pos, int y_pos)
{
	const int scale = AsConfig::Global_Scale;
	Is_Active = true;

	X_Pos = x_pos;
	Y_Pos = y_pos;

	Monster_Rect.left = X_Pos * scale;
	Monster_Rect.top = Y_Pos * scale;
	Monster_Rect.right = Monster_Rect.left + Width * scale;
	Monster_Rect.bottom = Monster_Rect.top + Height * scale;
}
