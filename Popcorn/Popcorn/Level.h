﻿#pragma once

#include "Active_Brick.h"
#include "Ball.h"

//------------------------------------------------------------------------------------------------------------
enum ELetter_Type
{
	ELT_None,

	ELT_O
};
//------------------------------------------------------------------------------------------------------------
class AFalling_Letter : public AGraphics_Object
{
public:
	AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT& paint_area);
	virtual bool Is_Finished();

	bool Got_Hit;

	const ELetter_Type Letter_Type;

private:
	void Draw_Brick_Letter(HDC hdc);
	void Set_Brick_Letter_Colors(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush);

	EBrick_Type Brick_Type;
	RECT Letter_Cell, Prev_Letter_Cell;
	int X, Y;
	int Rotation_Step;
	int Next_Rotation_Tick;

	static const int Ticks_Per_Step = 4;
};
//------------------------------------------------------------------------------------------------------------
class ALevel: public AHit_Checker
{
public:
	ALevel();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	void Init();
	void Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width]);
	void Act();
	void Draw(HDC hdc, RECT &paint_area);


	//AActive_Brick Active_Brick;
	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Test_Level[AsConfig::Level_Height][AsConfig::Level_Width];

private:
	bool Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos);
	bool Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos);
	void Draw_Brick(HDC hdc, RECT &brick_rect, EBrick_Type brick_type);
	void On_Hit(int brick_x, int brick_y);
	bool Add_Falling_Letter(int brick_x, int brick_y, EBrick_Type brick_type);
	void Add_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type);
	void Draw_Objects(HDC hdc, RECT &paint_area, AGraphics_Object **objects_array, int objects_max_count);
	void Act_Objects(AGraphics_Object **objects_array, int objects_max_count);


	RECT Level_Rect;

	double Current_Brick_Left_X, Current_Brick_Right_X;
	double Current_Brick_Top_Y, Current_Brick_Low_Y;

	char Current_Level[AsConfig::Level_Height][AsConfig::Level_Width];
	int Active_Bricks_Counter;
	int Falling_Letters_Counter;

	AActive_Brick *Active_Bricks[AsConfig::Max_Active_Bricks_Count];
	AFalling_Letter *Falling_Letters[AsConfig::Max_Falling_Letters_Count];
};
//------------------------------------------------------------------------------------------------------------
