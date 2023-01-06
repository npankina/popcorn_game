#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum EBall_State
{
	EBS_Normal,
	EBS_Lost,
	EBS_On_Platform,
	EBS_On_Parashute,
	EBS_Off_Parashute,
	EBS_Teleporting
};
//------------------------------------------------------------------------------------------------------------
class ABall;
class AHit_Checker
{
public:
	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball) = 0;

	bool Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x);
};
//------------------------------------------------------------------------------------------------------------
class ABall
{
public:
	ABall();

	void Draw(HDC hdc, RECT &paint_area);
	void Draw_Teleporting(HDC hdc, int step);
	void Move();
	void Set_For_Test();
	bool Is_Test_Finished();
	EBall_State Get_State();
	void Set_State(EBall_State new_state, double x_pos, double y_pos = Start_Ball_Y_Pos);
	double Get_Direction();
	void Set_Direction(double new_direction);
	void Get_Center(double &x_pos, double &y_pos);
	void Reflect(bool from_horizontal);
	bool Is_Moving_Up();
	bool Is_Moving_Left();
	void Set_On_Parashute(int x, int y);

	static void Add_Hit_Checker(AHit_Checker *hit_checker);

	static const double Radius;

private:
	void Redraw_Ball();
	void Draw_Parashute(HDC hdc, RECT &paint_area);
	void Redraw_Parashute();
	void Clear_Parashute(HDC hdc);

	EBall_State Ball_State, Prev_Ball_State;
	double Ball_Speed;
	double Rest_Distance;
	double Ball_Direction;

	bool Testing_Is_Active;
	int Test_Iteration;
	double Rest_Test_Distance;

	double Center_X_Pos, Center_Y_Pos;

	RECT Ball_Rect, Prev_Ball_Rect, Parashute_Rect, Prev_Parashute_Rect;

	static const double Start_Ball_Y_Pos;
	static const int Parashute_Size = 15;
	static int Hit_Checkers_Count;
	static AHit_Checker *Hit_Checkers[3];
};
//------------------------------------------------------------------------------------------------------------