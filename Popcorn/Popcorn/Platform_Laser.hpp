#pragma once
#include "Platform_State.hpp"
#include "Laser_Beam_Set.hpp"

enum class EFigure_type: unsigned char
{
	Ellipse,
	Rectangle,
	Round_Rect_3x
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