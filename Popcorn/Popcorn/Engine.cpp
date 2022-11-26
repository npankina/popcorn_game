#include "Engine.h"

char Level_01[ALevel::Level_Height][ALevel::Level_Width] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


// ABall
//------------------------------------------------------------------------------------------------------------
ABall::ABall()
: Ball_Pen(0), Ball_Brush(0), Ball_X_Pos(20), Ball_Y_Pos(170), Ball_Speed(3.0), Ball_Direction(M_PI - M_PI_4),
  Ball_Rect{}, Prev_Ball_Rect{}
{}
//------------------------------------------------------------------------------------------------------------
void ABall::Init()
{
	AsConfig::Create_Pen_Brush(255, 255, 255, Ball_Pen, Ball_Brush);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw(HDC hdc, RECT &paint_area, AsEngine *engine)
{
	RECT intersection_rect;
	if ( !(IntersectRect(&intersection_rect, &paint_area, &Ball_Rect)))
		return;


	// 1. Очищает фон
	SelectObject(hdc, engine->BG_Pen);
	SelectObject(hdc, engine->BG_Brush);

	Ellipse(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right, Prev_Ball_Rect.bottom);

	// 2. Рисует шарик
	SelectObject(hdc, Ball_Pen);
	SelectObject(hdc, Ball_Brush);

	Ellipse(hdc, Ball_Rect.left, Ball_Rect.top, Ball_Rect.right, Ball_Rect.bottom);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Move(AsEngine *engine, ALevel *level, APlatform *platform)
{
	int next_x_pos, next_y_pos;
	int max_x_pos = AsEngine::Max_X_Pos - Ball_Size;
	int platform_y_pos = APlatform::Y_Pos - Ball_Size;

	Prev_Ball_Rect = Ball_Rect;

	next_x_pos = Ball_X_Pos + (int)(Ball_Speed * cos(Ball_Direction)),
	next_y_pos = Ball_Y_Pos - (int)(Ball_Speed * sin(Ball_Direction));

	// Корректируем позицию при отражении от рамки
	if (next_x_pos < AsEngine::Border_X_Offset)
	{
		next_x_pos = ALevel::Level_X_Offset - (next_x_pos - ALevel::Level_X_Offset);
		Ball_Direction = M_PI - Ball_Direction;
	}

	if (next_y_pos < AsEngine::Border_Y_Offset)
	{
		next_y_pos = AsEngine::Border_Y_Offset - (next_y_pos - AsEngine::Border_Y_Offset);
		Ball_Direction = -Ball_Direction;
	}

	if (next_x_pos > max_x_pos)
	{
		next_x_pos = max_x_pos - (next_x_pos - max_x_pos);
		Ball_Direction = M_PI - Ball_Direction;
	}

	if (next_y_pos > AsEngine::Max_Y_Pos)
	{
		next_y_pos = AsEngine::Max_Y_Pos - (next_y_pos - AsEngine::Max_Y_Pos);
		Ball_Direction = M_PI + (M_PI - Ball_Direction);
	}

	// Корректируем позицию при отражении от платформы
	if (next_y_pos >= platform_y_pos)
	{
		if (next_x_pos >= platform->X_Pos and next_x_pos <= platform->X_Pos + platform->Width)
		{
			next_y_pos = platform_y_pos - (next_y_pos - platform_y_pos);
			Ball_Direction = M_PI + (M_PI - Ball_Direction);
		}
	}

	// Корректируем позицию при отражении от кирпичей
	level->Check_Level_Brick_Hit(next_y_pos, Ball_Direction);

	// Смещаем шарик
	Ball_X_Pos = next_x_pos;
	Ball_Y_Pos = next_y_pos;

	Ball_Rect.left = Ball_X_Pos * AsConfig::Global_Scale;
	Ball_Rect.top = Ball_Y_Pos * AsConfig::Global_Scale;
	Ball_Rect.right = Ball_Rect.left + Ball_Size * AsConfig::Global_Scale - 1;
	Ball_Rect.bottom = Ball_Rect.top + Ball_Size * AsConfig::Global_Scale - 1;

	InvalidateRect(engine->Hwnd, &Ball_Rect, FALSE);
	InvalidateRect(engine->Hwnd, &Prev_Ball_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------



// ALevel
//------------------------------------------------------------------------------------------------------------
ALevel::ALevel()
: Brick_Red_Pen(0), Brick_Blue_Pen(0), Brick_Red_Brush(0), Brick_Blue_Brush(0), Level_Rect{}, Letter_Pen{}
{}
//------------------------------------------------------------------------------------------------------------
void ALevel::Init()
{
	Letter_Pen = CreatePen(PS_SOLID, AsConfig::Global_Scale, RGB(255, 255, 255));

	AsConfig::Create_Pen_Brush(230, 25, 229, Brick_Red_Pen, Brick_Red_Brush); // 255, 129, 249
	AsConfig::Create_Pen_Brush(0, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush); // 85, 255, 255

	Level_Rect.left = Level_X_Offset * AsConfig::Global_Scale;
	Level_Rect.top = Level_Y_Offset * AsConfig::Global_Scale;
	Level_Rect.right = (Level_Rect.left + Cell_Width * Level_Width) * AsConfig::Global_Scale;
	Level_Rect.bottom = (Level_Rect.top + Cell_Height * Level_Height) * AsConfig::Global_Scale;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Draw(HDC hdc, RECT &paint_area)
{// Вывод всех кирпичей уровня

	int i, j;
	RECT intersection_rect;

	if ( !IntersectRect(&intersection_rect, &paint_area, &Level_Rect))
		return;

	for (i = 0; i < Level_Height; i++)
		for (j = 0; j < Level_Width; j++)
			Draw_Brick(hdc, Level_X_Offset + j * Cell_Width, Level_Y_Offset + i * Cell_Height, (EBrick_Type)Level_01[i][j]);
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Check_Level_Brick_Hit(int &next_y_pos, double &ball_direction)
{// Корректируем позицию при отражении от платформы
	int brick_y_pos = Level_Y_Offset + Level_Height * Cell_Height;

	for (int i = Level_Height - 1; i >= 0; i--)
	{
		for (int j = 0; j < Level_Width; j++)
		{
			if (Level_01[i][j] == 0)
				continue;

			if (next_y_pos < brick_y_pos)
			{
				next_y_pos = brick_y_pos - (next_y_pos - brick_y_pos);
				ball_direction = -ball_direction;
			}
		}
		brick_y_pos -= Cell_Height;
	}
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Set_Brick_Letter_Color(bool is_switch_color, HPEN &front_pen, HBRUSH &front_brush, HPEN &back_pen, HBRUSH &back_brush)
{
	if (is_switch_color) 
	{
		front_pen = Brick_Red_Pen;
		front_brush = Brick_Red_Brush;

		back_pen = Brick_Blue_Pen;
		back_brush = Brick_Blue_Brush;
	}
	else 
	{
		front_pen = Brick_Blue_Pen;
		front_brush = Brick_Blue_Brush;

		back_pen = Brick_Red_Pen;
		back_brush = Brick_Red_Brush;
	}
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Draw_Brick_Letter(HDC hdc, int x, int y,  EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step)
{// Отрисовка буквы кирпича

	bool switch_color;
	double offset;
	double rotation_angle; // преобразование шага в угол поворота
	int brick_half_height = Brick_Height * AsConfig::Global_Scale / 2;
	int back_part_offset;
	HPEN front_pen, back_pen;
	HBRUSH front_brush, back_brush;
	XFORM xform, old_xform;

	if (!(brick_type == EBT_Blue or brick_type == EBT_Red))
		return; // падающие буквы могут быть только от кирпичей такого типа

	// Корректировка шаг вращения и угол поворота
	rotation_step = rotation_step % 16;

	if (rotation_step < 8)
		rotation_angle = 2.0 * M_PI / 16.0 * (double)rotation_step;
	else
		rotation_angle = 2.0 * M_PI / 16.0 * (double)(8 - rotation_step);

	if (rotation_step > 4 and rotation_step <= 12) 
	{
		if (brick_type == EBT_Blue)
			switch_color = true;
		else
			switch_color = false;
	}
	else 
	{
		if (brick_type == EBT_Red)
			switch_color = true;
		else
			switch_color = false;
	}
	Set_Brick_Letter_Color(switch_color, front_pen, front_brush, back_pen, back_brush);

	if (rotation_step == 4 or rotation_step == 12) 
	{
		// вывод фона
		SelectObject(hdc, back_pen);
		SelectObject(hdc, back_brush);

		Rectangle(hdc, x, y + brick_half_height - AsConfig::Global_Scale, x + Brick_Width * AsConfig::Global_Scale, y + brick_half_height);

		// вывод переднего плана
		SelectObject(hdc, front_pen);
		SelectObject(hdc, front_brush);

		Rectangle(hdc, x, y + brick_half_height, x + Brick_Width * AsConfig::Global_Scale, y + brick_half_height + AsConfig::Global_Scale - 1);
	}
	else 
	{
		SetGraphicsMode(hdc, GM_ADVANCED);

		// настройка матрицы "переворота" буквы
		xform.eM11 = 1.0f;
		xform.eM12 = 0.0f;
		xform.eM21 = 0.0f;
		xform.eM22 = (float)cos(rotation_angle);
		xform.eDx = (float)x;
		xform.eDy = (float)y + (float)brick_half_height;
		GetWorldTransform(hdc, &old_xform);
		SetWorldTransform(hdc, &xform);

		// вывод фона
		SelectObject(hdc, back_pen);
		SelectObject(hdc, back_brush);

		offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)AsConfig::Global_Scale;
		back_part_offset = (int)round(offset);
		Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * AsConfig::Global_Scale, brick_half_height - back_part_offset);

		// вывод переднего плана
		SelectObject(hdc, front_pen);
		SelectObject(hdc, front_brush);
		Rectangle(hdc, 0, -brick_half_height, Brick_Width * AsConfig::Global_Scale, brick_half_height);

		if (rotation_step > 4 and rotation_step <= 12) 
		{
			if (letter_type == ELT_O) 
			{
				SelectObject(hdc, Letter_Pen);
				Ellipse(hdc, 0 + 5 * AsConfig::Global_Scale, -5 * AsConfig::Global_Scale / 2, 0 + 10 * AsConfig::Global_Scale, 5 * AsConfig::Global_Scale / 2);
			}
		}

		SetWorldTransform(hdc, &old_xform);
	}
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type)
{// Вывод "кирпича"

	HPEN pen;
	HBRUSH brush;

	switch (brick_type)
	{
	case EBT_None:
		return;
	case EBT_Red:
		pen = Brick_Red_Pen; 
		brush = Brick_Red_Brush;
		break;
	case EBT_Blue:
		pen = Brick_Blue_Pen;
		brush = Brick_Blue_Brush;
		break;
	default:
		return;
	}

	SelectObject(hdc, pen);
	SelectObject(hdc, brush);

	RoundRect(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + Brick_Width) * AsConfig::Global_Scale, (y + Brick_Height) * AsConfig::Global_Scale, 2 * AsConfig::Global_Scale, 2 * AsConfig::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------



// APlatform
//------------------------------------------------------------------------------------------------------------
APlatform::APlatform()
: Inner_Width(21), X_Pos(AsEngine::Border_X_Offset), X_Step(AsConfig::Global_Scale * 2), Width(28), 
  Highlight_Pen(0), Platform_Rect{}, Prev_Platform_Rect{}
{}
//------------------------------------------------------------------------------------------------------------
void APlatform::Init()
{
	Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
}
//------------------------------------------------------------------------------------------------------------
void APlatform::Redraw(AsEngine *engine)
{
	Prev_Platform_Rect = Platform_Rect;

	Platform_Rect.left = X_Pos * AsConfig::Global_Scale;
	Platform_Rect.top = Y_Pos * AsConfig::Global_Scale;
	Platform_Rect.right = Platform_Rect.left + Width * AsConfig::Global_Scale;
	Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;

	InvalidateRect(engine->Hwnd, &Prev_Platform_Rect, FALSE);
	InvalidateRect(engine->Hwnd, &Platform_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void APlatform::Draw(HDC hdc, RECT &paint_area, AsEngine *engine, ALevel *level)
{// Рисует платформу

	RECT intersection_rect;
	int x = X_Pos;
	int y = Y_Pos;

	if ( !IntersectRect(&intersection_rect, &paint_area, &Platform_Rect)) // если прямоугольники пересекаются рисуем платформу в новых координатах
		return;

	SelectObject(hdc, engine->BG_Pen);
	SelectObject(hdc, engine->BG_Brush);
	Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);

	// 1. Рисует шарики
	SelectObject(hdc, level->Brick_Red_Pen); // назначить карандаш текущим (для отрисовки)
	SelectObject(hdc, level->Brick_Red_Brush);  // назначить кисть текущей (для отрисовки)

	Ellipse(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + Circle_Size) * AsConfig::Global_Scale, (y + Circle_Size) * AsConfig::Global_Scale);
	Ellipse(hdc, (x + Inner_Width) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + Inner_Width + Circle_Size) * AsConfig::Global_Scale, (y + Circle_Size) * AsConfig::Global_Scale);

	// 2. Рисует блик
	SelectObject(hdc, Highlight_Pen);
	Arc(hdc, (x + 1) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + Circle_Size - 1) * AsConfig::Global_Scale, (y + Circle_Size - 1) * AsConfig::Global_Scale,
		(x + 1 + 1) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 1) * AsConfig::Global_Scale, (y + 1 + 2) * AsConfig::Global_Scale);

	// 3. Рисует среднюю часть
	SelectObject(hdc, level->Brick_Blue_Pen);
	SelectObject(hdc, level->Brick_Blue_Brush);

	RoundRect(hdc, (x + 4) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4 + Inner_Width - 1) * AsConfig::Global_Scale, (y + 1 + 5) * AsConfig::Global_Scale, 3 * AsConfig::Global_Scale, 3 * AsConfig::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------



// AsEngine
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
	: Hwnd(0), BG_Pen(0), BG_Brush(0)
{}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Init_Engine(HWND hWnd)
{// Настройка игры при старте

	Hwnd = hWnd;

	AsConfig::Create_Pen_Brush(6, 30, 82, BG_Pen, BG_Brush);

	Level.Init();
	Ball.Init();
	Platform.Init();
	Platform.Redraw(this);
	Border.Init();

	SetTimer(Hwnd, Timer_ID, 50, 0);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT &paint_area)
{// Отрисовка экрана игры

	Level.Draw(hdc, paint_area);

	Platform.Draw(hdc, paint_area, this, &Level);

	/*for (int i = 0; i < 16; i++) {
		Draw_Brick_Letter(hdc, (20 + i * Cell_Width) * Global_Scale, 100, EBT_Blue, ELT_O, i);
		Draw_Brick_Letter(hdc, (20 + i * Cell_Width) * Global_Scale, 130, EBT_Red, ELT_O, i);
	}*/

	
	Ball.Draw(hdc, paint_area, this);

	Border.Draw(hdc, paint_area, BG_Pen, BG_Brush);
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Key_Down(EKey_Type key_type)
{
	switch (key_type)
	{
	case EKT_Left:
		Platform.X_Pos -= Platform.X_Step;

		if (Platform.X_Pos <= Border_X_Offset)
			Platform.X_Pos = Border_X_Offset;

		Platform.Redraw(this);
		break;

	case EKT_Right:
		Platform.X_Pos += Platform.X_Step;

		if (Platform.X_Pos >= Max_X_Pos - Platform.Width + 1)
			Platform.X_Pos = Max_X_Pos - Platform.Width + 1;


		Platform.Redraw(this);
		break;

	case EKT_Space:
		break;
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Timer()
{
	Ball.Move(this, &Level, &Platform);
	return 0;
}
//------------------------------------------------------------------------------------------------------------
