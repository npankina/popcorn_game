#pragma once

#include "Falling_Letter.h"
#include "Info_Panel.h"
#include "Level_Data.h"
#include "Mop.h"
#include <vector>

//------------------------------------------------------------------------------------------------------------
class APoint
{
public:
	APoint();
	APoint(int x, int y);

	int X, Y;
};
//------------------------------------------------------------------------------------------------------------
enum class ELevel_Title_State: unsigned char
{
	Missing,
	Showing,
	Hiding
};
//------------------------------------------------------------------------------------------------------------
class AsLevel_Title: public AGraphics_Object
{
public:
	AsLevel_Title();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Show(int level_number);
	void Hide();

private:
	//bool Is_Visible;
	ELevel_Title_State Level_Title_State;
	RECT Title_Rect;
	ALabel Level_Name, Level_Number;

	static const int Width = 96;
	static const int Height = 14;
	static const int X_Pos = AsConfig::Max_X_Pos / 2 - Width / 2;
	static const int Y_Pos = 150;
	static const int scale_ = AsConfig::Global_Scale;
};
//------------------------------------------------------------------------------------------------------------
class AFinal_Letter : public AGraphics_Object
{
public:
	AFinal_Letter(double x_pos, double y_pos, const wchar_t letter);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	double X_Pos, Y_Pos;

private:
	wchar_t Letter;
};
//------------------------------------------------------------------------------------------------------------
enum class EGame_Title_State: unsigned char
{
	Idle,

	Game_Over_Descent,
	Game_Over_Destroy,

	Game_Won_Descent,
	Game_Won_Animate,

	Finished
};
//------------------------------------------------------------------------------------------------------------
class AsGame_Title : public AGraphics_Object
{
public:
	AsGame_Title();
	~AsGame_Title();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Show(bool is_over);
	bool Is_Visible();

private:
	RECT Title_Rect, Prev_Title_Rect;
	EGame_Title_State Game_Title_State;
	int Start_Tick;
	std::vector<AFinal_Letter *> Title_Letters;

	static const int Descent_Timeout = AsConfig::FPS * 6; // 6 seconds to descending
	static const int Height = 32;
	static const double Low_Y_Pos;
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
	void Mop_Level(int next_level);
	void Mop_Next_Level();
	bool Can_Mop_Next_Level();
	bool Is_Level_Mopping_Done();
	void Show_Title();
	void Hide_Title();

	static bool Has_Brick_At(int level_x, int level_y);
	static bool Has_Brick_At(RECT &monster_rect);

	AsGame_Title Game_Title; // Табличка финальных титров

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
	void Delete_Objects(std::vector<AGraphics_Object *> &falling_letters);
	void Act_Objects(std::vector<AGraphics_Object *> &falling_letters);
	void Cancel_All_Activity();

	bool Need_To_Cancel_All;
	int Next_Level_Number, Current_Level_Number;
	int Available_Bricks_Count;

	double Current_Brick_Left_X, Current_Brick_Right_X;
	double Current_Brick_Top_Y, Current_Brick_Low_Y;

	AColor Parachute_Color;
	RECT Level_Rect;

	char Current_Level[AsConfig::Level_Height][AsConfig::Level_Width];
	std::vector<AGraphics_Object *> Active_Bricks;
	std::vector<AGraphics_Object *> Falling_Letters;
	std::vector<APoint> Teleport_Bricks_Pos;
	AAdvertisement *Advertisement;
	std::vector<ALevel_Data *> Levels_Data;
	AsMop Mop;  // "Швабра", очищающая уровень
	AsLevel_Title Level_Title;  // Табличка с номером уровня

	static AsLevel *Level;
	static const int scale_ = AsConfig::Global_Scale;
};
//------------------------------------------------------------------------------------------------------------
