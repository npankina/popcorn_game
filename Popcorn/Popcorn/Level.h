﻿#pragma once

#include "Falling_Letter.h"
#include "Ball.h"

//------------------------------------------------------------------------------------------------------------
class AsLevel: public AHit_Checker
{
public:
	AsLevel();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	void Init();
	void Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width]);
	void Act();
	void Draw(HDC hdc, RECT &paint_area);
	bool Get_Next_Falling_letter(int &index, AFalling_Letter **falling_letter);


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

	AActive_Brick_Red_Blue *Active_Bricks[AsConfig::Max_Active_Bricks_Count];
	AFalling_Letter *Falling_Letters[AsConfig::Max_Falling_Letters_Count];
};
//------------------------------------------------------------------------------------------------------------
