#pragma once
#include "Mop_Indicator.h"
#include "Mop_Cylinder.h"


//------------------------------------------------------------------------------------------------------------
enum class EMop_State : unsigned char
{
	Idle,

	Clearing,
	Clear_Done,
	Showing,
	
	Ascending,
	Descending,
	Descend_Done
};
//------------------------------------------------------------------------------------------------------------
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

	void Activate(bool is_cleaning);
	void Set_Mop();
	void Clear_Area(HDC hdc);
	EMop_State Get_State() { return Mop_State;  }
	

private:
	int Get_Cylinders_Height();
	void Act_Lifting(bool is_lift_up);


	int Y_Pos, Max_Y_Pos;
	int Lifting_Height;
	int Start_Tick;
	RECT Mop_Rect, Prev_Mop_Rect;
	EMop_State Mop_State;
	std::vector<AMop_Indicator *> Mop_Indicators;
	std::vector < AMop_Cylinder *> Mop_Cylinders;

	static const int Width = (AsConfig::Level_Width - 1) * AsConfig::Cell_Width + AsConfig::Brick_Width;
	static const int Height = AsConfig::Brick_Height;
	static const int Indicator_Count = 10;
	static const int Expansion_Timeout = AsConfig::FPS * 2;
	static const int Lifting_Timeout = AsConfig::FPS;
	static const int scale_ = AsConfig::Global_Scale;
};