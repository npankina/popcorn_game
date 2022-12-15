#pragma once
#include "Config.h"
#include "Active_Brick.h"


//------------------------------------------------------------------------------------------------------------
enum ELetter_Type
{
	ELT_None,

	ELT_O
};
//------------------------------------------------------------------------------------------------------------
enum EFalling_Letter_State
{
	EFLS_Normal,
	EFLS_Finalising, // начинаем удалять объект
	EFLS_Finished	// можно удалять объект
};
//------------------------------------------------------------------------------------------------------------
class AFalling_Letter : public AGraphics_Object
{
public:
	AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Get_Letter_Cell(RECT &rect);
	void Finalize();
	
	const ELetter_Type Letter_Type;

private:
	void Draw_Brick_Letter(HDC hdc);
	void Set_Brick_Letter_Colors(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush);

	EBrick_Type Brick_Type;
	EFalling_Letter_State Falling_Letter_State;
	RECT Letter_Cell, Prev_Letter_Cell;
	int X, Y;
	int Rotation_Step;
	int Next_Rotation_Tick;

	static const int Ticks_Per_Step = 4;
};
