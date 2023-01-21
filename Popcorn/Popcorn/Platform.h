﻿#pragma once

#include "Falling_Letter.h"

//------------------------------------------------------------------------------------------------------------
enum EPlatform_State
{
	EPS_Missing,
	EPS_Ready,
	EPS_Normal,
	EPS_Pre_Meltdown,
	EPS_Meltdown,
	EPS_Roll_In,
	EPS_Expand_Roll_In,
	EPS_Glue_Init,
	EPS_Glue,
	EPS_Glue_Finalize
};
//------------------------------------------------------------------------------------------------------------
enum EPlatform_Moving_State
{
	EPMS_Stopping,
	EPMS_Stop,
	EPMS_Moving_Left,
	EPMS_Moving_Right
};
//------------------------------------------------------------------------------------------------------------
class AsPlatform: public AHit_Checker, public AMover, public AGraphics_Object
{
public:
	~AsPlatform();
	AsPlatform();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);
	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Begin_Movement();
	virtual void Finish_Movement();

	virtual void Act();
	virtual void Draw(HDC hdc, RECT& paint_area);
	virtual void Clear(HDC hdc, RECT& paint_area);
	virtual bool Is_Finished();

	EPlatform_State Get_State();
	void Set_State(EPlatform_State new_state);
	void Redraw_Platform();
	void Move(bool to_left, bool is_key_down);
	bool Hit_By(AFalling_Letter *falling_letter);
	double Get_Middle_Pos();
	void Get_Normal_Platform_Image(HDC hdc);

	int Width;

private:
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Draw_Normal_State(HDC hdc, RECT &paint_area);
	void Draw_Meltdown_State(HDC hdc, RECT &paint_area);
	void Draw_Roll_In_State(HDC hdc, RECT &paint_area);
	void Draw_Expanding_Roll_In_State(HDC hdc, RECT &paint_area);
	void Draw_Glue_State(HDC hdc, RECT &paint_area);
	void Draw_Glue_Spot(HDC hdc, int width, int height, int x_offset = 0);
	bool Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall *ball);
	bool Get_Platform_Image_Stroke_Color(int x, int y, const AColor **color, int &stroke_len);

	EPlatform_State Platform_State;
	EPlatform_Moving_State Platform_Moving_State;

	double X_Pos;

	int Inner_Width;
	int Rolling_Step;
	double Speed; // количество пикселов на которые смещается платформа за кадр
	bool Left_Key_Down, Right_Key_Down;
	
	int Normal_Platform_Image_Width, Normal_Platform_Image_Height;
	int *Normal_Platform_Image; // указатель на массив

	static const int Normal_Width = 28;
	int Meltdown_Platform_Y_Pos[Normal_Width * AsConfig::Global_Scale];

	RECT Platform_Rect, Prev_Platform_Rect;

	AColor Highlight_Color, Platform_Circle_Color, Platform_Inner_Color;

	static const int Height = 7;
	static const int Circle_Size = 7;
	static const int X_Step = 6;
	static const int Normal_Platform_Inner_Width = Normal_Width - Circle_Size;
	static const int Meltdown_Speed = 3;
	static const int Max_Rolling_Step = 16;
	static const int Roll_In_Platform_End_X_Pos = 99;
	static const int Rolling_Platform_Speed = 3;
};
//------------------------------------------------------------------------------------------------------------
