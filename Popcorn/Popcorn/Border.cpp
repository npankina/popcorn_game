#include "Border.h"

//Config
//------------------------------------------------------------------------------------------------------------
void AsConfig::Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush)
{
	pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
	brush = CreateSolidBrush(RGB(r, g, b));
}
//------------------------------------------------------------------------------------------------------------



// AsBorder
//------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
: Border_Blue_Pen{}, Border_White_Pen{}, Border_Blue_Brush{}, Border_White_Brush{}
{}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Init()
{
	AsConfig::Create_Pen_Brush(0, 255, 255, Border_Blue_Pen, Border_Blue_Brush);
	AsConfig::Create_Pen_Brush(255, 255, 255, Border_White_Pen, Border_White_Brush);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT &paint_area, HPEN bg_pen, HBRUSH bg_brush)
{// Отрисовка рамки уровня

 // 1. линия слева
	for (int i = 0; i < 50; i++)
		Draw_Element(hdc, 2, 1 + i * 4, false, bg_pen, bg_brush);

	// 2. линия справа
	for (int i = 0; i < 50; i++)
		Draw_Element(hdc, 201, 1 + i * 4, false, bg_pen, bg_brush);

	// 3. линия сверху
	for (int i = 0; i < 50; i++)
		Draw_Element(hdc, 3 + i * 4, 0, true, bg_pen, bg_brush);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, int x, int y, bool top_border, HPEN bg_pen, HBRUSH bg_brush)
{// Рисует элемент рамки уровня

 // Вывод основной линии
	SelectObject(hdc, Border_Blue_Pen);
	SelectObject(hdc, Border_Blue_Brush);

	if (top_border)
		Rectangle(hdc, x * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale, (y + 4) * AsConfig::Global_Scale);
	else
		Rectangle(hdc, (x + 1) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale, (y + 4) * AsConfig::Global_Scale);


	// Вывод белой линии
	SelectObject(hdc, Border_White_Pen);
	SelectObject(hdc, Border_White_Brush);

	if (top_border)
		Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale);
	else
		Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 1) * AsConfig::Global_Scale, (y + 4) * AsConfig::Global_Scale);

	// Черная точка
	SelectObject(hdc, bg_pen);
	SelectObject(hdc, bg_brush);

	if (top_border)
		Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale, (y + 3) * AsConfig::Global_Scale);
	else
		Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
