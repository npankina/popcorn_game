#include "Laser_Beam.h"

AHit_Checker_List ALaser_Beam::Hit_Checker_List;

ALaser_Beam::ALaser_Beam()
: Laser_Beam_State(ELaser_Beam_State::Disabled), X_Pos(0.0), Y_Pos(0.0), Speed(0.0), Beam_Rect{}, Prev_Beam_Rect{}
{}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Begin_Movement()
{
	// Заглушка! Метод не используется
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Finish_Movement()
{
	if (Laser_Beam_State != ELaser_Beam_State::Disabled)
		Redraw_Beam();
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Advance(double max_speed)
{
	double next_step;

	if (Laser_Beam_State != ELaser_Beam_State::Active)
		return;

	next_step = Speed / max_speed * AsConfig::Moving_Step_Size;

	Y_Pos -= next_step;

	if (Y_Pos < AsConfig::Level_Y_Offset)
		Stop();

	if (Hit_Checker_List.Check_Hit(X_Pos, Y_Pos) )
		Stop();
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Stop()
{
	Laser_Beam_State = ELaser_Beam_State::Stopping;
	Speed = 0.0;
}
//------------------------------------------------------------------------------------------------------------
double ALaser_Beam::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Act()
{
	// Заглушка! Метод не используется - Лазерные лучи не имеют своей анимации
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (Laser_Beam_State == ELaser_Beam_State::Disabled)
		return;

	if (Laser_Beam_State == ELaser_Beam_State::Cleanup)
		Laser_Beam_State = ELaser_Beam_State::Disabled;


	if ( !IntersectRect(&intersection_rect, &paint_area, &Prev_Beam_Rect) )
		return;

	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, Prev_Beam_Rect.left, Prev_Beam_Rect.top, Prev_Beam_Rect.right - 1, Prev_Beam_Rect.bottom);
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;
	int x, y;

	if (Laser_Beam_State == ELaser_Beam_State::Disabled)
		return;

	if (Laser_Beam_State == ELaser_Beam_State::Stopping)
		Laser_Beam_State = ELaser_Beam_State::Cleanup;

	if ( !IntersectRect(&intersection_rect, &paint_area, &Beam_Rect) )
		return;

	AsConfig::Laser_Color.Select(hdc);

	x = Beam_Rect.left + (Beam_Rect.right - Beam_Rect.left) / 2;
	y = Beam_Rect.top;

	MoveToEx(hdc, x, y + 1, 0);
	LineTo(hdc, x, y + Height * AsConfig::Global_Scale - AsConfig::Global_Scale / 2 - 1);
}
//------------------------------------------------------------------------------------------------------------
bool ALaser_Beam::Is_Finished()
{
	return false; // Заглушка! Метод не используется
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Redraw_Beam()
{
	Prev_Beam_Rect = Beam_Rect;

	Beam_Rect.left = (int)((X_Pos - (double)Width / 2.0) * AsConfig::D_Global_Scale);
	Beam_Rect.top = (int)(Y_Pos * AsConfig::D_Global_Scale);
	Beam_Rect.right = Beam_Rect.left + Width * AsConfig::Global_Scale;
	Beam_Rect.bottom = Beam_Rect.top + Height * AsConfig::Global_Scale;

	AsTools::Invalidate_Rect(Beam_Rect);
	AsTools::Invalidate_Rect(Prev_Beam_Rect);
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Set_At(double x, double y)
{
	X_Pos = x;
	Y_Pos = y;

	Laser_Beam_State = ELaser_Beam_State::Active;
	Speed = 10.0;

	Redraw_Beam();
}
//------------------------------------------------------------------------------------------------------------
bool ALaser_Beam::Is_Active()
{
	if (Laser_Beam_State == ELaser_Beam_State::Active)
		return true;
	return false;
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Disable()
{
	Laser_Beam_State = ELaser_Beam_State::Stopping;
	Speed = 0.0;
}
//------------------------------------------------------------------------------------------------------------