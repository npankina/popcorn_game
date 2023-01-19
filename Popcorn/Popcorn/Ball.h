#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum EBall_State
{
	EBS_Disabled, // мяч отключен: не рисуется, не перемещается, никак не действует

	EBS_Normal,
	EBS_Lost,
	EBS_On_Platform,
	EBS_On_Parashute,
	EBS_Off_Parashute,
	EBS_Teleporting
};
//------------------------------------------------------------------------------------------------------------
enum ETeleport_State
{
	ETS_Started,
	ETS_Finished,
	ETS_Done
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
class AMover
{
public:
	virtual ~AMover();

	virtual void Begin_Movement() = 0;
	virtual void Finish_Movement() = 0;
	virtual void Advance(double max_speed) = 0;
	virtual double Get_Speed() = 0;

};
//------------------------------------------------------------------------------------------------------------
class ABall : public AMover
{
public:
	ABall();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	void Draw(HDC hdc, RECT &paint_area);
	void Draw_Teleporting(HDC hdc, int step);
	void Set_For_Test();
	bool Is_Test_Finished();
	EBall_State Get_State();
	void Set_State(EBall_State new_state, double x_pos = 0, double y_pos = 0);
	void Set_Speed(double new_speed);
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
	RECT Ball_Rect, Prev_Ball_Rect, Parashute_Rect, Prev_Parashute_Rect;

	double Ball_Direction;
	double Ball_Speed;
	double Rest_Test_Distance;
	double Center_X_Pos, Center_Y_Pos;

	bool Testing_Is_Active;
	int Test_Iteration;

	static const int Parashute_Size = 15;
	static int Hit_Checkers_Count;
	static AHit_Checker *Hit_Checkers[3];
};
//------------------------------------------------------------------------------------------------------------