#pragma once
#include "Falling_Letter.h"

//------------------------------------------------------------------------------------------------------------
enum class EScore_Event_Type : unsigned char
{
	Hit_Brick,
	Hit_Monster,
	Catch_Letter
};
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

private:
	void Choose_Font();
	void Draw_Extra_Life(HDC hdc, int x_pos, int y_pos);
	void Show_Extra_Lifes(HDC hdc);
	void Draw_String(HDC hdc, RECT &rect, AString &name_str, bool draw_name);

	HFONT Logo_Pop_Font, Logo_Corn_Font, Player_Name_Font, Score_Font;
	AColor *Shadow_Color, *Highlight_Color, *Dark_Blue;

	AFalling_Letter Letter_P, Letter_G, Letter_M;
	AString Player_Name;
	static RECT Logo_Rect, Data_Rect; // Область данных на инфо панелипод логотипом, необходимы для перерисовки экрана

	static int Current_Score;
	static int Extra_Lives_Count;

	static const int Score_X = 208;
	static const int Score_Y = 108;
	static const int Score_Width = 110;
	static const int Score_Height = 90;
	static const int Score_Value_Offset = 20;
};