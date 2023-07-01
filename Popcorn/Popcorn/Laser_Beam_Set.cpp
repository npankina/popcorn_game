#include "Laser_Beam_Set.h"

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
//------------------------------------------------------------------------------------------------------------
bool AsLaser_Beam_Set::Get_Next_Game_Object(int &index, AGame_Object **game_obj) // **game_obj указатель на указатель
{
	if (index < 0 || index >= Max_Laser_Beams_Count)
		return false;

	*game_obj = &Laser_Beams[index++]; //  в указатель помещается адрес объекта, index по ссылке инкрементируется
	return true;
}
