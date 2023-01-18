#pragma once
#include "Ball.h"

//------------------------------------------------------------------------------------------------------------
class AsBall_Set: public AMover
{
public:
	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Begin_Movement();
	virtual void Finish_Movement();

	void Draw(HDC hdc, RECT &paint_area);
	void Release_From_Platform(double platform_x_pos);
	void Set_On_Platform(double platform_x_pos);
	bool All_Balls_Are_Lost();
	void Set_For_Test();
	bool Is_Test_Finished();
	void Tripple_Balls();
	void Inverse();
	void Accelerate();
	void Reset_Speed();

private:
	ABall Balls[AsConfig::Max_Balls_Count];
};
//------------------------------------------------------------------------------------------------------------