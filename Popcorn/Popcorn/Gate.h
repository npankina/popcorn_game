#pragma once
#include "Config.h"

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
	AGate(int x, int y, int level_x_pos = -1, int level_y_pos = -1);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Open_Gate(bool short_open);
	bool Is_Opened();
	bool Is_Closed();
	void Get_Y_Size(int &gate_top_y, int &gate_low_y);
	void Get_Pos(int &gate_x_pos, int &gate_y_pos);

	int Level_X_Pos, Level_Y_Pos;

	static const int Width = 6;
	static const int Height = 19;

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

	static const int Short_Opening_Timeout = AsConfig::FPS / 2; // 1 сек.
	static const int Long_Opening_Timeout = AsConfig::FPS / 2;
	static const double Max_Gap_Short_Height, Max_Gap_Long_Height;
	static const double Gap_Height_Short_Step, Gap_Height_Long_Step;
};