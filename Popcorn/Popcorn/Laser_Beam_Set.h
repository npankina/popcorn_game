﻿#pragma once
#include "Config.h"
#include "Laser_Beam.h"

class AsLaser_Beam_Set: public AsGame_Objects_Set
{
public:
	void Fire(double left_x_pos, double right_x_pos);

private:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **game_obj); // iterator

	static const int Max_Laser_Beams_Count = 10;
	ALaser_Beam Laser_Beams[Max_Laser_Beams_Count];
};