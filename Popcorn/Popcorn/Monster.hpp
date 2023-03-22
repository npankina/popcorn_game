#pragma once
#include "Config.hpp"

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
	int Cornea_Height;
	RECT Monster_Rect;

	static const int Width = 16;
	static const int Height = 16;
	static const int Max_Cornea_Height = 11;
};
