#include "Ball.h"

// AHit_Checker
//------------------------------------------------------------------------------------------------------------
bool AHit_Checker::Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x)
{// Проверяет пересечение горизонтального отрезка (проходящего от left_x до right_x через y) с окружностью радиусом radius

	double min_x, max_x;

	// x * x + y * y = R * R
	// x = sqrt(R * R - y * y)
	// y = sqrt(R * R - x * x)

	if (y > radius)
		return false;

	x = sqrt(radius * radius - y * y);

	max_x = next_x_pos + x;
	min_x = next_x_pos - x;

	if (max_x >= left_x and max_x <= right_x  or  min_x >= left_x and min_x <= right_x)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------




// ABall
const double ABall::Start_Ball_Y_Pos = 181.0;
const double ABall::Radius = 2.0 - 0.5 / AsConfig::Global_Scale;
int ABall::Hit_Checkers_Count = 0;
AHit_Checker *ABall::Hit_Checkers[] = {};
//------------------------------------------------------------------------------------------------------------
ABall::ABall()
: Ball_State(EBS_Normal), Center_X_Pos(0.0), Center_Y_Pos(Start_Ball_Y_Pos), Ball_Speed(0.0),
  Rest_Distance(0.0), Ball_Direction(0), Testing_Is_Active(false), Test_Iteration(0), Ball_Rect{},
  Prev_Ball_Rect{}, Parashute_Rect{}
{
	//Set_State(EBS_Normal, 0);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	// 1. Очищаем фон
	if (IntersectRect(&intersection_rect, &paint_area, &Prev_Ball_Rect) )
	{
		AsConfig::BG_Color.Select(hdc);
		Ellipse(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right - 1, Prev_Ball_Rect.bottom - 1);
	}

	if (Ball_State == EBS_On_Parashute)
		Draw_Parashute(hdc, paint_area);

	if (Ball_State == EBS_Lost)
		return;

	// 2. Рисуем шарик
	if (IntersectRect(&intersection_rect, &paint_area, &Ball_Rect) )
	{
		AsConfig::White_Color.Select(hdc);
		Ellipse(hdc, Ball_Rect.left, Ball_Rect.top, Ball_Rect.right - 1, Ball_Rect.bottom - 1);
	}
}
//------------------------------------------------------------------------------------------------------------
void ABall::Move()
{
	int i;
	bool got_hit;
	double next_x_pos, next_y_pos;
	double step_size = AsConfig::Moving_Step_Size;

	if (Ball_State != EBS_Normal)
		return;

	Prev_Ball_Rect = Ball_Rect;
	Rest_Distance += Ball_Speed;

	while (Rest_Distance >= step_size)
	{
		got_hit = false;

		next_x_pos = Center_X_Pos + step_size * cos(Ball_Direction);
		next_y_pos = Center_Y_Pos - step_size * sin(Ball_Direction);

		// Корректируем позицию при отражении:
		for (i = 0; i < Hit_Checkers_Count; i++)
			got_hit |= Hit_Checkers[i]->Check_Hit(next_x_pos, next_y_pos, this);

		if (! got_hit)
		{
			// Мячик продолжит движение, если не взаимодействовал с другими объектами
			Rest_Distance -= step_size;

			Center_X_Pos = next_x_pos;
			Center_Y_Pos = next_y_pos;

			if (Testing_Is_Active)
				Rest_Test_Distance -= step_size;
		}
	}

	Redraw_Ball();
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_For_Test()
{
	Testing_Is_Active = true;
	Rest_Test_Distance = 30.0;

	//Set_State(EBS_Normal, 60 + Test_Iteration, 100);
	//Ball_Direction = M_PI_4;

	//Set_State(EBS_Normal, 100 + Test_Iteration, 194);
	//Set_State(EBS_Normal, 90, 189 - Test_Iteration);
	// Ball_Direction = M_PI_4 / 4.0;
	// 
	/*Set_State(EBS_Normal, 80 + Test_Iteration, 194);
	Ball_Direction = M_PI_4;
	Ball_Speed = 1.0;*/

	Set_State(EBS_Normal, 100 + Test_Iteration, 170);
	Ball_Direction = 5.0;
	Ball_Speed = 1.0;

	++Test_Iteration;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Test_Finished()
{
	if (Testing_Is_Active)
	{
		if (Rest_Test_Distance <= 0.0)
		{
			Testing_Is_Active = false;
			Set_State(EBS_Lost, 0);
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
EBall_State ABall::Get_State()
{
	return Ball_State;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_State(EBall_State new_state, double x_pos, double y_pos)
{
	switch (new_state)
	{
	case EBS_Normal:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 3.0;
		Rest_Distance = 0.0;
		Ball_Direction = M_PI_4;
		Redraw_Ball();
		break;


	case EBS_Lost:
		Ball_Speed = 0.0;
		break;


	case EBS_On_Platform:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 0.0;
		Rest_Distance = 0.0;
		Ball_Direction = M_PI_4;
		Redraw_Ball();
		break;
	}

	Ball_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
double ABall::Get_Direction()
{
	return Ball_Direction;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_Direction(double new_direction)
{
	const double pi_2 = 2.0 * M_PI;

	while (new_direction > pi_2)
		new_direction -= pi_2;

	while (new_direction < 0.0)
		new_direction += pi_2;

	Ball_Direction = new_direction;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Reflect(bool from_horizontal)
{
	if (from_horizontal)
		Set_Direction(-Ball_Direction);
	else
		Set_Direction(M_PI - Ball_Direction);
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Up()
{
	if (Ball_Direction  >= 0.0 and Ball_Direction < M_PI)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Left()
{
	if (Ball_Direction > M_PI_2 and Ball_Direction < M_PI + M_PI_2)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_On_Parashute(int x, int y)
{
	int cell_x = AsConfig::Level_X_Offset + x * AsConfig::Cell_Width;
	int cell_y = AsConfig::Level_Y_Offset + y * AsConfig::Cell_Height;


	Ball_Direction = M_PI + M_PI_2;
	Ball_Speed = 1.0;
	Ball_State = EBS_On_Parashute;


	Parashute_Rect.left = cell_x * AsConfig::Global_Scale;
	Parashute_Rect.top = cell_y * AsConfig::Global_Scale;
	Parashute_Rect.right = Parashute_Rect.left + Parashute_Size * AsConfig::Global_Scale;
	Parashute_Rect.bottom = Parashute_Rect.top + Parashute_Size * AsConfig::Global_Scale;

	Center_X_Pos = (double)(cell_x + AsConfig::Cell_Width / 2.0) - 1.0 / (double)AsConfig::Global_Scale;
	Center_Y_Pos = (double)(cell_y + Parashute_Size) - ABall::Radius * 2.0;

	InvalidateRect(AsConfig::Hwnd, &Parashute_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Add_Hit_Checker(AHit_Checker *hit_checker)
{
	if (Hit_Checkers_Count >= sizeof(Hit_Checkers) / sizeof(Hit_Checkers[0]) )
		return;

	Hit_Checkers[Hit_Checkers_Count++] = hit_checker;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Redraw_Ball()
{
	Ball_Rect.left = (int)((Center_X_Pos - Radius) * AsConfig::Global_Scale);
	Ball_Rect.top = (int)((Center_Y_Pos - Radius) * AsConfig::Global_Scale);
	Ball_Rect.right = (int)((Center_X_Pos + Radius) * AsConfig::Global_Scale);
	Ball_Rect.bottom = (int)((Center_Y_Pos + Radius) * AsConfig::Global_Scale);

	InvalidateRect(AsConfig::Hwnd, &Prev_Ball_Rect, FALSE);
	InvalidateRect(AsConfig::Hwnd, &Ball_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw_Parashute(HDC hdc, RECT &paint_area)
{
	int scale = AsConfig::Global_Scale;
	int dome_height = (Parashute_Rect.bottom - Parashute_Rect.top) / 2;
	int arc_height = 4 * scale;
	int arc_x;
	int ball_center_x, ball_center_y, line_y;
	RECT intersection_rect, subarc_rect, other_arc;

	// 1. Очищаем фон
	if ( !IntersectRect(&intersection_rect, &paint_area, &Parashute_Rect) )
		return;

	// 1. Купол
	AsConfig::Red_Color.Select(hdc);
	Chord(hdc, Parashute_Rect.left, Parashute_Rect.top, Parashute_Rect.right - 1, Parashute_Rect.bottom - 1, 
		Parashute_Rect.right, Parashute_Rect.top + dome_height, Parashute_Rect.left, Parashute_Rect.top + dome_height);

	// 2. Арки
	arc_x = Parashute_Rect.left + 1;
	AsConfig::BG_Color.Select(hdc);

	// 2.1 Левая
	subarc_rect.left = arc_x;
	subarc_rect.top = Parashute_Rect.top + dome_height - arc_height / 2;
	subarc_rect.right = subarc_rect.left + 3 * scale;
	subarc_rect.bottom = subarc_rect.top + 4 * scale;

	Ellipse(hdc, subarc_rect.left, subarc_rect.top, subarc_rect.right - 1, subarc_rect.bottom - 1);

	// 2.2 Средняя
	other_arc = subarc_rect;

	other_arc.left = arc_x + 3 * scale;
	other_arc.right = arc_x + 11 * scale;

	Ellipse(hdc, other_arc.left, other_arc.top, other_arc.right - 1, other_arc.bottom - 1);

	// 2.3 Правая
	other_arc = subarc_rect;

	other_arc.left = arc_x + 11 * scale;
	other_arc.right = arc_x + 14 * scale;

	Ellipse(hdc, other_arc.left, other_arc.top, other_arc.right - 1, other_arc.bottom - 1);

	// 3. Стропы
	AsConfig::White_Color.Select(hdc);

	line_y = Parashute_Rect.top + dome_height;
	ball_center_x = (Parashute_Rect.left + Parashute_Rect.right) / 2;
	ball_center_y = Parashute_Rect.bottom - scale * 2;

	MoveToEx(hdc, Parashute_Rect.left, line_y, 0);
	LineTo(hdc, ball_center_x, ball_center_y);

	MoveToEx(hdc, Parashute_Rect.left + 3 * scale + 1, line_y, 0);
	LineTo(hdc, ball_center_x, ball_center_y);

	MoveToEx(hdc, Parashute_Rect.right, line_y, 0);
	LineTo(hdc, ball_center_x, ball_center_y);

	MoveToEx(hdc, Parashute_Rect.right - 4 * scale + 1, line_y, 0);
	LineTo(hdc, ball_center_x, ball_center_y);

}
//------------------------------------------------------------------------------------------------------------
