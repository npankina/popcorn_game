﻿
#pragma once

#include "Falling_Letter.h"
#include "Indicator.h"

//------------------------------------------------------------------------------------------------------------
class ALabel
{
public:
	ALabel(int x_pos, int y_pos, int width, int height, const AFont &font, const AColor &color);

	void Draw(HDC hdc);

	RECT Content_Rect;
	AString Content;

private:
	int X_Pos, Y_Pos;
	int Width, Height;
	const AFont &Font;
	const AColor &Color;

	static const int scale_ = AsConfig::Global_Scale;
};
//------------------------------------------------------------------------------------------------------------
class AsInfo_Panel: public AGame_Object
{
public:
	AsInfo_Panel();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Init();
	void Increase_Life_Count();
	bool Decrease_Life_Count();

	static void Update_Score(EScore_Event_Type event_type);

	AIndicator Floor_Indicator, Monster_Indicator;

private:
	void Choose_Font();
	void Show_Extra_Lives(HDC hdc);
	void Draw_Extra_Life(HDC hdc, int x_pos, int y_pos);
	//void Draw_String(HDC hdc, RECT &rect, AString &str, bool draw_name);

	int Extra_Lives_Count;
	//HFONT Logo_Pop_Font, Logo_Corn_Font;
	AColor Dark_Blue;

	AFalling_Letter Letter_P, Letter_G, Letter_M;

	//AString Player_Name;
	ALabel Player_Name_Label, Score_Label;

	static RECT Logo_Rect;  // Область логотипа
	static RECT Data_Rect;  // Область данных (имени игрока, счёта и индикаторов)

	static int Score;

	static const int Logo_X_Pos = 212;
	static const int Logo_Y_Pos = 0;
	static const int Shadow_X_Offset = 5;
	static const int Shadow_Y_Offset = 5;
	static const int Score_X = 208;
	static const int Score_Y = 108;
	static const int Score_Width = 110;
	static const int Score_Height = 90;
	static const int Score_Value_Offset = 20;
	static const int Indicator_Y_Offset = 55;

	static const int scale_ = AsConfig::Global_Scale;
};
//------------------------------------------------------------------------------------------------------------
