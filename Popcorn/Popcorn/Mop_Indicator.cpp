#include "Mop_Indicator.h"

AColor_Fade AMop_Indicator::Fading_Colors(AsConfig::Blue_Color, AsConfig::Red_Color, Max_Fade_Step);
//------------------------------------------------------------------------------------------------------------
AMop_Indicator::AMop_Indicator(int x, int y, int time_offset)
	: X_Pos(x), Y_Pos(y), Current_Color(&AsConfig::Blue_Color), Time_Offset(time_offset), Indicator_Rect{}
{
	Set_Y_Pos(y);
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicator::Act()
{
	int total_timeout = Max_Fade_Step + Normal_Timeout;
	int current_tick = (AsConfig::Current_Timer_Tick + Time_Offset) % total_timeout;
	int current_offset;

	if (current_tick < Normal_Timeout)
		Current_Color = &AsConfig::Red_Color;
	else
	{
		current_offset = current_tick - Normal_Timeout;

		if (current_offset < 0 or current_offset >= Max_Fade_Step)
			AsConfig::Throw();

		Current_Color = Fading_Colors.Get_Color(current_offset);
	}

	AsTools::Invalidate_Rect(Indicator_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicator::Clear(HDC hdc, RECT &paint_area)
{}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicator::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect{};
	int x_pos = X_Pos * scale_;
	int y_pos = Y_Pos * scale_;
	int y_pos_with_shift = (Y_Pos + Height) * scale_;
	int x_pos_with_shift = x_pos + Width * scale_;

	if (!IntersectRect(&intersection_rect, &paint_area, &Indicator_Rect))
		return;

	AsTools::Rect(hdc, Indicator_Rect, *Current_Color);

	// Рамка идикатора
	AsConfig::Highlight_Color.Select_Pen(hdc);
	MoveToEx(hdc, x_pos, y_pos_with_shift, 0);
	LineTo(hdc, x_pos, y_pos);
	LineTo(hdc, x_pos_with_shift, y_pos);

	AsConfig::Shadow_Color.Select_Pen(hdc);
	MoveToEx(hdc, x_pos_with_shift, y_pos, 0);
	LineTo(hdc, x_pos_with_shift, y_pos_with_shift);
	LineTo(hdc, x_pos, y_pos_with_shift);
}
//------------------------------------------------------------------------------------------------------------
bool AMop_Indicator::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicator::Setup_Colors()
{
	for (int i = 0; i < Max_Fade_Step; i++)
		AsTools::Get_Fading_Color(AsConfig::Blue_Color, i, Max_Fade_Step);
}
//------------------------------------------------------------------------------------------------------------
void AMop_Indicator::Set_Y_Pos(int y)
{
	Y_Pos = y;

	Indicator_Rect.left = X_Pos * scale_;
	Indicator_Rect.top = Y_Pos * scale_;
	Indicator_Rect.right = Indicator_Rect.left + Width * scale_;
	Indicator_Rect.bottom = Indicator_Rect.top + Height * scale_;
}
//------------------------------------------------------------------------------------------------------------