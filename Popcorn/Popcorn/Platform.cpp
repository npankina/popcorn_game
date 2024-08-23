﻿#include "Platform.h"


// class AsPlatform
AHit_Checker_List AsPlatform::Hit_Checker_List;
//------------------------------------------------------------------------------------------------------------
AsPlatform::~AsPlatform()
{
	delete[] Normal_Platform_Image;
}
//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
	: X_Pos(AsConfig::Border_X_Offset), Left_Key_Down(false),
	Right_Key_Down(false), Inner_Width(AsConfig::Platform_Normal_Inner_Width), Rolling_Step(0), Last_Redraw_Timer_Tick(0), Speed(0.0),
	Ball_Set(0), Platform_Glue(Platform_State), Platform_Expanding(Platform_State), Platform_Laser(Platform_State),
	Normal_Platform_Image_Width(0), Normal_Platform_Image_Height(0), Normal_Platform_Image(0), Platform_Rect{},
	Prev_Platform_Rect{}
{
	X_Pos = (AsConfig::Max_X_Pos - AsConfig::Platform_Normal_Width) / 2;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
	double inner_left_x, inner_right_x;
	double inner_top_y, inner_low_y;
	double inner_y;
	double reflection_pos;
	double ball_x, ball_y;
	double circle_x, circle_y, circle_radius;

	if (next_y_pos + AsConfig::Ball_Radius < AsConfig::Platform_Y_Pos)
		return false;

	inner_top_y = (double)(AsConfig::Platform_Y_Pos + 1);
	inner_low_y = (double)(AsConfig::Platform_Y_Pos + AsConfig::Platform_Height - 1);
	inner_left_x = (double)(X_Pos + AsConfig::Platform_Circle_Size - 1);
	inner_right_x = (double)(X_Pos + Get_Current_Platform_Width() - (AsConfig::Platform_Circle_Size - 1));


	// 1. Проверяем отражение от боковых шариков
	circle_radius = (double)AsConfig::Platform_Circle_Size / 2.0;
	circle_x = (double)X_Pos + circle_radius;
	circle_y = (double)AsConfig::Platform_Y_Pos + circle_radius;

	if (AsTools::Reflect_On_Circle(next_x_pos, next_y_pos, circle_x, circle_y, circle_radius, ball))
		goto _on_hit;  // От левого


	circle_x += Get_Current_Platform_Width() - AsConfig::Platform_Circle_Size;


	if (AsTools::Reflect_On_Circle(next_x_pos, next_y_pos, circle_x, circle_y, circle_radius, ball))
		goto _on_hit;  // От правого

	// 2. Проверяем отражение от центральной части платформы
	if (ball->Is_Moving_Up())
		inner_y = inner_low_y;  // От нижней грани
	else
		inner_y = inner_top_y;  // От верхней грани

	if (Hit_Circle_On_Line(next_y_pos - inner_y, next_x_pos, inner_left_x, inner_right_x, AsConfig::Ball_Radius, reflection_pos))
	{
		ball->Reflect(true);
		goto _on_hit;
	}

	return false;

_on_hit:
	if (ball->Get_State() == EBall_State::On_Parachute)
		ball->Set_State(EBall_State::Off_Parachute);

	if (Platform_State == EPlatform_State::Glue and Platform_State.Glue == EPlatform_Transformation::Active)
	{
		ball->Get_Center(ball_x, ball_y);
		ball->Set_State(EBall_State::On_Platform, ball_x, ball_y);
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Begin_Movement()
{
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Finish_Movement()
{
	if (Platform_State.Moving == EPlatform_Moving_State::Stop)
		return;

	Redraw_Platform();

	if (Platform_State.Moving == EPlatform_Moving_State::Stopping)
		Platform_State.Moving = EPlatform_Moving_State::Stop;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Advance(double max_speed)
{
	double next_step;

	if (Platform_State.Moving == EPlatform_Moving_State::Stopping or Platform_State.Moving == EPlatform_Moving_State::Stop)
		return;

	next_step = Speed / max_speed * AsConfig::Moving_Step_Size;
	X_Pos += next_step;

	//--- если коррекция произведена остановить платформу и обнулить скорость
	if (Correct_Platform_Pos())
	{
		Speed = 0.0;
		Platform_State.Moving = EPlatform_Moving_State::Stopping;
	}

	// Смещаем приклеенные мячики
	if ((Platform_State == EPlatform_State::Regular and Platform_State.Regular == EPlatform_Substate_Regular::Ready)
		or (Platform_State == EPlatform_State::Glue and Platform_State.Glue == EPlatform_Transformation::Active))
	{
		if (Platform_State.Moving == EPlatform_Moving_State::Moving_Left)
			Ball_Set->On_Platform_Advance(M_PI, fabs(Speed), max_speed);
		else
			if (Platform_State.Moving == EPlatform_Moving_State::Moving_Right)
				Ball_Set->On_Platform_Advance(0.0, fabs(Speed), max_speed);
	}

	Hit_Checker_List.Check_Hit(Platform_Rect);
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act()
{
	EPlatform_State next_state;
	bool correct_pos = false;


	switch (Platform_State)
	{
	case EPlatform_State::Meltdown:
		Act_For_Meltdown_State();
		break;


	case EPlatform_State::Rolling:
		Act_For_Rolling_State();
		break;


	case EPlatform_State::Glue:
		if (Platform_Glue.Act(Ball_Set, next_state))
			Redraw_Platform();

		if (next_state != EPlatform_State::Unknown)
			Set_State(next_state);
		break;


	case EPlatform_State::Laser:
		if (Platform_Laser.Act(next_state, X_Pos))
			Redraw_Platform();

		if (next_state != EPlatform_State::Unknown)
			Set_State(next_state);
		break;


	case EPlatform_State::Expanding:
		if (Platform_Expanding.Act(X_Pos, next_state, correct_pos))
			Redraw_Platform();

		if (correct_pos)
			Correct_Platform_Pos();

		if (next_state != EPlatform_State::Unknown)
			Set_State(next_state);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Prev_Platform_Rect))
		return;

	switch (Platform_State)
	{
	case EPlatform_State::Regular:
	{
		if (Platform_State.Regular == EPlatform_Substate_Regular::Missing)
			break;
	}
	// else - no break!

	case EPlatform_State::Meltdown:
	case EPlatform_State::Rolling:
	case EPlatform_State::Glue:
	case EPlatform_State::Laser:
	case EPlatform_State::Expanding:
		// Очищаем фоном прежнее место
		AsConfig::BG_Color.Select(hdc);
		Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw(HDC hdc, RECT &paint_area)
{// Рисуем платформу

	RECT intersection_rect;

	if (!IntersectRect(&intersection_rect, &paint_area, &Platform_Rect))
		return;

	switch (Platform_State)
	{
	case EPlatform_State::Regular:
		if (Platform_State.Regular == EPlatform_Substate_Regular::Ready or Platform_State.Regular == EPlatform_Substate_Regular::Normal)
			Draw_Normal_State(hdc, paint_area);
		break;

	case EPlatform_State::Meltdown:
		if (Platform_State.Meltdown == EPlatform_Substate_Meltdown::Active)
			Draw_Meltdown_State(hdc, paint_area);
		break;

	case EPlatform_State::Rolling:
		Draw_Rolling_State(hdc, paint_area);
		break;

	case EPlatform_State::Glue:
		Draw_Normal_State(hdc, paint_area);
		Platform_Glue.Draw(hdc, X_Pos);
		break;

	case EPlatform_State::Laser:
		Platform_Laser.Draw_State(hdc, X_Pos, Platform_Rect);
		break;

	case EPlatform_State::Expanding:
		Platform_Expanding.Draw_State(hdc, X_Pos);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Is_Finished()
{
	return false;  // Заглушка, т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Init(AsBall_Set *ball_set, AsLaser_Beam_Set *laser_beam_set)
{
	Ball_Set = ball_set;
	Platform_Laser.Init(laser_beam_set);
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform::Get_State()
{
	return Platform_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_State new_state)
{
	int i, len;

	if (Platform_State == new_state)
		return;

	switch (new_state)
	{
	case EPlatform_State::Regular:
		AsConfig::Throw();  // Состояние EPlatform_State::Regular устанавливается неявно при вызове Set_State(EPlatform_Substate_Regular new_regular_state)
		break;

	case EPlatform_State::Meltdown:
		if (Platform_State != EPlatform_State::Regular)
		{ // сохраняем новое состояние платформы в переменную, чтобы отыгрывать анимацию в правильном порядке (отложенное сост.)
			Platform_State.Set_Next_State(new_state);
			return;
		} // иначе мы применяем то состояние, которое запрошено

		Speed = 0.0;
		Platform_State.Meltdown = EPlatform_Substate_Meltdown::Init;
		len = sizeof(Meltdown_Platform_Y_Pos) / sizeof(Meltdown_Platform_Y_Pos[0]);

		for (i = 0; i < len; i++)
			Meltdown_Platform_Y_Pos[i] = Platform_Rect.top;

		break;

	case EPlatform_State::Rolling:
		Platform_State.Rolling = EPlatform_Substate_Rolling::Roll_In;
		X_Pos = AsConfig::Max_X_Pos - 1;
		Rolling_Step = Max_Rolling_Step - 1;
		Redraw_Platform();
		break;


	case EPlatform_State::Glue:
		if (Set_Transformation_State(new_state, Platform_State.Glue))
			return;
		else
		{
			Platform_State.Glue = EPlatform_Transformation::Init;
			Platform_Glue.Reset();
		}
		break;


	case EPlatform_State::Laser:
		if (Set_Transformation_State(new_state, Platform_State.Laser))
			return;
		else
		{
			Platform_State.Laser = EPlatform_Transformation::Init;
			Platform_Laser.Reset();
		}
		break;

	case EPlatform_State::Expanding:
		if (Set_Transformation_State(new_state, Platform_State.Expanding))
			return;
		else
		{
			Platform_State.Expanding = EPlatform_Transformation::Init;
			Platform_Expanding.Reset();
		}
		break;
	}

	Platform_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
//void AsPlatform::Set_State(EPlatform_Substate_Regular new_regular_state)
//{
//	EPlatform_Transformation *transformation_state = 0;
//	EPlatform_State next_state;
//
//	if (Platform_State == EPlatform_State::Regular and Platform_State.Regular == new_regular_state)
//		return;
//
//	if (new_regular_state == EPlatform_Substate_Regular::Normal)
//	{
//		switch (Platform_State)
//		{
//		case EPlatform_State::Glue:
//			transformation_state = &Platform_State.Glue;
//			break;
//
//		case EPlatform_State::Laser:
//			transformation_state = &Platform_State.Laser;
//			break;
//
//		case EPlatform_State::Expanding:
//			transformation_state = &Platform_State.Expanding;
//			break;
//		}
//
//		if (transformation_state != 0)
//		{
//			if (*transformation_state == EPlatform_Transformation::Unknown)	
//			{// Финализация состояния закончилась
//
//				next_state = Platform_State.Set_Next_Or_Regular_State(new_regular_state);
//				if (next_state != EPlatform_State::Unknown)
//					Set_State(next_state);
//			}
//			else
//				*transformation_state = EPlatform_Transformation::Finalize; // Запускаем финализацию состояния
//		
//			return;
//		}
//	}
//
//	Platform_State = EPlatform_State::Regular;
//	Platform_State.Regular = new_regular_state;
//}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_Substate_Regular new_regular_state)
{
	EPlatform_State next_state;
	next_state = Platform_State.Set_State(new_regular_state);

	if (next_state != EPlatform_State::Unknown)
		Set_State(next_state);
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Has_State(EPlatform_Substate_Regular regular_state)
{
	if (Platform_State != EPlatform_State::Regular)
		return false;

	if (Platform_State.Regular == regular_state)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Redraw_Platform()
{
	//--- если мы заказываем повторную перерисовку платформы не нужно обновлять предыдущий прямоугольник 
	if (Last_Redraw_Timer_Tick != AsConfig::Current_Timer_Tick)
	{
		Prev_Platform_Rect = Platform_Rect;
		Last_Redraw_Timer_Tick = AsConfig::Current_Timer_Tick; //--- при последующем вызове переменная отображает время последней перерисовки
	}

	Platform_Rect.left = (int)(X_Pos * AsConfig::D_Global_Scale);
	Platform_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
	Platform_Rect.right = (int)((X_Pos + Get_Current_Platform_Width()) * AsConfig::D_Global_Scale);
	Platform_Rect.bottom = Platform_Rect.top + AsConfig::Platform_Height * AsConfig::Global_Scale;

	if (Platform_State == EPlatform_State::Meltdown)
		Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;

	AsTools::Invalidate_Rect(Prev_Platform_Rect);
	AsTools::Invalidate_Rect(Platform_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Move(bool to_left, bool key_down)
{
	if (!(Has_State(EPlatform_Substate_Regular::Normal) or Platform_State == EPlatform_State::Glue
		or Platform_State == EPlatform_State::Expanding or Platform_State == EPlatform_State::Laser))
		return;

	if (to_left)
		Left_Key_Down = key_down;
	else
		Right_Key_Down = key_down;

	if (Left_Key_Down and Right_Key_Down)
		return;  // Игнорируем одновременное нажатие двух клавиш

	if (!Left_Key_Down and !Right_Key_Down)
	{
		Speed = 0.0;
		Platform_State.Moving = EPlatform_Moving_State::Stopping;
		return;
	}


	if (Left_Key_Down)
	{
		Platform_State.Moving = EPlatform_Moving_State::Moving_Left;
		Speed = -X_Step;
	}
	if (Right_Key_Down)
	{
		Platform_State.Moving = EPlatform_Moving_State::Moving_Right;
		Speed = X_Step;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::On_Space_Key(bool key_down)
{
	if (Has_State(EPlatform_Substate_Regular::Ready))
	{
		if (!key_down)
		{
			Ball_Set->Release_From_Platform(Get_Middle_Pos());
			Set_State(EPlatform_Substate_Regular::Normal);
		}
	}
	else
	{
		if (Platform_State == EPlatform_State::Glue)
		{
			if (!key_down)
				Ball_Set->Release_Next_Ball();
		}

		else if (Platform_State == EPlatform_State::Laser)
			Platform_Laser.Fire(key_down);
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Hit_By(AFalling_Letter *falling_letter)
{
	RECT intersection_rect, falling_letter_rect;

	falling_letter->Get_Letter_Cell(falling_letter_rect);

	if (IntersectRect(&intersection_rect, &falling_letter_rect, &Platform_Rect))
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Middle_Pos()
{
	return X_Pos + Get_Current_Platform_Width() / 2.0;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Set_Transformation_State(EPlatform_State new_state, EPlatform_Transformation &transformation_state)
{
	if (Platform_State != EPlatform_State::Regular)
	{
		Platform_State.Set_Next_State(new_state);
		return true;
	}

	if (transformation_state == EPlatform_Transformation::Finalize)
		return true;
	else
	{
		transformation_state = EPlatform_Transformation::Init;
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Meltdown_State()
{
	switch (Platform_State.Meltdown)
	{
	case EPlatform_Substate_Meltdown::Init:
		Platform_State.Meltdown = EPlatform_Substate_Meltdown::Active;
		break;

	case EPlatform_Substate_Meltdown::Active:
		break;
	}

	Redraw_Platform();
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Rolling_State()
{
	switch (Platform_State.Rolling)
	{
	case EPlatform_Substate_Rolling::Roll_In:
		++Rolling_Step;

		if (Rolling_Step >= Max_Rolling_Step)
			Rolling_Step -= Max_Rolling_Step;

		X_Pos -= Rolling_Platform_Speed;

		if (X_Pos <= Roll_In_Platform_End_X_Pos)
		{
			X_Pos = Roll_In_Platform_End_X_Pos;
			Platform_State.Rolling = EPlatform_Substate_Rolling::Expand_Roll_In;
			Inner_Width = 1;
		}
		break;


	case EPlatform_Substate_Rolling::Expand_Roll_In:
		--X_Pos;
		Inner_Width += 2;

		if (Inner_Width >= AsConfig::Platform_Normal_Inner_Width)
		{
			Inner_Width = AsConfig::Platform_Normal_Inner_Width;
			Set_State(EPlatform_Substate_Regular::Ready);
			Platform_State.Rolling = EPlatform_Substate_Rolling::Unknown;
			Redraw_Platform();
		}
		break;
	}

	Redraw_Platform();
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Normal_State(HDC hdc, RECT &paint_area)
{// Рисуем платформу в нормальном состоянии

	double x = X_Pos;
	int y = AsConfig::Platform_Y_Pos;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	RECT inner_rect{}, rect{};

	// 1. Рисуем боковые шарики
	AsConfig::Platform_Circle_Color.Select(hdc);

	rect.left = (int)(x * d_scale);
	rect.top = y * scale;
	rect.right = (int)((x + (double)AsConfig::Platform_Circle_Size) * d_scale);
	rect.bottom = (y + AsConfig::Platform_Circle_Size) * scale;

	Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);

	rect.left = (int)( (x + Inner_Width) * d_scale);
	rect.right = (int)( (x + (double)AsConfig::Platform_Circle_Size + Inner_Width) * d_scale);

	Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);

	// 2. Рисуем блик
	Platform_Expanding.Draw_Circle_Highlight(hdc, (int)(x * d_scale), y * scale);

	// 3. Рисуем среднюю часть
	AsConfig::Platform_Inner_Color.Select(hdc);

	inner_rect.left = (int)((x + 4) * d_scale);
	inner_rect.top = (y + 1) * scale;
	inner_rect.right = (int)((x + 4 + Inner_Width - 1) * d_scale);
	inner_rect.bottom = (y + 1 + 5) * scale;

	AsTools::Round_Rect(hdc, inner_rect, 3);

	if (Normal_Platform_Image == 0 and Has_State(EPlatform_Substate_Regular::Ready))
		Get_Normal_Platform_Image(hdc);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Get_Normal_Platform_Image(HDC hdc)
{
	int i, j;
	int x = (int)(X_Pos * AsConfig::D_Global_Scale);
	int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
	int offset = 0;


	Normal_Platform_Image_Width = AsConfig::Platform_Normal_Width * AsConfig::Global_Scale;
	Normal_Platform_Image_Height = AsConfig::Platform_Height * AsConfig::Global_Scale;

	Normal_Platform_Image = new int[Normal_Platform_Image_Width * Normal_Platform_Image_Height];

	for (i = 0; i < Normal_Platform_Image_Height; i++)
		for (j = 0; j < Normal_Platform_Image_Width; j++)
			Normal_Platform_Image[offset++] = GetPixel(hdc, x + j, y + i);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Meltdown_State(HDC hdc, RECT &paint_area)
{// Рисуем платформу в расплавленном состоянии

	int i, j;
	int x, y;
	int y_offset;
	int stroke_len;

	int moved_columns_count = 0;
	int max_platform_y;
	const AColor *color;

	max_platform_y = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;

	for (i = 0; i < Normal_Platform_Image_Width; i++)
	{
		if (Meltdown_Platform_Y_Pos[i] > max_platform_y)
			continue;

		++moved_columns_count;

		y_offset = AsTools::Rand(Meltdown_Speed) + 1;
		x = Platform_Rect.left + i;

		j = 0;
		y = Meltdown_Platform_Y_Pos[i];


		MoveToEx(hdc, x, y, 0);

		// Рисуем последовательность вертикальных штрихов разного цвета (согласно прообразу, сохранённому в Normal_Platform_Image)
		while (Get_Platform_Image_Stroke_Color(i, j, &color, stroke_len))
		{
			color->Select_Pen(hdc);
			LineTo(hdc, x, y + stroke_len);

			y += stroke_len;
			j += stroke_len;
		}

		// Стираем фоном пиксели над штрихом
		y = Meltdown_Platform_Y_Pos[i];
		MoveToEx(hdc, x, y, 0);
		AsConfig::BG_Color.Select_Pen(hdc);
		LineTo(hdc, x, y + y_offset);


		Meltdown_Platform_Y_Pos[i] += y_offset;
	}

	if (moved_columns_count == 0)
		Set_State(EPlatform_Substate_Regular::Missing);  // Вся платформа сдвинулась за пределы окна
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Rolling_State(HDC hdc, RECT &paint_area)
{// Рисуем выкатывающуюся и расширяющуюся платформу

	switch (Platform_State.Rolling)
	{
	case EPlatform_Substate_Rolling::Roll_In:
		Draw_Roll_In_State(hdc, paint_area);
		break;

	case EPlatform_Substate_Rolling::Expand_Roll_In:
		Draw_Normal_State(hdc, paint_area);  // Рисуем расширяющуюся после выкатывания платформу
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Roll_In_State(HDC hdc, RECT &paint_area)
{// Рисуем выкатывающуюся платформу

	int x = (int)(X_Pos * AsConfig::D_Global_Scale);
	int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
	int roller_size = AsConfig::Platform_Circle_Size * AsConfig::Global_Scale;
	double alpha;
	XFORM xform{}, old_xform{};

	// 1. Шарик
	AsConfig::Platform_Circle_Color.Select(hdc);

	Ellipse(hdc, x, y, x + roller_size - 1, y + roller_size - 1);


	// 2. Разделительная линия
	alpha = -2.0 * M_PI / (double)Max_Rolling_Step * (double)Rolling_Step;

	xform.eM11 = (float)cos(alpha);
	xform.eM12 = (float)sin(alpha);
	xform.eM21 = (float)-sin(alpha);
	xform.eM22 = (float)cos(alpha);
	xform.eDx = (float)(x + roller_size / 2);
	xform.eDy = (float)(y + roller_size / 2);
	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	AsConfig::BG_Color.Select(hdc);

	Rectangle(hdc, -AsConfig::Global_Scale / 2, -roller_size / 2, AsConfig::Global_Scale / 2, roller_size / 2);

	SetWorldTransform(hdc, &old_xform);


	// 3. Блик
	Platform_Expanding.Draw_Circle_Highlight(hdc, x, y);
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Get_Platform_Image_Stroke_Color(int x, int y, const AColor **color, int &stroke_len)
{// Вычисляет длину очередного вертикального штриха

	int i;
	int offset = y * Normal_Platform_Image_Width + x;  // Позиция в массиве Normal_Platform_Image, соответствующая смещению (x, y)
	int color_value;

	stroke_len = 0;

	if (y >= Normal_Platform_Image_Height)
		return false;

	for (i = y; i < Normal_Platform_Image_Height; i++)
	{
		if (i == y)
		{
			color_value = Normal_Platform_Image[offset];
			stroke_len = 1;
		}
		else
		{
			if (color_value == Normal_Platform_Image[offset])
				++stroke_len;
			else
				break;
		}

		offset += Normal_Platform_Image_Width;  // Переходим на строку ниже
	}

	if (color_value == AsConfig::Highlight_Color.Get_RGB())
		*color = &AsConfig::Highlight_Color;
	else if (color_value == AsConfig::Platform_Circle_Color.Get_RGB())
		*color = &AsConfig::Platform_Circle_Color;
	else if (color_value == AsConfig::Platform_Inner_Color.Get_RGB())
		*color = &AsConfig::Platform_Inner_Color;
	else if (color_value == AsConfig::BG_Color.Get_RGB())
		*color = &AsConfig::BG_Color;
	else
		AsConfig::Throw();

	return true;
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Current_Platform_Width()
{
	//--- вычисляем ширину платформы для разных состояний
	if (Platform_State == EPlatform_State::Rolling and Platform_State.Rolling == EPlatform_Substate_Rolling::Roll_In)
		return (double)AsConfig::Platform_Circle_Size;
	else if (Platform_State == EPlatform_State::Expanding)
		return Platform_Expanding.Expanding_Platform_Width;
	else
		return (double)AsConfig::Platform_Normal_Width;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Correct_Platform_Pos()
{ //--- проверка что край платформы не залез за край рамки слева и справа
	double 	max_platform_x = AsConfig::Max_X_Pos - Get_Current_Platform_Width();
	double 	min_platform_x = AsConfig::Border_X_Offset + 1;
	bool got_correction = false;

	if (X_Pos <= min_platform_x)
	{
		X_Pos = min_platform_x;
		got_correction = true;
	}

	if (X_Pos >= max_platform_x)
		X_Pos = max_platform_x;

	return got_correction;
}
