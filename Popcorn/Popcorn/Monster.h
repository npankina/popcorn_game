﻿#pragma once

#include "Explosive_Ball.h"
#include "Info_Panel.h"
#include "Gate.h"
#include "Level.h"

//------------------------------------------------------------------------------------------------------------
enum class EEye_State : unsigned char
{
	Closed,
	Opening,
	Staring,
	Closing
};
//------------------------------------------------------------------------------------------------------------
enum class EMonster_State : unsigned char
{
	Missing,
	Emitting,
	Alive,
	Destroing
};
//------------------------------------------------------------------------------------------------------------
class AMonster : public AHit_Checker, public AGame_Object
{
public:
	virtual ~AMonster(); // чтобы уничтожать объект производного типа имея указатель на базовый класс
	AMonster();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Activate(int x_pos, int y_pos, bool moving_right);
	void Destroy();
	void Set_Freeze_State(bool freeze);

	static const int Width = 16;
	static const int Height = 16;

protected:
	virtual void Draw_Alive(HDC hdc) = 0; // для реализации в производном классе
	virtual void Act_Alive() = 0;
	virtual void On_Activation() = 0;

	EMonster_State Monster_State;
	RECT Monster_Rect, Prev_Monster_Rect;
	int Next_Direction_Switch_Tick;
	int Alive_Timer_Tick;

	double Direction;
	double X_Pos, Y_Pos;

private:
	void Draw_Destroing(HDC hdc, RECT &paint_area);
	void Act_Destroing();
	void Get_Monster_Rect(double x_pos, double y_pos, RECT &rect);
	void Redraw_Monster();
	void Change_Direction();

	double Speed, Prev_Speed;
	bool Need_To_Freeze;

	std::vector<AExplosive_Ball> Explosive_Balls;
	static const int Explosive_Balls_Count = 20;
};
//------------------------------------------------------------------------------------------------------------
class AMonster_Eye : public AMonster
{
public:
	AMonster_Eye();
private:
	virtual void Draw_Alive(HDC hdc);
	virtual void Act_Alive();
	virtual void On_Activation();

	EEye_State Eye_State;
	double Cornea_Height;
	int Start_Blink_Timeout, Total_Animation_Timeout;

	std::vector<int> Blink_Ticks;

	static const int Blink_Stages_Count = 7;
	static const double Max_Cornea_Height;
	static const double Blink_Timeouts[Blink_Stages_Count];
	static const EEye_State Blink_States[Blink_Stages_Count];
};
//------------------------------------------------------------------------------------------------------------
class AMonster_Comet : public AMonster
{
public:
	AMonster_Comet();

private:
	virtual void Clear(HDC hdc, RECT& paint_area); // переопределен метод базового класса
	virtual void Draw_Alive(HDC hdc);
	virtual void Act_Alive();
	virtual void On_Activation();

	double Current_Angle;
	int Ticks_Per_Rotation;

	static const int Min_Ticks_Per_Rotation = AsConfig::FPS * 2;
	static const int Max_Ticks_Per_Rotation = AsConfig::FPS * 4;
};
//------------------------------------------------------------------------------------------------------------