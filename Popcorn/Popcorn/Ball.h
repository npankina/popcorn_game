#pragma once

#include "Config.h"
#include "Level.h"

//------------------------------------------------------------------------------------------------------------
enum EBall_State
{
	EBS_Normal,
	EBS_Lost,
	EBS_On_Platform
};
//------------------------------------------------------------------------------------------------------------
class ABall
{
public:
	ABall();

	void Init();
	void Draw(HDC hdc, RECT &paint_area);
	void Move(ALevel *level, int platform_x_pos, int platform_width);
	void Set_State(EBall_State new_state, int x_pos);
	EBall_State Get_State();

private:
	void Redraw_Ball();

	HPEN Ball_Pen;
	HBRUSH Ball_Brush;

	RECT Ball_Rect, Prev_Ball_Rect;

	EBall_State Ball_State;

	double Ball_X_Pos, Ball_Y_Pos;
	double Ball_Speed;
	double Ball_Direction;

	static const double Start_Ball_Y_Pos; 
};