#include "Platform_Laser.h"

AsPlatform_Laser::AsPlatform_Laser(AsPlatform_State &platform_state)
	: Platform_State(&platform_state), Laser_Beam_Set(0), Enable_Laser_Firing(false), Laser_Transformation_Step(0),
	Last_Laser_Shot_Tick(0)
{}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Init(AsLaser_Beam_Set *laser_beam_set)
{
	Laser_Beam_Set = laser_beam_set;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform_Laser::Act(EPlatform_State &next_state, double x_pos)
{
	next_state = EPlatform_State::Unknown;
	double left_gun_x_pos, right_gun_x_pos;

	switch (Platform_State->Laser)
	{
	case EPlatform_Transformation::Init:
		if (Laser_Transformation_Step < Max_Laser_Transformation_Step)
			++Laser_Transformation_Step;
		else
			Platform_State->Laser = EPlatform_Transformation::Active;

		return true; // нужно перерисовать платформу


	case EPlatform_Transformation::Active:
		if (Enable_Laser_Firing)
		{
			if (Last_Laser_Shot_Tick + Laser_Shot_Timeout <= AsConfig::Current_Timer_Tick)
			{
				Last_Laser_Shot_Tick = AsConfig::Current_Timer_Tick + Laser_Shot_Timeout;

				left_gun_x_pos = Get_Gun_Pos(x_pos, true) + 0.5;
				right_gun_x_pos = Get_Gun_Pos(x_pos, false) + 0.5;

				Laser_Beam_Set->Fire(left_gun_x_pos, right_gun_x_pos);
			}
		}
		break;


	case EPlatform_Transformation::Finalize:
		if (Laser_Transformation_Step > 0)
			--Laser_Transformation_Step;
		else
		{
			Platform_State->Laser = EPlatform_Transformation::Unknown;
			next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
			Enable_Laser_Firing = false;
		}

		return true; // нужно перерисовать платформу

	default:
		AsConfig::Throw();
	}

	return false; // перерисовка платформы не требуется
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_State(HDC hdc, double x_pos, RECT &platform_rect)
{
	HRGN region; //--- указывает область обрезки изображения, которые выводится на экран

	region = CreateRectRgnIndirect(&platform_rect);
	SelectClipRgn(hdc, region); //--- установка области обрезки

	// 1. Левое крыло
	Draw_Wing(hdc, x_pos, true);

	// 2. Правое крыло
	Draw_Wing(hdc, x_pos, false);

	// 3. Центральная часть
	// 3.0 Нормальная средняя часть
	Draw_Inner_part(hdc, x_pos);

	// 3.1. Левая нога
	Draw_Leg(hdc, x_pos, true);

	// 3.2. Правая нога
	Draw_Leg(hdc, x_pos, false);

	// 3.3. Кабина
	Draw_Cabin(hdc, x_pos);

	SelectClipRgn(hdc, 0); //--- удаление области обрезки
	DeleteObject(region); //--- удалить объект нужно обязательно!!
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Reset()
{
	Laser_Transformation_Step = 0;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Fire(bool fire_on)
{
	if (Platform_State->Laser != EPlatform_Transformation::Active)
		return; // Игнорируем выстрел, пока платформа не сформируется

	Enable_Laser_Firing = fire_on;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Wing(HDC hdc, double x_pos, bool is_left)
{
	double x, y;
	int x_offset;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;
	double height;
	int half_max_step = Max_Laser_Transformation_Step / 2;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;


	AsConfig::Platform_Circle_Color.Select(hdc); //--- установка цвета полуэллипса

	y = AsConfig::Platform_Y_Pos;
	x = x_pos;

	if (!is_left)
		x += AsConfig::Platform_Normal_Width - AsConfig::Platform_Circle_Size;

	// 1. Крыло //--- в форме полуэллипса
	Draw_Expanding_Figure(hdc, EFigure_type::Ellipse, x, y, 7, 7, ratio, x, y + 1, 7, 12);


	// 1.1. Перемычка
	if (is_left)
		x_offset = 5;
	else
		x_offset = -4;

	Draw_Expanding_Figure(hdc, EFigure_type::Rectangle, x + 3, y + 6, 1, 1, ratio, x + x_offset, y + 2, 6, 5);


	// 1.2. Пушка
	AsConfig::Gun_Color.Select(hdc);

	if (Laser_Transformation_Step >= half_max_step)
	{
		ratio = (double)(Laser_Transformation_Step - half_max_step) / (double)half_max_step;

		x = Get_Gun_Pos(x_pos, is_left);

		height = 3.0 * (1.0 - ratio) * d_scale;

		MoveToEx(hdc, (int)(x * d_scale + 1.0), (int)(y * d_scale + 3.0 * d_scale + 1.0), 0);
		LineTo(hdc, (int)(x * d_scale + 1.0), (int)(y * d_scale + height + 1.0)); 

		// 1.3. Хвост
		Draw_Expanding_Figure(hdc, EFigure_type::Ellipse, x + 1, y + 5, 0, 0, ratio, x - 1, y + 5 + 1.0 / d_scale, 3, 6);
	}
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform_Laser::Get_Gun_Pos(double platform_x_pos, bool is_left)
{
	double gun_x_pos;

	if (is_left)
		gun_x_pos = platform_x_pos + 3.0;
	else
		gun_x_pos = platform_x_pos + (AsConfig::Platform_Normal_Width - 4);

	return gun_x_pos;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Inner_part(HDC hdc, double x_pos)
{// рисует уменьшающуюся часть обычной платформы
 // Размер 20 х 5 => 8 x 1

	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;
	int x = (int)x_pos;
	int y = AsConfig::Platform_Y_Pos;

	AsConfig::Platform_Inner_Color.Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_type::Round_Rect_3x, x + 4, y + 1, 20, 5, ratio, x + 10, y + 1, 8, 1);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Leg(HDC hdc, double x_pos, bool is_left)
{
	double x, y;
	double x_scale;
	int scale = AsConfig::Global_Scale;
	double d_scale = AsConfig::D_Global_Scale;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

	AsConfig::Platform_Inner_Color.Select(hdc);

	if (is_left)
	{
		x = (x_pos + 6.0) * d_scale;
		x_scale = scale;
	}
	else
	{
		x = x_pos * d_scale + (AsConfig::Platform_Normal_Width - 6) * d_scale - 1.0;
		x_scale = -scale;
	}

	y = (AsConfig::Platform_Y_Pos + 3) * scale;

	POINT leg_points[7] = {
		{(int)x, (int)y},
		{(int)(x + 2.0 * x_scale), (int)(y - 2.0 * scale)},
		{(int)(x + 4.0 * x_scale), (int)(y - 2.0 * scale)},
		{(int)(x + 4.0 * x_scale), (int)y},
		{(int)(x + 2.0 * x_scale), (int)(y + 2.0 * scale)},
		{(int)(x + 2.0 * x_scale), (int)(y + (2.0 + 2.0 * ratio) * d_scale)},
		{(int)x, (int)(y + 4.0 * ratio * d_scale)}
	};

	Polygon(hdc, leg_points, 7); //--- многоугольник
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Cabin(HDC hdc, double x_pos)
{
	double x = x_pos;
	double y = AsConfig::Platform_Y_Pos;
	int scale = AsConfig::Global_Scale;
	double one_pixel = 1.0 / AsConfig::D_Global_Scale;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

	// 3.3.1. Внешняя часть
	AsConfig::Platform_Inner_Color.Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_type::Ellipse, x + 13, y + 1, 2, 1, ratio, x + 9, y - 1, 10, 8 - one_pixel);

	// 3.3.2. Среднее кольцо
	AsConfig::BG_Color.Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_type::Rectangle, x + 13, y + 1, 2, 1, ratio, x + 11, y, 6, 1);
	Draw_Expanding_Figure(hdc, EFigure_type::Ellipse, x + 13, y + 1, 2, 1, ratio, x + 10, y, 8, 5 - one_pixel);

	// 3.3.3 Внутренняя часть
	AsConfig::White_Color.Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_type::Ellipse, x + 13, y + 1, 2, 1, ratio, x + 11, y, 6, 4 - one_pixel);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Expanding_Figure(HDC hdc, EFigure_type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height)
{
	const double d_scale = AsConfig::D_Global_Scale;
	int x, y;
	int width, height;
	RECT rect{};

	x = Get_Expanding_Value(start_x, end_x, ratio);
	y = Get_Expanding_Value(start_y, end_y, ratio);
	width = Get_Expanding_Value(start_width, end_width, ratio);
	height = Get_Expanding_Value(start_height, end_height, ratio);

	switch (figure_type)
	{
	case EFigure_type::Ellipse:
		Ellipse(hdc, x, y, x + width - 1, y + height);
		break;

	case EFigure_type::Rectangle:
		Rectangle(hdc, x, y, x + width - 1, y + height);
		break;

	case EFigure_type::Round_Rect_3x:
		rect.left = x;
		rect.top = y;
		rect.right = rect.left + width;
		rect.bottom = rect.top + height + 1;

		AsTools::Round_Rect(hdc, rect, 3);
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
int AsPlatform_Laser::Get_Expanding_Value(double start, double end, double ratio)
{
	double delta;
	int value;

	delta = end - start;
	value = (int)( (start + delta * ratio) * AsConfig::Global_Scale);

	return value;
}