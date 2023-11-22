#pragma once

#include "Border.h"

enum class EEye_State : unsigned char
{
	Closed,
	Opening,
	Staring,
	Closing
};

enum class EMonster_State : unsigned char
{
	Missing,
	Alive,
	Destroing
};

enum class EExplosive_Ball_State: unsigned char
{
	Idle,
	Charging,
	Expanding,
	Fading
};

class AExplosive_Ball : public AGraphics_Object
{
public:
	AExplosive_Ball();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Explode(int x_pos, int y_pos, int size, bool is_red, int time_offset, int step_count);

	static void Setup_Colors();

private:
	void Update_Ball_Rect();

	EExplosive_Ball_State Explosive_Ball_State;
	bool Is_Red;
	int X_Pos, Y_Pos;
	int Step_Count;
	int Start_Expanding_Tick, Start_Fading_Tick;
	int Time_Offset;
	double Size, Max_Size, Size_Step;
	RECT Ball_Rect;

	static const int Fading_Timeout = AsConfig::FPS;
	static const int Max_Fade_Step = AsConfig::FPS;
	static AColor Fading_Red_Colors[Max_Fade_Step];
	static AColor Fading_Blue_Colors[Max_Fade_Step];
};

class AMonster: public AGame_Object
{
public:
	AMonster();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	bool Is_Active();
	void Activate(int x_pos, int y_pos, bool moving_right);
	void Destroy();

	static const int Width = 16;
	static const int Height = 16;

private:
	void Draw_Alive(HDC hdc);
	void Draw_Destroing(HDC hdc, RECT& paint_area);
	void Act_Alive();
	void Act_Destroing();
	void Redraw_Monster();

	EEye_State Eye_State;
	EMonster_State Monster_State;
	double X_Pos, Y_Pos;
	double Speed, Direction;
	double Cornea_Height;
	int Start_Blink_Timeout, Total_Animation_Timeout;
	int Next_Direction_Switch_Tick;
	RECT Monster_Rect, Prev_Monster_Rect;

	static const int Blink_Stages_Count = 7;
	static const int Explosive_Balls_Count = 20;

	int Blink_Ticks[Blink_Stages_Count];
	AExplosive_Ball Explosive_Balls[Explosive_Balls_Count];

	static const double Max_Cornea_Height;
	static const double Blink_Timeouts[Blink_Stages_Count];
	static const EEye_State Blink_States[Blink_Stages_Count];	
};

enum class EMonster_Set_State : unsigned char
{
	Idle, // ничего не делаем
	Selecting_Next_Gate,
	Waiting_Gate_Openening,
	Waiting_Gate_Closing
};

class AsMonster_Set: public AGame_Objects_Set
{
public:
	AsMonster_Set();
	virtual void Act();
	void Init(AsBorder *border);
	void Emit_At_Gate(int gate_index);

private:
	bool Get_Next_Game_Object(int &index, AGame_Object **game_obj); // iterator

	AsBorder *Border; // UNO
	static const int Max_Monsters_Count = 10;
	AMonster Monsters[Max_Monsters_Count];
	EMonster_Set_State Monster_Set_State;
};