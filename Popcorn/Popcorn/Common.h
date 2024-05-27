#pragma once

#include <Windows.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <vector>

//------------------------------------------------------------------------------------------------------------
enum class EBall_State : unsigned char
{
	Disabled,  // Отключён (не рисуется, не перемещается и не взаимодействует)

	Normal,
	Lost,
	On_Platform,
	On_Parachute,
	Off_Parachute,
	Teleporting

};
//------------------------------------------------------------------------------------------------------------
class ABall_Object
{ // интерфейс для мячика, чтобы не создавалась циклическая зависимость
public:
	virtual double Get_Direction() = 0;
	virtual void Set_Direction(double new_direction) = 0;
	virtual EBall_State Get_State() = 0;
	virtual void Set_State(EBall_State new_state, double x_pos = 0, double y_pos = 0) = 0;
	virtual void Reflect(bool from_horizontal) = 0;
	virtual void Draw_Teleporting(HDC hdc, int step) = 0;
	virtual void Set_On_Parachute(int brick_x, int brick_y) = 0;
	virtual bool Is_Moving_Up() = 0;
	virtual bool Is_Moving_Left() = 0;
	virtual void Get_Center(double &x_pos, double &y_pos) = 0;
};
//------------------------------------------------------------------------------------------------------------
class AMover
{
public:
	virtual ~AMover();

	virtual void Begin_Movement() = 0;
	virtual void Finish_Movement() = 0;
	virtual void Advance(double max_speed) = 0;
	virtual double Get_Speed() = 0;

};
//------------------------------------------------------------------------------------------------------------
class AGraphics_Object
{
public:
	virtual ~AGraphics_Object();

	virtual void Act() = 0;
	virtual void Clear(HDC hdc, RECT &paint_area) = 0;
	virtual void Draw(HDC hdc, RECT &paint_area) = 0;
	virtual bool Is_Finished() = 0;
};
//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	~AColor();
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);
	AColor(const AColor &color, int pen_size);
	AColor(unsigned char r, unsigned char g, unsigned char b, int pen_size);
	AColor(const AColor &pen_color, const AColor &brush_color, int pen_size);
	void operator=(const AColor &another);

	int Get_RGB() const;
	void Select(HDC hdc) const;
	void Select_Pen(HDC hdc) const;
	HBRUSH Get_Brush() const;

	unsigned char R, G, B;

private:
	HPEN Pen;
	HBRUSH Brush;
};
//------------------------------------------------------------------------------------------------------------
class AGame_Object : public AMover, public AGraphics_Object
{
};
//------------------------------------------------------------------------------------------------------------
class AGame_Objects_Set: public AGame_Object
{
public:
	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();


	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

protected:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **game_obj) = 0; // iterator
};
//------------------------------------------------------------------------------------------------------------
class AString
{
public:
	AString();
	AString(const wchar_t *str);

	void Append(int value);
	const wchar_t *Get_Content();
	int Get_Length();

private:
	std::wstring Content;
};
//------------------------------------------------------------------------------------------------------------