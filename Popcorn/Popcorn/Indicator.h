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
class AIndicator : public AGraphics_Object
{
public:
	AIndicator(EMessage_Type type, int x_pos, int y_pos);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Restart();
	void Reset();
	void Cancel();

private:
	int X_Pos, Y_Pos;
	int End_Tick;
	RECT Indicator_Rect;
	EMessage_Type Message_Type;
	bool Need_To_Notify;

	static const int Width = 12;
	static const int Height = 30;
	static const int Inner_Width = Width - 2;
	static const int Inner_Height = Height - 2;
	static const int Indicator_Timeout = 50;
};
//------------------------------------------------------------------------------------------------------------