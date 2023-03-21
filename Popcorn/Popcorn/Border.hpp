﻿#pragma once
#include "Ball.hpp"

enum class EGate_State : unsigned char
{
	Closed,
	Short_Open,
	Long_Open
};
//------------------------------------------------------------------------------------------------------------
enum class EGate_Transformation : unsigned char
{
	Unknown,

	Init,
	Active,
	Finalize
};
//------------------------------------------------------------------------------------------------------------
class AGate : public AGraphics_Object
{
public:
	AGate(int x, int y);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Open_Gate(bool short_open);
	bool Is_Opened();
	void Get_Y_Size(int &gate_top_y, int &gate_low_y);

private:
	void Draw_Cup(HDC hdc, bool is_top);
	void Draw_Discharge(HDC hdc);
	void Draw_One_Edge(HDC hdc, int edge_y_offset, bool is_long);
	void Draw_Red_Edge(HDC hdc, int edge_y_offset, bool is_long, bool has_highlight);
	void Draw_Edges(HDC hdc);
	void Draw_Long_Opening_Edges(HDC hdc);
	void Draw_Short_Opening_Edges(HDC hdc);
	bool Act_For_Short_Open();
	bool Act_For_Long_Open(bool &correct_pos);
	void Redraw_Gate();

	EGate_State Gate_State;
	EGate_Transformation Gate_Transformation;
	int X_Pos;
	double Y_Pos, Origin_Y_Pos;
	int Edge_Count;
	int Gate_Close_Tick;
	double Gap_Height;
	RECT Gate_Rect;

	static const int Width = 6;
	static const int Height = 19;
	static const int Short_Opening_Timeout = AsConfig::FPS / 2; // 1 сек.
	static const int Long_Opening_Timeout = AsConfig::FPS / 2;
	static const double Max_Gap_Short_Height, Max_Gap_Long_Height;
	static const double Gap_Height_Short_Step, Gap_Height_Long_Step;
};
//------------------------------------------------------------------------------------------------------------
class AsBorder: public AHit_Checker, public AGraphics_Object
{
public:
	AsBorder();
	~AsBorder();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Redraw_Floor();
	void Open_Gate(int gate_index, bool short_open);
	bool Is_Gate_Open(int gates_index);

private:
	void Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border);
	void Draw_Floor(HDC hdc, RECT &paint_area);

	RECT Floor_Rect;
	AGate *Gates[AsConfig::Gates_Number];
};