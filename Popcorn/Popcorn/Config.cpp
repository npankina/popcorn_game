#include "Config.h"

bool AsConfig::Level_Has_Floor = true;
int AsConfig::Current_Timer_Tick = 0;

const AColor AsConfig::BG_Color(6, 30, 82);
const AColor AsConfig::Red_Color(230, 25, 229);
const AColor AsConfig::Blue_Color(0, 255, 255);
const AColor AsConfig::White_Color(255, 255, 255);
const AColor AsConfig::Letter_Color(White_Color, Global_Scale);
const AColor AsConfig::Gate_Color(White_Color, Global_Scale);
const AColor AsConfig::Laser_Color(White_Color, Global_Scale);
const AColor AsConfig::Unbreakable_Blue_Highlight(Blue_Color, Global_Scale);
const AColor AsConfig::Unbreakable_Red_Highlight(Red_Color, 3 * Global_Scale);
const AColor AsConfig::Teleport_Portal_Color(Blue_Color, BG_Color, Global_Scale);
const AColor AsConfig::Advertisement_Blue_Table(0, 159, 159, Global_Scale);
const AColor AsConfig::Advertisement_Red_Table(Red_Color, 2 * Global_Scale);
const AColor AsConfig::Platform_Inner_Color(0, 255, 255);
const AColor AsConfig::Platform_Circle_Color(230, 25, 229);
const AColor AsConfig::Highlight_Color(255, 255, 255);
const AColor AsConfig::Truss_Color(Platform_Inner_Color, Global_Scale);
const AColor AsConfig::Gun_Color(Highlight_Color, Global_Scale);
const AColor AsConfig::Monster_Dark_Red_Color(153, 17, 153);
const AColor AsConfig::Monster_Cornea_Color(BG_Color, White_Color, 2);
const AColor AsConfig::Monster_Iris_Color(BG_Color, Blue_Color, 2);
const AColor AsConfig::BG_Outcome_Color(BG_Color, 2);
const AColor AsConfig::Explosion_Red_Color(AsConfig::White_Color, AsConfig::Red_Color, 0);
const AColor AsConfig::Explosion_Blue_Color(AsConfig::White_Color, AsConfig::Blue_Color, 0);


HWND AsConfig::Hwnd;

const double AsConfig::D_Global_Scale = (double)Global_Scale;
const double AsConfig::Moving_Step_Size = 1.0 / Global_Scale;
const double AsConfig::Start_Ball_Y_Pos = 184.0;
const double AsConfig::Ball_Acceleration = 1.001;
const double AsConfig::Normal_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;

//------------------------------------------------------------------------------------------------------------
void AsConfig::Throw()
{
	throw 22;
}
//------------------------------------------------------------------------------------------------------------




// AsTools
//------------------------------------------------------------------------------------------------------------
int AsTools::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, .. range - 1]

	return rand() * range / RAND_MAX;
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Round_Rect(HDC hdc, RECT &rect, int corner_radius)
{
	int radius = corner_radius * AsConfig::Global_Scale;

	RoundRect(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1, radius, radius);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Invalidate_Rect(RECT &rect)
{
	InvalidateRect(AsConfig::Hwnd, &rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Rect(HDC hdc, RECT &rect, const AColor &color)
{
	color.Select(hdc);
	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Rect(HDC hdc, int x, int y, int width, int height, const AColor &color)
{
	RECT rect{};

	const int scale = AsConfig::Global_Scale;

	rect.left = x * scale;
	rect.top = y * scale;
	rect.right = rect.left + width * scale;
	rect.bottom = rect.top + height * scale;

	color.Select(hdc);
	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Ellipse(HDC hdc, RECT &rect, const AColor &color)
{
	color.Select(hdc);
	::Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
unsigned char AsTools::Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step, int max_step)
{
	return color - step * (color - bg_color) / (max_step - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Get_Fading_Color(const AColor &origin_color, int step, AColor &result_color, int max_step)
{
	unsigned char r, g, b;

	r = Get_Fading_Channel(origin_color.R, AsConfig::BG_Color.R, step, max_step);
	g = Get_Fading_Channel(origin_color.G, AsConfig::BG_Color.G, step, max_step);
	b = Get_Fading_Channel(origin_color.B, AsConfig::BG_Color.B, step, max_step);

	result_color = AColor(r, g, b);
}
//------------------------------------------------------------------------------------------------------------
bool AsTools::Reflect_On_Circle(double next_x_pos, double next_y_pos, double circle_x, double circle_y, double circle_radius, ABall_Object *ball)
{
	double dx, dy;
	double distance, two_radiuses;
	double alpha, beta, gamma;
	double related_ball_direction;
	const double pi_2 = 2.0 * M_PI;


	dx = next_x_pos - circle_x;
	dy = next_y_pos - circle_y;

	distance = sqrt(dx * dx + dy * dy);
	two_radiuses = circle_radius + ball->Radius;

	if (distance + AsConfig::Moving_Step_Size < two_radiuses)
	{// Мячик коснулся бокового шарика

		beta = atan2(-dy, dx);

		related_ball_direction = ball->Get_Direction();
		related_ball_direction -= beta;

		if (related_ball_direction > pi_2)
			related_ball_direction -= pi_2;

		if (related_ball_direction < 0.0)
			related_ball_direction += pi_2;

		if (related_ball_direction > M_PI_2 && related_ball_direction < M_PI + M_PI_2)
		{
			alpha = beta + M_PI - ball->Get_Direction();
			gamma = beta + alpha;

			ball->Set_Direction(gamma);

			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------