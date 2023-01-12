﻿#include "Active_Brick.h"

// AGraphics_Object
//------------------------------------------------------------------------------------------------------------
AGraphics_Object::~AGraphics_Object()
{}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick
//------------------------------------------------------------------------------------------------------------
void AActive_Brick::Get_Level_Pos(int &dest_brick_x, int &dest_brick_y)
{
	dest_brick_x = Level_X;
	dest_brick_y = Level_Y;
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick::~AActive_Brick()
{}
//------------------------------------------------------------------------------------------------------------
AActive_Brick::AActive_Brick(EBrick_Type brick_type, int level_x, int level_y)
: Brick_Type(brick_type), Level_X(level_x), Level_Y(level_y), Brick_Rect{}
{
	Brick_Rect.left = (AsConfig::Level_X_Offset + level_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	Brick_Rect.top = (AsConfig::Level_Y_Offset + level_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	Brick_Rect.right = Brick_Rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	Brick_Rect.bottom = Brick_Rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick::Clear(HDC hdc, RECT& paint_area)
{}
//------------------------------------------------------------------------------------------------------------
double AActive_Brick::Get_Brick_X_Pos(bool of_center)
{
	double pos;

	pos = (double)(AsConfig::Level_X_Offset + Level_X * AsConfig::Cell_Width);

	if (of_center)
		pos += (double)AsConfig::Brick_Width / 2.0;

	return pos;
}
//------------------------------------------------------------------------------------------------------------
double AActive_Brick::Get_Brick_Y_Pos(bool of_center)
{
	double pos;

	pos = (double)(AsConfig::Level_Y_Offset + Level_Y * AsConfig::Cell_Height);

	if (of_center)
		pos += (double)AsConfig::Brick_Height / 2.0;

	return pos;
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Red_Blue
//------------------------------------------------------------------------------------------------------------
AColor AActive_Brick_Red_Blue::Fading_Blue_Brick_Colors[Max_Fade_Step];
AColor AActive_Brick_Red_Blue::Fading_Red_Brick_Colors[Max_Fade_Step];
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Red_Blue::~AActive_Brick_Red_Blue()
{}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Red_Blue::AActive_Brick_Red_Blue(EBrick_Type brick_type, int level_x, int level_y)
: AActive_Brick(brick_type, level_x, level_y), Fade_Step(0)
{
	if ( !(brick_type == EBT_Blue or brick_type == EBT_Red) )
		AsConfig::Throw();
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Act()
{
	if (Fade_Step < Max_Fade_Step - 1)
	{
		++Fade_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Draw(HDC hdc, RECT &paint_area)
{
	AColor *color = 0;

	switch (Brick_Type)
	{
	case EBT_Red:
		color = &Fading_Red_Brick_Colors[Fade_Step];
		break;

	case EBT_Blue:
		color = &Fading_Blue_Brick_Colors[Fade_Step];
		break;
	}

	if (color != 0)
		color->Select(hdc);

	AsConfig::Round_Rect(hdc, Brick_Rect);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Red_Blue::Is_Finished()
{
	if (Fade_Step >= Max_Fade_Step)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Setup_Colors()
{
	int i;

	for (i = 0; i < Max_Fade_Step; i++)
	{
		Get_Fading_Color(AsConfig::Red_Color, i, Fading_Red_Brick_Colors[i]);
		Get_Fading_Color(AsConfig::Blue_Color, i, Fading_Blue_Brick_Colors[i]);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{// Вывод неактивного кирпича на уровне
	const AColor *color = 0;

	switch (brick_type)
	{
	case EBT_None:
		color = &AsConfig::BG_Color;
		break;

	case EBT_Red:
		color = &AsConfig::Red_Color;
		break;

	case EBT_Blue:
		color = &AsConfig::Blue_Color;
		break;

	default:
		AsConfig::Throw();
	}

	if (color != 0)
		color->Select(hdc);

	AsConfig::Round_Rect(hdc, brick_rect);
}
//------------------------------------------------------------------------------------------------------------
unsigned char AActive_Brick_Red_Blue::Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step)
{
	return color - step * (color - bg_color) / (Max_Fade_Step - 1);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Get_Fading_Color(const AColor &origin_color, int step, AColor &result_color)
{
	unsigned char r, g, b;

	r = Get_Fading_Channel(origin_color.R, AsConfig::BG_Color.R, step);
	g = Get_Fading_Channel(origin_color.G, AsConfig::BG_Color.G, step);
	b = Get_Fading_Channel(origin_color.B, AsConfig::BG_Color.B, step);

	result_color = AColor(r, g, b);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Unbreakable
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Unbreakable::~AActive_Brick_Unbreakable()
{
	DeleteObject(Region);
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Unbreakable::AActive_Brick_Unbreakable(int level_x, int level_y)
: AActive_Brick(EBT_Unbreakable, level_x, level_y), Animation_Step(0), Region(0)
{
	Region = CreateRoundRectRgn(Brick_Rect.left, Brick_Rect.top, Brick_Rect.right + 1, Brick_Rect.bottom + 1, 2 * AsConfig::Global_Scale - 1, 2 * AsConfig::Global_Scale - 1);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Act()
{
	if (Animation_Step <= Max_Animation_Step)
	{
		++Animation_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Draw(HDC hdc, RECT &paint_area)
{
	int offset;
	const int scale = AsConfig::Global_Scale;

	Draw_In_Level(hdc, Brick_Rect);

	AsConfig::Red_Color.Select(hdc);

	SelectClipRgn(hdc, Region);

	offset = 2 * Animation_Step * scale - AsConfig::Brick_Width * scale;

	AsConfig::Unbreakable_Blue_Highlight.Select_Pen(hdc);
	MoveToEx(hdc, Brick_Rect.left + 4 * scale + offset, Brick_Rect.bottom + scale, 0);
	LineTo(hdc, Brick_Rect.left + 13 * scale + offset - 1, Brick_Rect.top - 1 * scale);

	AsConfig::Unbreakable_Red_Highlight.Select_Pen(hdc);
	MoveToEx(hdc, Brick_Rect.left + 6 * scale + offset, Brick_Rect.bottom + scale, 0);
	LineTo(hdc, Brick_Rect.left + 15 * scale + offset - 1, Brick_Rect.top - 1 * scale);

	SelectClipRgn(hdc, 0);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Unbreakable::Is_Finished()
{
	if (Animation_Step >= Max_Animation_Step)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Draw_In_Level(HDC hdc, RECT &brick_rect)
{ // Вывод неактивного кирпича на уровне
	AsConfig::White_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, brick_rect);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Multihit
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Multihit::~AActive_Brick_Multihit()
{}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Multihit::AActive_Brick_Multihit(int level_x, int level_y)
: AActive_Brick(EBT_Multihit_1, level_x, level_y), Rotation_Step(0)
{}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Act()
{
	if (Rotation_Step <= Max_Rotation_Step)
	{
		++Rotation_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw(HDC hdc, RECT &paint_area)
{
	int step;
	const int scale = AsConfig::Global_Scale;
	double rotation_angle, x_ratio;
	RECT zero_rect{};
	XFORM xform{}, old_xform{};

	// 1. Очистка фона
	AsConfig::BG_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, Brick_Rect);


	// 2. Настраиваем матрицу "переворота" сотни
	step = Rotation_Step % Steps_Per_Turn;
	rotation_angle = M_PI_4 / 2.0 * (double)step;
	x_ratio = cos(rotation_angle);

	xform.eM11 = (float)x_ratio;
	xform.eM12 = 0.0f;
	xform.eM21 = 0.0f;
	xform.eM22 = 1.0f;
	xform.eDx = (float)Brick_Rect.left + (float)(1.0 - x_ratio) * (float)(AsConfig::Brick_Width * AsConfig::Global_Scale) / 2.0f;
	xform.eDy = (float)Brick_Rect.top;
	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);


	// 3. Рисуем "сотню"
	AsConfig::Letter_Color.Select_Pen(hdc);
	MoveToEx(hdc, 0 + 1 * scale + 1, 0 + 3 * scale, 0);
	LineTo(hdc, 0 + 3 * scale + 1, 0 + 1 * scale);
	LineTo(hdc, 0 + 3 * scale + 1, 0 + 6 * scale - 1);

	zero_rect.left = 0 + 5 * scale + 1;
	zero_rect.top = 0 + 1 * scale;
	zero_rect.right = zero_rect.left + 3 * scale + 1;
	zero_rect.bottom = zero_rect.top + 5 * scale;
	AsConfig::Round_Rect(hdc, zero_rect);

	zero_rect.left += 5 * scale;
	zero_rect.right += 5 * scale;
	AsConfig::Round_Rect(hdc, zero_rect);

	SetWorldTransform(hdc, &old_xform);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Multihit::Is_Finished()
{
	if (Rotation_Step >= Max_Rotation_Step)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{ // Вывод неактивного кирпича на уровне

	const int scale = AsConfig::Global_Scale;

	// 1. Рисование фона
	AsConfig::White_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, brick_rect);

	AsConfig::Red_Color.Select(hdc);
	Rectangle(hdc, brick_rect.left + scale, brick_rect.top + scale, brick_rect.right - scale - 1, brick_rect.bottom - scale - 1);


	// 2. Рисование внутренних прямоугольников
	switch(brick_type)
	{
	case EBT_Multihit_1:
		Draw_Stage(hdc, brick_rect, 2, 10);
		break;

	case EBT_Multihit_2:
		Draw_Stage(hdc, brick_rect, 2, 4);
		Draw_Stage(hdc, brick_rect, 8, 4);
		break;

	case EBT_Multihit_3:
		Draw_Stage(hdc, brick_rect, 2, 2);
		Draw_Stage(hdc, brick_rect, 6, 2);
		Draw_Stage(hdc, brick_rect, 10, 2);
		break;

	case EBT_Multihit_4:
		Draw_Stage(hdc, brick_rect, 2, 2);
		Draw_Stage(hdc, brick_rect, 5, 2);
		Draw_Stage(hdc, brick_rect, 8, 2);
		Draw_Stage(hdc, brick_rect, 11, 2);
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw_Stage(HDC hdc, RECT &brick_rect, int x, int width)
{
	const int scale = AsConfig::Global_Scale;
	RECT stage_rect{};

	stage_rect.left = brick_rect.left + x * scale;
	stage_rect.top = brick_rect.top + 2 * scale;
	stage_rect.right = stage_rect.left + width * scale;
	stage_rect.bottom = stage_rect.top + 3 * scale;

	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, stage_rect.left + scale, stage_rect.top + scale, stage_rect.right + scale - 1, stage_rect.bottom + scale - 1);

	AsConfig::Blue_Color.Select(hdc);
	Rectangle(hdc, stage_rect.left, stage_rect.top, stage_rect.right - 1, stage_rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Teleport
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport::~AActive_Brick_Teleport()
{}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport::AActive_Brick_Teleport(int level_x, int level_y, ABall *ball, AActive_Brick_Teleport *destination_teleport)
: AActive_Brick(EBT_Teleport, level_x, level_y), Teleport_State(ETS_Started), Animation_Step(0),Ball(0),
  Destination_Teleport(destination_teleport)
{
	Set_Ball(ball);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Act()
{
	/*if (AsConfig::Current_Timer_Tick % 10 != 0)
		return;*/

	double ball_x = 0.0;
	double ball_y = 0.0;
	double direction;


	if (Animation_Step <= Max_Animation_Step)
	{
		++Animation_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
	else
	{
		switch(Teleport_State)
		{
		case ETS_Started:
			Animation_Step = 0;
			Teleport_State = ETS_Finished;

			if (Destination_Teleport != 0)
			{
				Destination_Teleport->Set_Ball(Ball);
				Ball = 0;
			}
			break;

		case ETS_Finished:
			Teleport_State = ETS_Done;

			if (Ball != 0)
			{
				switch(Reliase_Direction)
				{
				case EDT_Left:
					ball_x = Get_Brick_X_Pos(false) - ABall::Radius;
					ball_y = Get_Brick_Y_Pos(true);
					break;

				case EDT_Up:
					ball_x = Get_Brick_X_Pos(true);
					ball_y = Get_Brick_Y_Pos(false) - ABall::Radius;
					break;

				case EDT_Right:
					ball_x = Get_Brick_X_Pos(false) + (double)AsConfig::Brick_Width + ABall::Radius;
					ball_y = Get_Brick_Y_Pos(true);
					break;

				case EDT_Down:
					ball_x = Get_Brick_X_Pos(true);
					ball_y = Get_Brick_Y_Pos(false) + (double)AsConfig::Brick_Height + ABall::Radius;
					break;
				
				default:
					AsConfig::Throw();
				}

				direction = Ball->Get_Direction();
				Ball->Set_State(EBS_Normal, ball_x, ball_y);
				Ball->Set_Direction(direction);

				Ball = 0; // Ставим мячику статус - отсутствует в телепорте
				InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
			}
			break;
	
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Draw(HDC hdc, RECT &paint_area)
{
	int step;

	switch(Teleport_State)
	{
	case ETS_Started:
		step = Animation_Step;
		break;

	case ETS_Finished:
		step = Max_Animation_Step - Animation_Step;
		break;

	default:
		step = 0;
	}

	Draw_In_Level(hdc, Brick_Rect, step);

	if (Ball != 0)
		Ball->Draw_Teleporting(hdc, step);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Teleport::Is_Finished()
{
	if (Teleport_State == ETS_Done)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Draw_In_Level(HDC hdc, RECT &brick_rect, int step)
{ // Вывод неактивного кирпича на уровне

	const int scale = AsConfig::Global_Scale;
	int top_x = brick_rect.left + 3 * scale + 1;
	int top_y = brick_rect.top + step / 2 + 1;
	int low_x = brick_rect.left + 11 * scale + 1;
	int low_y = brick_rect.top + 6 * scale - step / 2 + 1;

	AsConfig::Red_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, brick_rect);

	AsConfig::Teleport_Portal_Color.Select(hdc);
	Ellipse(hdc, top_x, top_y, low_x, low_y);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Set_Ball(ABall *ball)
{
	double ball_x, ball_y;

	// Ставим мячик в центр кирпича
	ball_x = Get_Brick_X_Pos(true);
	ball_y = Get_Brick_Y_Pos(true);

	if (ball != 0)
		ball->Set_State(EBS_Teleporting, ball_x, ball_y);

	Ball = ball;
}
//------------------------------------------------------------------------------------------------------------


// AAdvertisement
//------------------------------------------------------------------------------------------------------------
AAdvertisement::~AAdvertisement()
{
	delete[] Brick_Mask;
}
//------------------------------------------------------------------------------------------------------------
AAdvertisement::AAdvertisement(int level_x, int level_y, int width, int height)
: Level_X(level_x), Level_Y(level_y), Width(width), Height(height), Brick_Mask(0)
{
	Brick_Mask = new char[Width * Height];
	memset(Brick_Mask, 0, Width * Height); // сбросили массив в 0

	Ad_Rect.left = (AsConfig::Level_X_Offset + Level_X * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	Ad_Rect.top = (AsConfig::Level_Y_Offset + Level_Y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	Ad_Rect.right = Ad_Rect.left + Width * AsConfig::Cell_Width * AsConfig::Global_Scale;
	Ad_Rect.bottom = Ad_Rect.top + Height * AsConfig::Cell_Height * AsConfig::Global_Scale;
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Act()
{
	int i, j;
	int cell_width = AsConfig::Cell_Width * AsConfig::Global_Scale;
	int cell_height = AsConfig::Cell_Height * AsConfig::Global_Scale;

	RECT rect{};

	for (i = 0; i < Height; i++) // y
		for (j = 0; j < Width; j++) // x
			if (Brick_Mask[i * Width + j] == 1) // строка * ширину + смещение внутри этой строки
			{
				rect.left = Ad_Rect.left + j * cell_width; // смещение области рекламы на х ячеек 
				rect.top = Ad_Rect.top + i * cell_height; // смещение на y ячеек по высоте
				rect.right = rect.left + cell_width;
				rect.bottom = rect.top + cell_height;

				InvalidateRect(AsConfig::Hwnd, &rect, FALSE);
			}
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Draw(HDC hdc, RECT& paint_area)
{
	const int scale = AsConfig::Global_Scale;
	RECT inresection_rect{};

	if ( !IntersectRect(&inresection_rect, &paint_area, &Ad_Rect) )
		return;

	// 1. Стол
	// 1.1 Белая поверхность
	AsConfig::White_Color.Select(hdc);
	MoveToEx(hdc, Ad_Rect.left + 1, Ad_Rect.top + 15 * scale, 0);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale);
	LineTo(hdc, Ad_Rect.left + 30 * scale + 1, Ad_Rect.top + 15 * scale);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale);
	LineTo(hdc, Ad_Rect.left + 1, Ad_Rect.top + 15 * scale);

	FloodFill(hdc, Ad_Rect.left + 15 * scale, Ad_Rect.top + 15 * scale, AsConfig::White_Color.Get_RGB() );

	// 2. Тень под шариком
	// 2.1 Эллипс размер 8х6, пока шарик над столом
	AsConfig::Blue_Color.Select(hdc);
	Ellipse(hdc, Ad_Rect.left + 11 * scale + 1, Ad_Rect.top + 14 * scale, Ad_Rect.left + 20 * scale - 1, Ad_Rect.top + 18 * scale - 1);



	// 2.2 Уезжает вниз, когда шарик в верхней точке
	// 2.3 Увеличивается, когда шарик плющится


	// Каемки стола
	// 3.1 Синяя кайма толщиной в 1 пиксел
	AsConfig::Advertising_Blue_Table_Color.Select(hdc);
	MoveToEx(hdc, Ad_Rect.left + 1, Ad_Rect.top + 15 * scale, 0);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale);
	LineTo(hdc, Ad_Rect.left + 30 * scale + 1, Ad_Rect.top + 15 * scale);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale);
	LineTo(hdc, Ad_Rect.left + 1, Ad_Rect.top + 15 * scale);

	// 3.2 Розовая кайма толщиной в 2 пиксела
	AsConfig::Advertising_Red_Color.Select(hdc);
	MoveToEx(hdc, Ad_Rect.left + scale - 1, Ad_Rect.top + 16 * scale, 0);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 21 * scale);
	LineTo(hdc, Ad_Rect.left + 30 * scale, Ad_Rect.top + 16 * scale);

	// 4. Шарик
	// 4.1 Эллипс 12х12
	AsConfig::Red_Color.Select(hdc);
	Ellipse(hdc, Ad_Rect.left + 9 * scale + 1, Ad_Rect.top + 2 * scale, Ad_Rect.left + 21 * scale + 1, Ad_Rect.top + 14 * scale);

	// 5.2 Блик сверху
	// 5.3 Летает сверху-вниз (по затухающей траектории)
	// 5.4 Сплющивается внизу до 16х9

	

	// 6. Рамка
	// 6.1 Тонкая пунктирная рамка скругленная по краям


}
//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Clear(HDC hdc, RECT& paint_area)
{
	RECT inresection_rect{};

	if ( !IntersectRect(&inresection_rect, &paint_area, &Ad_Rect) )
		return;

	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, Ad_Rect.left, Ad_Rect.top, Ad_Rect.right - 1, Ad_Rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
bool AAdvertisement::Is_Finished()
{
	return false; // Реклама не заканчивается до окончания уровня
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Show_Under_Brick(int level_x, int level_y)
{
	int x, y;
	x = level_x - Level_X;
	y = level_y - Level_Y;

	if (x < 0 or x > Width)
		AsConfig::Throw();

	if (y < 0 or y > Height)
		AsConfig::Throw();

	Brick_Mask[y * Width + x] = 1; // вычисляем адрес соответствующий байту внутри нашей маски
}
//------------------------------------------------------------------------------------------------------------
bool AAdvertisement::Has_Brick_At(int level_x, int level_y)
{
	if (level_x >= Level_X and level_x <= Level_X + Width)
		if (level_y >= Level_Y and level_y <= Level_Y + Height)
			return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Ad
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Ad::~AActive_Brick_Ad()
{}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Ad::AActive_Brick_Ad(int level_x, int level_y, AAdvertisement *advertisement)
: AActive_Brick(EBT_Unbreakable, level_x, level_y), Advertisement(advertisement)
{
	if (Advertisement != 0)
		Advertisement->Show_Under_Brick(Level_X, Level_Y);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Act()
{
	/*if (Animation_Step <= Max_Animation_Step)
	{
		++Animation_Step;*/
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	//}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Draw(HDC hdc, RECT &paint_area)
{
	/*RECT inresection_rect{};

	if ( !IntersectRect(&inresection_rect, &paint_area, &Brick_Rect) )
		return;*/
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Ad::Is_Finished()
{
	/*if (Animation_Step >= Max_Animation_Step)
		return true;
	else*/
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Draw_In_Level(HDC hdc, RECT &brick_rect)
{ // Вывод неактивного кирпича на уровне

	int i;
	const int scale = AsConfig::Global_Scale;
	int x = brick_rect.left;
	int y = brick_rect.top;
	int size = (Circle_Size - 1) * scale - 1;

	// Стираем предыдущее изображение
	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, x, y, brick_rect.right + scale, brick_rect.bottom + scale);

	// Рисуем шарики
	for (i = 0; i < 2; i++)
	{
		AsConfig::Red_Color.Select(hdc);
		Ellipse(hdc, x,y, x + 7 * scale, brick_rect.bottom);

		// Рисуем блик на шарике
		AsConfig::White_Color.Select(hdc);
		Arc(hdc, x + scale, y + scale, x + size, y + size, x + 2 * scale, y + scale, x + scale, y + 3 * scale);

		x += 8 * scale;
	}
}
//------------------------------------------------------------------------------------------------------------
