#pragma once
#include "Falling_Letter.hpp"
#include "Laser_Beam_Set.hpp"
#include "Platform_State.hpp"
#include "Platform_Glue.hpp"

enum class EFigure_type: unsigned char
{
	Ellipse,
	Rectangle,
	Round_Rect_3x
};
//------------------------------------------------------------------------------------------------------------
class AsPlatform_Expanding
{
public:
	AsPlatform_Expanding(AsPlatform_State &platform_state);
	bool Act(double &x_pos, EPlatform_State &next_state, bool &correct_pos);
	void Draw_State(HDC hdc, double x);
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Reset();

	double Expanding_Platform_Width;

private:
	void Draw_Expanding_Platform_Ball(HDC hdc, double x, bool is_left);
	void Draw_Expanding_Truss(HDC hdc, RECT &inner_rect, bool is_left);

	AsPlatform_State *Platform_State;

	static const int Expanding_Platform_Inner_Width = 12;
	static const double Max_Expanding_Platform_Width, Min_Expanding_Platform_Width, Expanding_Platform_Width_Step;
};
//------------------------------------------------------------------------------------------------------------
class AsPlatform_Laser
{
public:
	AsPlatform_Laser(AsPlatform_State &platform_state);
	void Init(AsLaser_Beam_Set *laser_beam_set);
	bool Act(EPlatform_State &next_state, double x_pos);
	void Draw_State(HDC hdc, double x_pos, RECT &platform_rect);
	void Reset();
	void Fire(bool fire_on);
	
private:
	void Draw_Wing(HDC hdc, double x_pos, bool is_left);
	double Get_Gun_Pos(double platform_x_pos, bool is_left);
	void Draw_Inner_part(HDC hdc, double x_pos);
	void Draw_Leg(HDC hdc, double x_pos, bool is_left);
	void Draw_Cabin(HDC hdc, double x_pos);
	void Draw_Expanding_Figure(HDC hdc, EFigure_type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height);
	int Get_Expanding_Value(double start, double end, double ratio);

	AsPlatform_State *Platform_State;
	AsLaser_Beam_Set *Laser_Beam_Set; // UNO

	bool Enable_Laser_Firing;
	int Laser_Transformation_Step;
	int Last_Laser_Shot_Tick;

	static const int Max_Laser_Transformation_Step = 20;
	static const int Laser_Shot_Timeout = AsConfig::FPS / 2; // 2 раза в секунду будет стрелять лазер
};
//------------------------------------------------------------------------------------------------------------
class AsPlatform: public AHit_Checker, public AMover, public AGraphics_Object
{
public:
	~AsPlatform();
	AsPlatform();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Init(AsBall_Set *ball_set, AsLaser_Beam_Set *laser_beam_set);
	EPlatform_State Get_State();
	void Set_State(EPlatform_State new_state);
	void Set_State(EPlatform_Substate_Regular new_regular_state);
	bool Has_State(EPlatform_Substate_Regular regular_state);
	void Redraw_Platform();
	void Move(bool to_left, bool key_down);
	void On_Space_Key(bool key_down);
	bool Hit_By(AFalling_Letter *falling_letter);
	double Get_Middle_Pos();

	double X_Pos;

private:
	bool Set_Transformation_State(EPlatform_State new_state, EPlatform_Transformation &transformation_state);
	void Act_For_Meltdown_State();
	void Act_For_Rolling_State();
	void Draw_Normal_State(HDC hdc, RECT &paint_area);
	void Draw_Meltdown_State(HDC hdc, RECT &paint_area);
	void Draw_Rolling_State(HDC hdc, RECT &paint_area);
	void Draw_Roll_In_State(HDC hdc, RECT &paint_area);
	bool Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall *ball);
	bool Get_Platform_Image_Stroke_Color(int x, int y, const AColor **color, int &stroke_len);
	void Get_Normal_Platform_Image(HDC hdc);
	double Get_Current_Platform_Width();
	bool Correct_Platform_Pos();

	AsPlatform_State Platform_State;
	AsPlatform_Glue Platform_Glue;
	AsPlatform_Expanding Platform_Expanding;
	AsPlatform_Laser Platform_Laser;

	bool Left_Key_Down, Right_Key_Down;
	int Inner_Width;
	int Rolling_Step;
	int Last_Redraw_Timer_Tick;
	double Speed;
	AsBall_Set *Ball_Set;

	int Normal_Platform_Image_Width, Normal_Platform_Image_Height;
	int *Normal_Platform_Image;  // Пиксели изображения платформы на фоне

	int Meltdown_Platform_Y_Pos[AsConfig::Platform_Normal_Width * AsConfig::Global_Scale];

	RECT Platform_Rect, Prev_Platform_Rect;

	static const int Meltdown_Speed = 3;
	static const int Max_Rolling_Step = 16;
	static const int Roll_In_Platform_End_X_Pos = 99;
	static const int Rolling_Platform_Speed = 3;
	static const int X_Step = 6;
};