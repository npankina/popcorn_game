#pragma once
#include "Indicator.h"

//------------------------------------------------------------------------------------------------------------
class AsInfo_Panel : public AGame_Object
{
public:
	~AsInfo_Panel();
	AsInfo_Panel();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT& paint_area);
	virtual void Draw(HDC hdc, RECT& paint_area);
	virtual bool Is_Finished();

	void Init();

	static void Update_Score(EScore_Event_Type event_type);
	void Increase_Life_Count();
	bool Decrease_Life_Count();

	AIndicator Floor_Indicator, Monster_Indicator;

private:
	void Choose_Font();
	void Draw_Extra_Life(HDC hdc, int x_pos, int y_pos);
	void Show_Extra_Lifes(HDC hdc);
	void Draw_String(HDC hdc, RECT &rect, AString &name_str, bool draw_name);

	HFONT Logo_Pop_Font, Logo_Corn_Font, Player_Name_Font, Score_Font;
	AColor *Dark_Blue;
	AFalling_Letter Letter_P, Letter_G, Letter_M;
	AString Player_Name;
	int Extra_Lives_Count;

	static RECT Logo_Rect, Data_Rect; // Область данных на инфо панели под логотипом, необходимы для перерисовки экрана
	static int Current_Score;
	static const int Score_X = 208;
	static const int Score_Y = 108;
	static const int Score_Width = 110;
	static const int Score_Height = 90;
	static const int Score_Value_Offset = 20;
	static const int Logo_X_Pos = 212;
	static const int Logo_Y_Pos = 0;
	static const int Shadow_X_Offset = 5;
	static const int Shadow_Y_Offset = 5;
	static const int Indicator_Y_Offset = 55;
};
//------------------------------------------------------------------------------------------------------------