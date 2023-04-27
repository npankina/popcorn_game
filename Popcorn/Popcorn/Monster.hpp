#pragma once
#include "Config.hpp"

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

class AExplosive_Ball : public AGraphics_Object
{
public:
	AExplosive_Ball();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Explode(int x_pos, int y_pos, int size, int step_count);

private:
	int X_Pos, Y_Pos;
	int Size;
	int Step_Count;
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
	void Activate(int x_pos, int y_pos);
	void Destroy();

private:
	void Draw_Alive(HDC hdc);
	void Draw_Destroing(HDC hdc);

	int X_Pos, Y_Pos;
	int Start_Blink_Timeout;
	int Total_Animation_Timeout;
	double Cornea_Height;
	EEye_State Eye_State;
	EMonster_State Monster_State;
	RECT Monster_Rect;


	static const int Width = 16;
	static const int Height = 16;
	static const int Blink_Stages_Count = 7;
	static const int Explosive_Balls_Count = 10;
	static const double Max_Cornea_Height;
	static const double Blink_Timeouts[Blink_Stages_Count];
	static const EEye_State Blinks_States[Blink_Stages_Count];
	
	int Blink_Ticks[Blink_Stages_Count];
	AExplosive_Ball Explosive_Balls[Explosive_Balls_Count];
};