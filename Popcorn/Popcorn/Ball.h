#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum EBall_State
{
	EBS_Normal,
	EBS_Lost,
	EBS_On_Platform
};
//------------------------------------------------------------------------------------------------------------
class ABall;
class AHit_Checker
{
public:
	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball) = 0;
};
//------------------------------------------------------------------------------------------------------------
class ABall
{
public:
	ABall();

	void Init();
	void Draw(HDC hdc, RECT &paint_area);
	void Move();
	void Set_State(EBall_State new_state, int x_pos);
	EBall_State Get_State();
	
	static void Add_Hit_Checker(AHit_Checker *hit_checker);

	double Ball_Direction;
	double Rest_Distance;

	static const double Radius;
	

private:
	void Redraw_Ball();

	HPEN Ball_Pen;
	HBRUSH Ball_Brush;

	RECT Ball_Rect, Prev_Ball_Rect;

	EBall_State Ball_State;

	double Center_X_Pos, Center_Y_Pos;
	double Ball_Speed;

	static const double Start_Ball_Y_Pos;
	static int Hit_Checkers_Count;
	static AHit_Checker *Hit_Checkers[3];
};