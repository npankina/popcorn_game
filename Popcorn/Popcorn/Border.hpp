#pragma once
#include "Ball.hpp"
#include "Gate.hpp"

class AsBorder: public AHit_Checker, public AGame_Object
{
public:
	AsBorder();
	~AsBorder();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Redraw_Floor();
	void Open_Gate(int gate_index, bool short_open);
	bool Is_Gate_Open(int gates_index);
	void Get_Gate_Pos(int gate_index, int &gate_x_pos, int &gate_y_pos);

private:
	void Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border);
	void Draw_Floor(HDC hdc, RECT &paint_area);

	RECT Floor_Rect;
	AGate *Gates[AsConfig::Gates_Number];
};