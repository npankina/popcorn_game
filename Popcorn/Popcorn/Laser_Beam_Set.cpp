﻿#include "Laser_Beam_Set.hpp"

void AsLaser_Beam_Set::Begin_Movement()
{
	int i;

	for (i = 0; i < Max_Laser_Beams_Count; i++)
		Laser_Beams[i].Begin_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Finish_Movement()
{
	int i;

	for (i = 0; i < Max_Laser_Beams_Count; i++)
		Laser_Beams[i].Finish_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Advance(double max_speed)
{
	int i;

	for (i = 0; i < Max_Laser_Beams_Count; i++)
		Laser_Beams[i].Advance(max_speed);
}
//------------------------------------------------------------------------------------------------------------
double AsLaser_Beam_Set::Get_Speed()
{
	int i;
	double curr_speed, max_speed = 0.0;

	for (i = 0; i < Max_Laser_Beams_Count; i++)
	{
		curr_speed = Laser_Beams[i].Get_Speed();

		if (curr_speed > max_speed)
			max_speed = curr_speed;
	}

	return max_speed;
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Act()
{	// Заглушка. Не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Clear(HDC hdc, RECT &paint_area)
{
	int i;

	for (i = 0; i < Max_Laser_Beams_Count; i++)
		Laser_Beams[i].Clear(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Draw(HDC hdc, RECT &paint_area)
{
	int i;

	for (i = 0; i < Max_Laser_Beams_Count; i++)
		Laser_Beams[i].Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsLaser_Beam_Set::Is_Finished()
{
	return false;  // Заглушка, т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Fire(double left_x_pos, double right_x_pos)
{
	int i;
	ALaser_Beam *left = 0, *right = 0;

	for (i = 0; i < Max_Laser_Beams_Count; i++)
	{
		if (Laser_Beams[i].Is_Active() )
			continue; // луч летит дальше

		if (left == 0)
			left = &Laser_Beams[i];
		else
			if (right == 0)
			{
				right = &Laser_Beams[i];
				break;
			}
	}

	if (left == 0 || right == 0)
		AsConfig::Throw(); // Не хватило "лазерных лучей"

	left->Set_At(left_x_pos, AsConfig::Platform_Y_Pos - 1);
	right->Set_At(right_x_pos, AsConfig::Platform_Y_Pos - 1);
}