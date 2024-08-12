
#include "Mop.h"

//------------------------------------------------------------------------------------------------------------
AsMop::~AsMop()
{
	for (auto *indicator : Mop_Indicators)
		delete indicator;

	for (auto *cylinder : Mop_Cylinders)
		delete cylinder;

	Mop_Indicators.erase(Mop_Indicators.begin(), Mop_Indicators.end());
	Mop_Cylinders.erase(Mop_Cylinders.begin(), Mop_Cylinders.end());
}
//------------------------------------------------------------------------------------------------------------
AsMop::AsMop() : Y_Pos(0), Max_Y_Pos(0), Lifting_Height(0), Start_Tick(0), Mop_Rect{}, Prev_Mop_Rect{}, Mop_State(EMop_State::Idle)
{
	AMop_Indicator *indicator = nullptr;
	AMop_Cylinder *cylinder = nullptr;

	int x_pos;
	int y_pos;
	int height, max_height;

	for (int i = 0; i < Indicator_Count; i++)
	{
		x_pos = AsConfig::Level_X_Offset + 1 + i * 19;

		indicator = new AMop_Indicator(x_pos, AsConfig::Level_Y_Offset + 1, i * 80);
		Mop_Indicators.push_back(indicator);
	}

	for (int i = 0; i < 4; i++)
	{
		x_pos = AsConfig::Level_X_Offset + Width / 2 - 6 - i;
		y_pos = AsConfig::Level_Y_Offset + 7 + i * 5;
		height = 13 + i * 2;
		max_height = AMop_Cylinder::Max_Cylinders_Height[i];

		cylinder = new AMop_Cylinder(x_pos, y_pos, height, max_height);
		Mop_Cylinders.push_back(cylinder);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Begin_Movement()
{

}
//------------------------------------------------------------------------------------------------------------
void AsMop::Finish_Movement()
{}
//------------------------------------------------------------------------------------------------------------
void AsMop::Advance(double max_speed)
{}
//------------------------------------------------------------------------------------------------------------
double AsMop::Get_Speed()
{
	return 0.0;
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Act()
{
	int time_offset;
	double ratio;

	Prev_Mop_Rect = Mop_Rect;

	if ( Mop_State == EMop_State::Idle or Mop_State == EMop_State::Descend_Done)
		return;

	time_offset = AsConfig::Current_Timer_Tick - Start_Tick;

	if (Mop_State == EMop_State::Ascending)
	{

	}

	if (time_offset <= Expansion_Timeout)
	{
		ratio = (double)time_offset / (double)Expansion_Timeout;

		if (Mop_State == EMop_State::Showing)
			ratio = 1.0 - ratio;

		for (auto *cylinder : Mop_Cylinders)
			cylinder->Set_Height_For(ratio); // увеличить высоту цилиндра

		Set_Mop();
	}
	else
	{
		switch (Mop_State)
		{
		case EMop_State::Clearing:
			Mop_State = EMop_State::Clear_Done;
			break;

		case EMop_State::Showing:
			Mop_State = EMop_State::Show_Done;
			break;

		default:
			AsConfig::Throw();
		}
	}

	for (auto *indicator : Mop_Indicators)
		indicator->Act();

	AsTools::Invalidate_Rect(Mop_Rect);
	AsTools::Invalidate_Rect(Prev_Mop_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect{};

	if (Mop_State == EMop_State::Idle)
		return;

	if (!IntersectRect(&intersection_rect, &paint_area, &Mop_Rect))
		return;

	AsTools::Rect(hdc, Prev_Mop_Rect, AsConfig::BG_Color);

	for (auto *cylinder : Mop_Cylinders)
		cylinder->Clear(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Draw(HDC hdc, RECT &paint_area)
{
	if (Mop_State == EMop_State::Idle)
		return;

	AsTools::Rect(hdc, AsConfig::Level_X_Offset, Y_Pos, Width, Height, AsConfig::Red_Color);

	for (auto *indicator : Mop_Indicators)
		indicator->Draw(hdc, paint_area);

	for (auto *cylinder : Mop_Cylinders)
		cylinder->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsMop::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Activate(bool is_cleaning)
{
	if (is_cleaning)
	{
		Y_Pos = 172;
		Mop_State = EMop_State::Ascending;

		Lifting_Height = Get_Cylinders_Height() + Height;
		Max_Y_Pos = AsConfig::Max_Y_Pos + Lifting_Height;
	}
	else
		Mop_State = EMop_State::Showing;

	Start_Tick = AsConfig::Current_Timer_Tick;

	Set_Mop();
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Set_Mop()
{
	int current_y_pos = 0;

	Y_Pos = Max_Y_Pos - Get_Cylinders_Height() - Height + 1;

	for (auto *indicator : Mop_Indicators)
		indicator->Set_Y_Pos(Y_Pos + 1);

	for (int i = 0; i < Mop_Cylinders.size(); i++)
	{
		Mop_Cylinders[i]->Set_Y_Pos(Y_Pos + Height + current_y_pos);
		current_y_pos += Mop_Cylinders[i]->Get_Height();
	}

	Mop_Rect.left = AsConfig::Level_X_Offset * scale_;
	Mop_Rect.top = Y_Pos * scale_;
	Mop_Rect.right = Mop_Rect.left + Width * scale_;
	Mop_Rect.bottom = Mop_Rect.top + Height * scale_;
}
//------------------------------------------------------------------------------------------------------------
void AsMop::Clear_Area(HDC hdc)
{
	RECT rect = Mop_Rect;
	rect.bottom = AsConfig::Max_Y_Pos * scale_;

	if (Mop_State == EMop_State::Idle)
		return;

	AsTools::Rect(hdc, rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
int AsMop::Get_Cylinders_Height()
{
	int total_cylinders_height = 0;


	for (auto *cylinder : Mop_Cylinders)
		total_cylinders_height += cylinder->Get_Height();

	return total_cylinders_height;
}
//------------------------------------------------------------------------------------------------------------