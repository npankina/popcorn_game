#pragma once

#include "Falling_Letter.h"
#include "Info_Panel.h"
#include "Level_Data.h"
#include <vector>

//------------------------------------------------------------------------------------------------------------
class APoint
{
public:
	APoint() : X(0), Y(0) {};
	APoint(int x, int y) : X(x), Y(y) {};

	int X, Y;
};
//------------------------------------------------------------------------------------------------------------
class AMop_Cylinder : public AGraphics_Object
{
public:
	AMop_Cylinder(int x, int y, int width, int height);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

private:
	int X_Pos, Y_Pos;
	int Width, Height;
	static const int scale_ = AsConfig::Global_Scale;
};
//------------------------------------------------------------------------------------------------------------
class AMop_Indicator : public AGraphics_Object
{
public:
	AMop_Indicator(int x, int y, int time_offset);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	static void Setup_Colors();

private:
	int X_Pos, Y_Pos;
	int Time_Offset;
	const AColor *Current_Color;
	RECT Indicator_Rect;

	static const int scale_ = AsConfig::Global_Scale;
	static const int Width = 17;
	static const int Height = 5;
	static const int Max_Fade_Step = AsConfig::FPS * 4 / 10; // 0.4 second
	static const int Normal_Timeout = AsConfig::FPS; // 1 second
	static AColor_Fade Fading_Colors;
};
//------------------------------------------------------------------------------------------------------------
class AsMop : public AGame_Object
{
public:
	~AsMop();
	AsMop();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();


	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

private:
	std::vector<AMop_Indicator *> Mop_Indicators;
	std::vector < AMop_Cylinder *> Mop_Cylinders;

	static const int Width = (AsConfig::Level_Width - 1) * AsConfig::Cell_Width + AsConfig::Brick_Width;
	static const int Height = AsConfig::Brick_Height;
	static const int Indicator_Count = 10;
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
	void Set_Current_Level(int level_number);
	bool Get_Next_Falling_Letter(int &index, AFalling_Letter **falling_letter);
	void Stop();

	static bool Has_Brick_At(int x_pos, int y_pos);
	static bool Has_Brick_At(RECT &monster_rect);

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
	AAdvertisement *Advertisement;
	std::vector<APoint> Teleport_Bricks_Pos;
	std::vector<AGraphics_Object *> Active_Bricks;
	std::vector<AGraphics_Object *> Falling_Letters;
	std::vector<ALevel_Data *> Levels_Data;
	AsMop Mop; // Швабра для очистки уровня

	static AsLevel *Level;
};
//------------------------------------------------------------------------------------------------------------