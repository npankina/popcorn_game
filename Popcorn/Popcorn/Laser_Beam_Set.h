#pragma once
#include "Config.h"
#include "Laser_Beam.h"

class AsLaser_Beam_Set: public AGame_Objects_Set
{
public:
	AsLaser_Beam_Set();
	void Fire(double left_x_pos, double right_x_pos);
	void Disable_All();

private:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **game_obj); // iterator

	std::vector<ALaser_Beam> Laser_Beams;
	static const int Max_Laser_Beams_Count = 10;
};