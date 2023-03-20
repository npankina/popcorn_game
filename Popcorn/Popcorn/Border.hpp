#pragma once
#include "Ball.hpp"

class AGate : public AGraphics_Object
{
public:
	AGate(int x, int y);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

private:
	void Draw_Cup(HDC hdc, bool is_top);
	void Draw_One_Edge(HDC hdc, int edge_y_offset, bool is_long);
	void Draw_Edges(HDC hdc);


	int X_Pos, Y_Pos;
	int Edge_Count;
};
//------------------------------------------------------------------------------------------------------------
class AsBorder: public AHit_Checker, public AGraphics_Object
{
public:
	AsBorder();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Redraw_Floor();

private:
	void Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border);
	void Draw_Floor(HDC hdc, RECT &paint_area);

	RECT Floor_Rect;
	AGate Gate;
};