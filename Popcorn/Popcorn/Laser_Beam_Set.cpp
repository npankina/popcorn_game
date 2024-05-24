#include "Laser_Beam_Set.h"


AsLaser_Beam_Set::AsLaser_Beam_Set()
: Laser_Beams(Max_Laser_Beams_Count)
{}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Fire(double left_x_pos, double right_x_pos)
{
	ALaser_Beam *left = 0, *right = 0;

	for (auto &item : Laser_Beams)
	{
		if (item.Is_Active() )
			continue; // луч летит дальше

		if (left == 0)
			left = &item;
		else
			if (right == 0)
			{
				right = &item;
				break;
			}
	}

	if (left == 0 or right == 0)
		AsConfig::Throw(); // Не хватило "лазерных лучей"

	left->Set_At(left_x_pos, AsConfig::Platform_Y_Pos - 1);
	right->Set_At(right_x_pos, AsConfig::Platform_Y_Pos - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Disable_All()
{
	for (auto &item : Laser_Beams)
		item.Disable();
}
//------------------------------------------------------------------------------------------------------------
bool AsLaser_Beam_Set::Get_Next_Game_Object(int &index, AGame_Object **game_obj) // **game_obj указатель на указатель
{
	if (index < 0 or index >= Laser_Beams.size() )
		return false;

	*game_obj = &Laser_Beams[index++]; //  в указатель помещается адрес объекта, index по ссылке инкрементируется
	return true;
}
//------------------------------------------------------------------------------------------------------------