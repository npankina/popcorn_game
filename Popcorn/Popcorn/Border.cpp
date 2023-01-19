#include "Border.h"

// AsBorder
//------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
: Floor_Rect{}
{
	int scale = AsConfig::Global_Scale;
	 
	Floor_Rect.left = AsConfig::Level_X_Offset * scale;
	Floor_Rect.top = (AsConfig::Max_Y_Pos - 1) * scale;
	Floor_Rect.right = (AsConfig::Max_X_Pos - 1) * scale;
	Floor_Rect.bottom = AsConfig::Max_Y_Pos * scale;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT &paint_area)
{// Отрисовка рамки уровня

	int i;
	RECT intersection_rect;

	// 1. Линия слева
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 2, 1 + i * 4, false);

	// 2. Линия справа
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, AsConfig::Max_X_Pos + 1, 1 + i * 4, false);

	// 3. Линия сверху
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 3 + i * 4, 0, true);

	if (AsConfig::Level_Has_Floor)
	{
		if ( IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		{
			//AsConfig::Unbreakable_Red_Highlight.Select(hdc);


			MoveToEx(hdc, Floor_Rect.left, Floor_Rect.top, 0);
			LineTo(hdc, Floor_Rect.right, Floor_Rect.top);
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor()
{
	InvalidateRect(AsConfig::Hwnd, &Floor_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{
	bool got_hit = false;

	// Корректируем позицию при отражении от рамки
	if (next_x_pos - ball->Radius < AsConfig::Border_X_Offset)
	{
		ball->Reflect(false);
		got_hit = true;
	}

	if (next_y_pos - ball->Radius < AsConfig::Border_Y_Offset)
	{
		ball->Reflect(true);
		got_hit = true;
	}

	if (next_x_pos + ball->Radius > AsConfig::Max_X_Pos)
	{
		ball->Reflect(false);
		got_hit = true;
	}

	if (next_y_pos + ball->Radius > AsConfig::Max_Y_Pos)
	{
		if (AsConfig::Level_Has_Floor)
		{
			ball->Reflect(true);
			got_hit = true;
		}
		else
		{
			if (next_y_pos + ball->Radius > (double)AsConfig::Max_Y_Pos + ball->Radius * 4.0)  // Чтобы шарик смог улететь ниже пола, проверяем его max_y_pos ниже видимой границы
				ball->Set_State(EBS_Lost, next_x_pos);
		}
	}

	return got_hit;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{// Рисует элемент рамки уровня

	RECT intersection_rect, rect;
	int scale = AsConfig::Global_Scale;

	rect.left = x * scale;
	rect.top = y * scale;
	rect.right = (x + 4) * scale - 1;
	rect.bottom = (y + 4) * scale - 1;

	if ( !IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

 // Вывод основной линии
	AsConfig::Blue_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * scale, (y + 1) * scale, (x + 4) * scale - 1, (y + 4) * scale - 1);
	else
		Rectangle(hdc, (x + 1) * scale, y * scale, (x + 4) * scale - 1, (y + 4) * scale - 1);


	// Вывод белой линии
	AsConfig::White_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * scale, y * scale, (x + 4) * scale - 1, (y + 1) * scale - 1);
	else
		Rectangle(hdc, x * scale, y * scale, (x + 1) * scale - 1, (y + 4) * scale - 1);

	// Черная точка
	AsConfig::BG_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, (x + 2) * scale, (y + 2) * scale, (x + 3) * scale - 1, (y + 3) * scale - 1);
	else										  				   						
		Rectangle(hdc, (x + 2) * scale, (y + 1) * scale, (x + 3) * scale - 1, (y + 2) * scale - 1);
}
//------------------------------------------------------------------------------------------------------------