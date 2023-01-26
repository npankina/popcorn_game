#include "Border.h"

// AsBorder
//------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
: Floor_Rect{}
{
	int scale = AsConfig::Global_Scale;
	 
	Floor_Rect.left = AsConfig::Level_X_Offset * scale;
	Floor_Rect.top = AsConfig::Floor_Y_Pos * scale;
	Floor_Rect.right = (AsConfig::Max_X_Pos - 1) * scale;
	Floor_Rect.bottom = AsConfig::Max_Y_Pos * scale;
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{ // Корректируем позицию при отражении от рамки

	bool got_hit = false;

	// Отражение от левого края
	if (next_x_pos - ball->Radius < AsConfig::Border_X_Offset)
	{
		ball->Reflect(false);
		got_hit = true;
	}

	// Отражение от верхнего края
	if (next_y_pos - ball->Radius < AsConfig::Border_Y_Offset)
	{
		ball->Reflect(true);
		got_hit = true;
	}

	// Отражение от правого края
	if (next_x_pos + ball->Radius > AsConfig::Max_X_Pos)
	{
		ball->Reflect(false);
		got_hit = true;
	}

	// Отражение от нижнего края
	if (AsConfig::Level_Has_Floor and next_y_pos + ball->Radius > AsConfig::Floor_Y_Pos)
	{
		ball->Reflect(true);
		got_hit = true;
	}
	
	// Чтобы шарик смог улететь ниже пола, проверяем его max_y_pos ниже видимой границы
	if (next_y_pos + ball->Radius > (double)AsConfig::Max_Y_Pos + ball->Radius * 4.0)
		ball->Set_State(EBS_Lost);

	return got_hit;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Act() {/* Заглушка! т.к. border сам не анимируется */}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT& paint_area)
{// Отрисовка рамки уровня
	int i;

	// 1. Линия слева
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 2, 1 + i * 4, false);

	// 2. Линия справа
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, AsConfig::Max_X_Pos + 1, 1 + i * 4, false);

	// 3. Линия сверху
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 3 + i * 4, 0, true);

	// 4. Пол, если есть
	if (AsConfig::Level_Has_Floor)
		Draw_Floor(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Clear(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (! AsConfig::Level_Has_Floor)
		return;

	if (! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, Floor_Rect.left, Floor_Rect.top, Floor_Rect.right - 1, Floor_Rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Finished() { return false; /* Заглушка! т.к. border сам не анимируется */ }
//------------------------------------------------------------------------------------------------------------
void AsBorder::Redraw_Floor()
{
	AsConfig::Invalidate_Rect(Floor_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{// Рисует элемент рамки уровня

	RECT intersection_rect{}, rect{};
	int scale = AsConfig::Global_Scale;

	rect.left = x * scale;
	rect.top = y * scale;
	rect.right = (x + 4) * scale;
	rect.bottom = (y + 4) * scale;

	if ( !IntersectRect(&intersection_rect, &paint_area, &rect) )
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
void AsBorder::Draw_Floor(HDC hdc, RECT &paint_area)
{
	int i;
	int x_pos, y_pos;
	int strokes_counter, stroke_len;
	int line_len = 4 * AsConfig::Global_Scale;
	int gap_len = 2 * AsConfig::Global_Scale;
	RECT intersection_rect;

	if ( !IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	stroke_len = line_len + gap_len;
	strokes_counter = (Floor_Rect.right - Floor_Rect.left) / stroke_len;
	x_pos = Floor_Rect.left;
	y_pos = Floor_Rect.top + (Floor_Rect.bottom - Floor_Rect.top) / 2;
	AsConfig::Letter_Color.Select(hdc);

	for (i = 0; i <= strokes_counter; i++)
	{
		MoveToEx(hdc, x_pos, y_pos, 0);
		LineTo(hdc, x_pos + line_len, y_pos);

		x_pos += stroke_len; 
	}
}
//------------------------------------------------------------------------------------------------------------
