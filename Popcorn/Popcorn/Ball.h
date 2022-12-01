#pragma once

#include "Config.h"
#include "Level.h"

//------------------------------------------------------------------------------------------------------------
class ABall
{
public:
	ABall();

	void Init();
	void Draw(HDC hdc, RECT &paint_area);
	void Move(ALevel *level, int platform_x_pos, int platform_width);

private:
	HPEN Ball_Pen;
	HBRUSH Ball_Brush;

	RECT Ball_Rect, Prev_Ball_Rect;

	int Ball_X_Pos, Ball_Y_Pos;
	double Ball_Speed = 3.0;
	double Ball_Direction = M_PI - M_PI_4;
};