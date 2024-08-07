#pragma once
#include "Mop_Indicator.h"
#include "Mop_Cylinder.h"

class AsMop : public AGame_Object
{
public:
	~AsMop();
	AsMop();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();


	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Erase_Level();
	void Set_Mop();

private:
	int Y_Pos;
	int Start_Tick;
	bool Acting;
	RECT Mop_Rect, Prev_Mop_Rect;
	std::vector<AMop_Indicator *> Mop_Indicators;
	std::vector < AMop_Cylinder *> Mop_Cylinders;

	static const int Width = (AsConfig::Level_Width - 1) * AsConfig::Cell_Width + AsConfig::Brick_Width;
	static const int Height = AsConfig::Brick_Height;
	static const int Indicator_Count = 10;
	static const int Expansion_Timeout = AsConfig::FPS * 2;
	static const int scale_ = AsConfig::Global_Scale;
};