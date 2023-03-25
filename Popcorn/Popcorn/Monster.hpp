#pragma once
#include "Config.hpp"

enum class EEye_State : unsigned char
{
	Closed,
	Opening,
	Staring,
	Closing
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

	bool Is_Active;
	void Activate(int x_pos, int y_pos);

private:
	int X_Pos, Y_Pos;
	int Start_Blink_Timeout;
	double Cornea_Height;
	RECT Monster_Rect;
	EEye_State Eye_State;

	static const int Width = 16;
	static const int Height = 16;
	static const int Blink_Stages_Count = 7;
	static const double Max_Cornea_Height;
	static const double Blink_Timeouts[Blink_Stages_Count];
	static const EEye_State Blinks_States[Blink_Stages_Count];
	
	int Blink_Ticks[Blink_Stages_Count];
};
