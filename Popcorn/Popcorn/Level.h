﻿#pragma once

#include "Falling_Letter.h"
#include "Info_Panel.h"
#include <vector>

//------------------------------------------------------------------------------------------------------------
struct SPoint
{
	int X, Y;
};
//------------------------------------------------------------------------------------------------------------
class AsLevel: public AHit_Checker, public AGame_Object
{
public:
	~AsLevel();
	AsLevel();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	
	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Init();
	void Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width]);
	bool Get_Next_Falling_Letter(int &index, AFalling_Letter **falling_letter);
	void Stop();

	static bool Has_Brick_At(int x_pos, int y_pos);
	static bool Has_Brick_At(RECT &monster_rect);


	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_02[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_03[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_04[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_05[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_06[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_07[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_08[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_09[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_10[AsConfig::Level_Height][AsConfig::Level_Width];


	static char Test_Level[AsConfig::Level_Height][AsConfig::Level_Width];

private:
	bool On_Hit(int brick_x, int brick_y, ABall_Object *ball, bool vertical_hit);
	void Redraw_Brick(int brick_x, int brick_y);
	bool Add_Falling_Letter(int brick_x, int brick_y, EBrick_Type brick_type);
	bool Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall_Object *ball, bool vertical_hit);
	void Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall_Object *ball, bool vertical_hit);
	AActive_Brick_Teleport *Select_Destination_Teleport(int source_x, int source_y);
	bool Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos);
	bool Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos);
	void Draw_Brick(HDC hdc, RECT &brick_rect, int level_x, int level_y);
	void Draw_Parachute_In_Level(HDC hdc, RECT &brick_rect);
	void Draw_Parachute_Part(HDC hdc, RECT &brick_rect, int offset, int width);
	void Delete_Objects(std::vector<AGraphics_Object *> &obj);
	void Draw_Objects(HDC hdc, RECT &paint_area, std::vector<AGraphics_Object *> &obj, int objects_max_count);
	void Act_Objects(std::vector<AGraphics_Object *> &obj);
	void Cancel_All_Activity();


	RECT Level_Rect;
	bool Need_To_Cancel_All;

	double Current_Brick_Left_X, Current_Brick_Right_X;
	double Current_Brick_Top_Y, Current_Brick_Low_Y;

	AColor Parachute_Color;

	char Current_Level[AsConfig::Level_Height][AsConfig::Level_Width];
	int Teleport_Bricks_Count;
	SPoint *Teleport_Bricks_Pos;
	AAdvertisement *Advertisement;

	static AsLevel *Level;

	std::vector<AGraphics_Object *> Active_Bricks;
	std::vector<AGraphics_Object *> Falling_Letters;
};
//------------------------------------------------------------------------------------------------------------
