#include "Platform.h"

// AsPlatform_State
//------------------------------------------------------------------------------------------------------------
AsPlatform_State::AsPlatform_State()
: Current_State(EPlatform_State::Regular), Next_State(EPlatform_State::Unknown), Regular(EPlatform_Substate_Regular::Missing),
  Meltdown(EPlatform_Substate_Meltdown::Unknown), Rolling(EPlatform_Substate_Rolling::Unknown),
  Glue(EPlatform_Transformation::Unknown), Expanding(EPlatform_Transformation::Unknown), Laser(EPlatform_Transformation::Unknown),
  Moving(EPlatform_Moving_State::Stop)
{}
//------------------------------------------------------------------------------------------------------------
AsPlatform_State::operator EPlatform_State() const
{
	return Current_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_State::operator = (EPlatform_State new_state)
{
	Current_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_State::Set_Next_State(EPlatform_State next_state)
{
	if (next_state == Current_State)
		return;

	switch (Current_State)
	{
	case EPlatform_State::Regular: 
		AsConfig::Throw(); // обычное состояние само не заканчивается, переключение в другое состояние должно быть явным
		break;
	
	case EPlatform_State::Meltdown:
		return; // игнорируем любое состояние после Meltdown, т.к. после него перезапускается игровой процесс

	case EPlatform_State::Rolling:
		AsConfig::Throw(); // состояние Rolling само должно переключаться в следующее
		break;

	case EPlatform_State::Glue:
		Glue = EPlatform_Transformation::Finalize;
		break;

	case EPlatform_State::Laser:
		Laser = EPlatform_Transformation::Finalize;
		break;

	case EPlatform_State::Expanding:
		Expanding = EPlatform_Transformation::Finalize;
		break;

	default:
		AsConfig::Throw();
	}

	Next_State = next_state;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform_State::Get_Next_State()
{
	return Next_State;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform_State::Set_Next_Or_Regular_State(EPlatform_Substate_Regular new_regular_state)
{// Возврат: если не Unknown, надо перейти в это состояние

	EPlatform_State next_state;

	Current_State = EPlatform_State::Regular;

	// Если есть отложенное состояние, то переведем в него, иначе в Regular
	next_state = Get_Next_State();

	if (next_state == EPlatform_State::Unknown)
		Regular = new_regular_state;

	return next_state;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform_State::Set_State(EPlatform_Substate_Regular new_regular_state)
{
	EPlatform_Transformation *transformation_state = 0;

	if (Current_State == EPlatform_State::Regular && Regular == new_regular_state)
		return EPlatform_State::Unknown;

	if (new_regular_state == EPlatform_Substate_Regular::Normal)
	{
		switch (Current_State)
		{
		case EPlatform_State::Glue:
			transformation_state = &Glue;
			break;

		case EPlatform_State::Laser:
			transformation_state = &Laser;
			break;

		case EPlatform_State::Expanding:
			transformation_state = &Expanding;
			break;
		}

		if (transformation_state != 0)
		{
			if (*transformation_state == EPlatform_Transformation::Unknown)	
			// Финализация состояния закончилась
				return Set_Next_Or_Regular_State(new_regular_state);
				/*if (next_state != EPlatform_State::Unknown)
				Set_State(next_state);*/
			else
				*transformation_state = EPlatform_Transformation::Finalize; // Запускаем финализацию состояния

			return EPlatform_State::Unknown;
		}
	}

	Current_State = EPlatform_State::Regular;
	Regular = new_regular_state;

	return EPlatform_State::Unknown;
}
//------------------------------------------------------------------------------------------------------------




// AsPlatform_Glue
const double AsPlatform_Glue::Max_Glue_Spot_Height_Ratio = 1.0;
const double AsPlatform_Glue::Min_Glue_Spot_Height_Ratio = 0.4;
const double AsPlatform_Glue::Glue_Spot_Height_Ratio_Step = 0.05;
//------------------------------------------------------------------------------------------------------------
AsPlatform_Glue::AsPlatform_Glue(AsPlatform_State &platform_state)
: Glue_Spot_Height_Ratio(0.0), Platform_State(&platform_state)
{}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform_Glue::Act_For_Glue_State(EPlatform_Transformation &glue_state, AsBall_Set *ball_set, EPlatform_State &next_state)
{// метод анимации клея

	next_state = EPlatform_State::Unknown;

	switch (glue_state)
	{
	case EPlatform_Transformation::Init:
		if (Glue_Spot_Height_Ratio < Max_Glue_Spot_Height_Ratio)
			Glue_Spot_Height_Ratio += Glue_Spot_Height_Ratio_Step;
		else
			glue_state = EPlatform_Transformation::Active;

		return true;


	case EPlatform_Transformation::Active:
		break;


	case EPlatform_Transformation::Finalize:
		if (Glue_Spot_Height_Ratio > Min_Glue_Spot_Height_Ratio)
		{
			Glue_Spot_Height_Ratio -= Glue_Spot_Height_Ratio_Step;
			while (ball_set->Release_Next_Ball() ) //--- отпускает все мячики на платформе
			{}
		}
		else
		{
			glue_state = EPlatform_Transformation::Unknown;
			next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
		}

		return true;

	default:
		AsConfig::Throw();
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Glue::Draw_Glue_State(HDC hdc, double x_pos)
{// Рисуем платформу с растекающимся клеем

	HRGN region;
	RECT glue_rect{};

	glue_rect.left = (int)( (x_pos + 5.0) * AsConfig::D_Global_Scale);
	glue_rect.top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
	glue_rect.right = glue_rect.left + AsPlatform::Normal_Platform_Inner_Width * AsConfig::Global_Scale;
	glue_rect.bottom = glue_rect.top + (AsPlatform::Height - 2) * AsConfig::Global_Scale;

	region = CreateRectRgnIndirect(&glue_rect);
	SelectClipRgn(hdc, region);

	AsConfig::BG_Color.Select(hdc);
	Draw_Glue_Spot(hdc, x_pos, 0, 9, 5);
	Draw_Glue_Spot(hdc, x_pos, 6, 6, 5);
	Draw_Glue_Spot(hdc, x_pos, 9, 9, 6);

	AsConfig::White_Color.Select(hdc);
	Draw_Glue_Spot(hdc, x_pos, 0, 9, 4);
	Draw_Glue_Spot(hdc, x_pos, 6, 6, 4);
	Draw_Glue_Spot(hdc, x_pos, 9, 9, 5);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Glue::Reset()
{
	Glue_Spot_Height_Ratio = Min_Glue_Spot_Height_Ratio;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Glue::Draw_Glue_Spot(HDC hdc, double x_pos, int x_offset, int width, int height)
{// Рисуем пятно клея

	RECT spot_rect{};
	int platform_top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
	int spot_height = (int)( (double)height * AsConfig::D_Global_Scale * Glue_Spot_Height_Ratio);

	// Рисуем полуэллипс как "пятно" клея
	spot_rect.left = (int)( (x_pos + 5.0 + (double)x_offset) * AsConfig::D_Global_Scale);
	spot_rect.top = platform_top - spot_height;
	spot_rect.right = spot_rect.left + width * AsConfig::Global_Scale;
	spot_rect.bottom = platform_top + spot_height - AsConfig::Global_Scale;

	Chord(hdc, spot_rect.left, spot_rect.top, spot_rect.right - 1, spot_rect.bottom - 1,  spot_rect.left, platform_top - 1, spot_rect.right - 1, platform_top - 1);
}
//------------------------------------------------------------------------------------------------------------




// AsPlatform
//const double AsPlatform::Max_Glue_Spot_Height_Ratio = 1.0;
//const double AsPlatform::Min_Glue_Spot_Height_Ratio = 0.4;
//const double AsPlatform::Glue_Spot_Height_Ratio_Step = 0.05;
const double AsPlatform::Max_Expanding_Platform_Width = 40.0;
const double AsPlatform::Min_Expanding_Platform_Width = (double)Normal_Width;
const double AsPlatform::Expanding_Platform_Width_Step = 1.0;
//------------------------------------------------------------------------------------------------------------
AsPlatform::~AsPlatform()
{
	delete[] Normal_Platform_Image;
}
//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
: X_Pos(AsConfig::Border_X_Offset), Left_Key_Down(false),
  Right_Key_Down(false), Inner_Width(Normal_Platform_Inner_Width), Rolling_Step(0), Last_Redraw_Timer_Tick(0), Speed(0.0),
  Expanding_Platform_Width(0.0), Ball_Set(0), Platform_Glue(Platform_State), Normal_Platform_Image_Width(0), Normal_Platform_Image_Height(0),
  Normal_Platform_Image(0), Platform_Rect{}, Prev_Platform_Rect{}, Highlight_Color(255, 255, 255), Laser_Transformation_Step(0),
  Platform_Circle_Color(230, 25, 229), Platform_Inner_Color(0, 255, 255), Truss_Color(Platform_Inner_Color, AsConfig::Global_Scale),
  Gun_Color(Highlight_Color, AsConfig::Global_Scale)
{
	X_Pos = (double)(AsConfig::Max_X_Pos - Normal_Width) / 2.0;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{
	double inner_left_x, inner_right_x;
	double inner_top_y, inner_low_y;
	double inner_y;
	double reflection_pos;
	double ball_x, ball_y;

	if (next_y_pos + ball->Radius < AsConfig::Platform_Y_Pos)
		return false;

	inner_top_y = (double)(AsConfig::Platform_Y_Pos + 1);
	inner_low_y = (double)(AsConfig::Platform_Y_Pos + Height - 1);
	inner_left_x = (double)(X_Pos + Circle_Size - 1);
	inner_right_x = (double)(X_Pos + Get_Current_Platform_Width() - (Circle_Size - 1) );


	// 1. Проверяем отражение от боковых шариков
	if (Reflect_On_Circle(next_x_pos, next_y_pos, 0.0, ball) )
		goto _on_hit;  // От левого

	if (Reflect_On_Circle(next_x_pos, next_y_pos, Get_Current_Platform_Width() - Circle_Size, ball) )
		goto _on_hit;  // От правого

	// 2. Проверяем отражение от центральной части платформы
	if (ball->Is_Moving_Up() )
		inner_y = inner_low_y;  // От нижней грани
	else
		inner_y = inner_top_y;  // От верхней грани

	if (Hit_Circle_On_Line(next_y_pos - inner_y, next_x_pos, inner_left_x, inner_right_x, ball->Radius, reflection_pos) )
	{
		ball->Reflect(true);
		goto _on_hit;
	}

	return false;

_on_hit:
	if (ball->Get_State() == EBS_On_Parachute)
		ball->Set_State(EBS_Off_Parachute);

	if (Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active)
	{
		ball->Get_Center(ball_x, ball_y);
		ball->Set_State(EBS_On_Platform, ball_x, ball_y);
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

	if (Platform_State.Moving == EPlatform_Moving_State::Stopping || Platform_State.Moving == EPlatform_Moving_State::Stop)
		return;

	next_step = Speed / max_speed * AsConfig::Moving_Step_Size;
	X_Pos += next_step;

	//--- если коррекция произведена остановить платформу и обнулить скорость
	if (Correct_Platform_Pos() )
	{
		Speed = 0.0;
		Platform_State.Moving = EPlatform_Moving_State::Stopping;
	}

	// Смещаем приклеенные мячики
	if ( (Platform_State == EPlatform_State::Regular && Platform_State.Regular == EPlatform_Substate_Regular::Ready)
		|| (Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active) )
	{
		if (Platform_State.Moving == EPlatform_Moving_State::Moving_Left)
			Ball_Set->On_Platform_Advance(M_PI, fabs(Speed), max_speed);
		else
			if (Platform_State.Moving == EPlatform_Moving_State::Moving_Right)
				Ball_Set->On_Platform_Advance(0.0, fabs(Speed), max_speed);
	}
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

	switch (Platform_State)
	{
	case EPlatform_State::Meltdown:
		Act_For_Meltdown_State();
		break;

	case EPlatform_State::Rolling:
		Act_For_Rolling_State();
		break;

	case EPlatform_State::Glue:
		if (Platform_Glue.Act_For_Glue_State(Platform_State.Glue, Ball_Set, next_state) )
			Redraw_Platform();

		if (next_state != EPlatform_State::Unknown)
			Set_State(next_state);
		break;

	case EPlatform_State::Laser:
		Act_For_Laser_State();
		break;

	case EPlatform_State::Expanding:
		Act_For_Expanding_State();
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Prev_Platform_Rect) )
		return;

	switch (Platform_State)
	{
	case EPlatform_State::Regular:
	{
		if (Platform_State.Regular == EPlatform_Substate_Regular::Missing)
			break;
	}
	// else - no break!

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

	if (! IntersectRect(&intersection_rect, &paint_area, &Platform_Rect) )
		return;

	switch (Platform_State)
	{
	case EPlatform_State::Regular:
		if (Platform_State.Regular == EPlatform_Substate_Regular::Ready || Platform_State.Regular == EPlatform_Substate_Regular::Normal)
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
		Platform_Glue.Draw_Glue_State(hdc, X_Pos);
		break;

	case EPlatform_State::Laser:
		Draw_Laser_State(hdc, paint_area);
		break;

	case EPlatform_State::Expanding:
		Draw_Expanding_State(hdc, paint_area);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Is_Finished()
{
	return false;  // Заглушка, т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Init(AsBall_Set *ball_set)
{
	Ball_Set = ball_set;
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
		AsConfig::Throw();  // Состояние EPlatform_State::Regular устанавлвается неявно при вызове Set_State(EPlatform_Substate_Regular new_regular_state)
		break;

	case EPlatform_State::Meltdown:
		{ //--- сохраняем новое состояние платформы в переменную, чтобы отыгрывать анимацию в правильном порядке (отложенное сост.)
			Platform_State.Set_Next_State(new_state);
			return;
		} //--- иначе мы применяем то состояние, которое запрошено

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
		break;


	case EPlatform_State::Glue:
		if ( Set_Transformation_State(new_state, Platform_State.Glue) )
			return;
		else
		{
			Platform_State.Glue = EPlatform_Transformation::Init;
			Platform_Glue.Reset();
		}
		break;
		

	case EPlatform_State::Laser:
		if ( Set_Transformation_State(new_state, Platform_State.Laser) )
			return;
		else
		{
			Platform_State.Laser = EPlatform_Transformation::Init;
			Laser_Transformation_Step = 0;
		}
		break;

	case EPlatform_State::Expanding:
		if ( Set_Transformation_State(new_state, Platform_State.Expanding) )
			return;
		else
		{
			Platform_State.Expanding = EPlatform_Transformation::Init;
			Expanding_Platform_Width = Min_Expanding_Platform_Width;
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
//	if (Platform_State == EPlatform_State::Regular && Platform_State.Regular == new_regular_state)
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
	Platform_Rect.right = (int)( (X_Pos + Get_Current_Platform_Width() ) * AsConfig::D_Global_Scale);
	Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;

	if (Platform_State == EPlatform_State::Meltdown)
		Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;

	AsConfig::Invalidate_Rect(Prev_Platform_Rect);
	AsConfig::Invalidate_Rect(Platform_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Move(bool to_left, bool key_down)
{
	if (! (Has_State(EPlatform_Substate_Regular::Normal) || Platform_State == EPlatform_State::Glue 
		|| Platform_State == EPlatform_State::Expanding || Platform_State == EPlatform_State::Laser) )
		return;

	if (to_left)
		Left_Key_Down = key_down;
	else
		Right_Key_Down = key_down;

	if (Left_Key_Down && Right_Key_Down)
		return;  // Игнорируем одновременное нажатие двух клавиш

	if (! Left_Key_Down && ! Right_Key_Down)
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
	if (! key_down)
		return;

	if (Has_State(EPlatform_Substate_Regular::Ready) )
	{
		Ball_Set->Release_From_Platform(Get_Middle_Pos() );
		Set_State(EPlatform_Substate_Regular::Normal);
	}
	else
	{
		if (Platform_State == EPlatform_State::Glue)
			Ball_Set->Release_Next_Ball();

		else if (Platform_State == EPlatform_State::Laser)
			AsConfig::Throw(); // !!! надо сделать
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Hit_By(AFalling_Letter *falling_letter)
{
	RECT intersection_rect, falling_letter_rect;

	falling_letter->Get_Letter_Cell(falling_letter_rect);

	if (IntersectRect(&intersection_rect, &falling_letter_rect, &Platform_Rect) )
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
		Redraw_Platform();
		break;
	}
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

		if (Inner_Width >= Normal_Platform_Inner_Width)
		{
			Inner_Width = Normal_Platform_Inner_Width;
			Set_State(EPlatform_Substate_Regular::Ready);
			Platform_State.Rolling = EPlatform_Substate_Rolling::Unknown;
			Redraw_Platform();
		}
		break;
	}

	Redraw_Platform();
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Laser_State()
{
	switch (Platform_State.Laser)
	{
	case EPlatform_Transformation::Init:
		if (Laser_Transformation_Step < Max_Laser_Transformation_Step)
			++Laser_Transformation_Step;
		else
			Platform_State.Laser = EPlatform_Transformation::Active;

		Redraw_Platform();
		break;


	case EPlatform_Transformation::Active:
		break;


	case EPlatform_Transformation::Finalize:
		if (Laser_Transformation_Step > 0)
			--Laser_Transformation_Step;
		else
		{
			Platform_State.Laser = EPlatform_Transformation::Unknown;
			Set_State(EPlatform_Substate_Regular::Normal);
		}

		Redraw_Platform();
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
//void AsPlatform::Act_For_Glue_State()
//{// метод анимации клея
//
//	switch (Platform_State.Glue)
//	{
//	case EPlatform_Transformation::Init:
//		if (Glue_Spot_Height_Ratio < Max_Glue_Spot_Height_Ratio)
//			Glue_Spot_Height_Ratio += Glue_Spot_Height_Ratio_Step;
//		else
//			Platform_State.Glue = EPlatform_Transformation::Active;
//
//		Redraw_Platform();
//		break;
//
//
//	case EPlatform_Transformation::Active:
//		break;
//
//
//	case EPlatform_Transformation::Finalize:
//		if (Glue_Spot_Height_Ratio > Min_Glue_Spot_Height_Ratio)
//		{
//			Glue_Spot_Height_Ratio -= Glue_Spot_Height_Ratio_Step;
//			while (Ball_Set->Release_Next_Ball() ) //--- отпускает все мячики на платформе
//			{}
//		}
//		else
//		{
//			Platform_State.Glue = EPlatform_Transformation::Unknown;
//			Set_State(EPlatform_Substate_Regular::Normal);
//		}
//
//		Redraw_Platform();
//		break;
//
//	default:
//		AsConfig::Throw();
//	}
//}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Expanding_State()
{
	switch (Platform_State.Expanding)
	{
	case EPlatform_Transformation::Init:
		
		if (Expanding_Platform_Width < Max_Expanding_Platform_Width)
		{
			Expanding_Platform_Width += Expanding_Platform_Width_Step;
			X_Pos -= Expanding_Platform_Width_Step / 2.0;
			Correct_Platform_Pos();
		}
		else
			Platform_State.Expanding = EPlatform_Transformation::Active;

		Redraw_Platform();
		break;


	case EPlatform_Transformation::Active:
		break;


	case EPlatform_Transformation::Finalize:
		if (Expanding_Platform_Width > Min_Expanding_Platform_Width)
		{
			Expanding_Platform_Width -= Expanding_Platform_Width_Step;
			X_Pos += Expanding_Platform_Width_Step / 2.0;
			Correct_Platform_Pos();
		}
		else
		{
			Platform_State.Expanding = EPlatform_Transformation::Unknown;
			Set_State(EPlatform_Substate_Regular::Normal);
		}

		Redraw_Platform();
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Circle_Highlight(HDC hdc, int x, int y)
{// Рисуем блик на шарике

	Highlight_Color.Select_Pen(hdc);

	Arc(hdc, x + AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + (Circle_Size - 1) * AsConfig::Global_Scale - 1, y + (Circle_Size - 1) * AsConfig::Global_Scale - 1,
		x + 2 * AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + AsConfig::Global_Scale, y + 3 * AsConfig::Global_Scale);
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
	Platform_Circle_Color.Select(hdc);

	rect.left = (int)(x * d_scale);
	rect.top = y * scale;
	rect.right = (int)( (x + (double)Circle_Size) * d_scale);
	rect.bottom = (y + Circle_Size) * scale;

	Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);

	rect.left = (int)( (x + Inner_Width) * d_scale);
	rect.top = y * scale;
	rect.right = (int)( (x + (double)Circle_Size + Inner_Width) * d_scale);
	rect.bottom = (y + Circle_Size) * scale;

	Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);

	// 2. Рисуем блик
	Draw_Circle_Highlight(hdc, (int)(x * d_scale), y * scale);

	// 3. Рисуем среднюю часть
	Platform_Inner_Color.Select(hdc);

	inner_rect.left = (int)( (x + 4) * d_scale);
	inner_rect.top = (y + 1) * scale;
	inner_rect.right = (int)( (x + 4 + Inner_Width - 1) * d_scale);
	inner_rect.bottom = (y + 1 + 5) * scale;

	AsConfig::Round_Rect(hdc, inner_rect, 3);

	if (Normal_Platform_Image == 0 && Has_State(EPlatform_Substate_Regular::Ready) )
		Get_Normal_Platform_Image(hdc);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Get_Normal_Platform_Image(HDC hdc)
{
	int i, j;
	int x = (int)(X_Pos * AsConfig::D_Global_Scale);
	int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
	int offset = 0;


	Normal_Platform_Image_Width = Normal_Width * AsConfig::Global_Scale;
	Normal_Platform_Image_Height = Height * AsConfig::Global_Scale;

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

		y_offset = AsConfig::Rand(Meltdown_Speed) + 1;
		x = Platform_Rect.left + i;

		j = 0;
		y = Meltdown_Platform_Y_Pos[i];


		MoveToEx(hdc, x, y, 0);

		// Рисуем последовательность вертикальных штрихов разного цвета (согласно прообразу, сохранённому в Normal_Platform_Image)
		while (Get_Platform_Image_Stroke_Color(i, j, &color, stroke_len) )
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
	int roller_size = Circle_Size * AsConfig::Global_Scale;
	double alpha;
	XFORM xform{}, old_xform{};

	// 1. Шарик
	Platform_Circle_Color.Select(hdc);

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
	Draw_Circle_Highlight(hdc, x, y);
}
//------------------------------------------------------------------------------------------------------------
//void AsPlatform::Draw_Glue_State(HDC hdc, RECT &paint_area)
//{// Рисуем платформу с растекающимся клеем
//
//	HRGN region;
//	RECT glue_rect{};
//
//	Draw_Normal_State(hdc, paint_area);
//
//	glue_rect.left = (int)( (X_Pos + 5.0) * AsConfig::D_Global_Scale);
//	glue_rect.top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
//	glue_rect.right = glue_rect.left + Normal_Platform_Inner_Width * AsConfig::Global_Scale;
//	glue_rect.bottom = glue_rect.top + (Height - 2) * AsConfig::Global_Scale;
//
//	region = CreateRectRgnIndirect(&glue_rect);
//	SelectClipRgn(hdc, region);
//
//	AsConfig::BG_Color.Select(hdc);
//	Draw_Glue_Spot(hdc, 0, 9, 5);
//	Draw_Glue_Spot(hdc, 6, 6, 5);
//	Draw_Glue_Spot(hdc, 9, 9, 6);
//
//	AsConfig::White_Color.Select(hdc);
//	Draw_Glue_Spot(hdc, 0, 9, 4);
//	Draw_Glue_Spot(hdc, 6, 6, 4);
//	Draw_Glue_Spot(hdc, 9, 9, 5);
//
//	SelectClipRgn(hdc, 0);
//	DeleteObject(region);
//}
//------------------------------------------------------------------------------------------------------------
//void AsPlatform::Draw_Glue_Spot(HDC hdc, int x_offset, int width, int height)
//{// Рисуем пятно клея
//
//	RECT spot_rect{};
//	int platform_top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
//	int spot_height = (int)( (double)height * AsConfig::D_Global_Scale * Glue_Spot_Height_Ratio);
//
//	// Рисуем полуэллипс как "пятно" клея
//	spot_rect.left = (int)( (X_Pos + 5.0 + (double)x_offset) * AsConfig::D_Global_Scale);
//	spot_rect.top = platform_top - spot_height;
//	spot_rect.right = spot_rect.left + width * AsConfig::Global_Scale;
//	spot_rect.bottom = platform_top + spot_height - AsConfig::Global_Scale;
//
//	Chord(hdc, spot_rect.left, spot_rect.top, spot_rect.right - 1, spot_rect.bottom - 1,  spot_rect.left, platform_top - 1, spot_rect.right - 1, platform_top - 1);
//}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Expanding_State(HDC hdc, RECT &paint_area)
{// Рисуем расширяющуюся платформу

	double x = X_Pos;
	int y = AsConfig::Platform_Y_Pos;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	RECT inner_rect{};

	inner_rect.left = (int)( (x + (Expanding_Platform_Width - (double)Expanding_Platform_Inner_Width) / 2.0) * d_scale);
	inner_rect.top = (y + 1) * scale;
	inner_rect.right = inner_rect.left + Expanding_Platform_Inner_Width * scale;
	inner_rect.bottom = (y + 1 + 5) * scale;

	// 1. Левая сторона
	// 1.1. Шарик
	Draw_Expanding_Platform_Ball(hdc, true);

	// 1.2. Фермы
	Draw_Expanding_Truss(hdc, inner_rect, true);

	// 2. Правая сторона
	// 2.1. Шарик
	Draw_Expanding_Platform_Ball(hdc, false);

	// 2.2. Фермы
	Draw_Expanding_Truss(hdc, inner_rect, false);


	//// 1.2. Правый шарик
	//rect.left = (int)( (x + Expanding_Platform_Width - (double)Circle_Size) * d_scale);
	//rect.top = y * scale;
	//rect.right = rect.left + Circle_Size * scale;
	//rect.bottom = (y + Circle_Size) * scale;

	//Platform_Circle_Color.Select(hdc);
	//Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);

	// 3. Рисуем среднюю часть
	Platform_Inner_Color.Select(hdc);

	Rectangle(hdc, inner_rect.left, inner_rect.top, inner_rect.right - 1, inner_rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Expanding_Platform_Ball(HDC hdc, bool is_left)
{// Рисуем боковой шарик расширяющейся платформы

	double x = X_Pos;
	int y = AsConfig::Platform_Y_Pos;
	int arc_mid_x;
	int arc_start_y, arc_end_y, arc_right_offset;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	RECT rect{}, arc_rect{};

	// 1.1. Шарик
	if (is_left)
		rect.left = (int)(x * d_scale);
	else
		rect.left = (int)( (x + Expanding_Platform_Width - (double)Circle_Size) * d_scale);

	rect.top = y * scale;
	rect.right = rect.left + Circle_Size * scale;
	rect.bottom = (y + Circle_Size) * scale;


	Platform_Circle_Color.Select(hdc);
	Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);

	// 1.2. Переходник на ферму
	if (is_left)
		Rectangle(hdc, rect.left + 4 * scale, rect.top, rect.right - scale + 1, rect.bottom - 1);
	else
		Rectangle(hdc, rect.left + 1, rect.top, rect.left + 3 * scale, rect.bottom - 1);

	// 1.3. Рисуем блик
	Draw_Circle_Highlight(hdc, (int)(x * d_scale), y * scale);

	// 1.4. Дуга фермы на шарике
	arc_rect.left = rect.left + 4 * scale + 2;
	arc_rect.top = rect.top + scale + 1;
	arc_rect.right = rect.left + (4 + 3) * scale + 2;
	arc_rect.bottom = rect.bottom - scale - 1;

	arc_mid_x = arc_rect.left + (arc_rect.right - arc_rect.left) / 2;

	if (is_left)
	{
		arc_start_y = arc_rect.top;
		arc_end_y = arc_rect.bottom;
	}
	else
	{
		arc_start_y = arc_rect.bottom;
		arc_end_y = arc_rect.top;

		arc_right_offset = (Circle_Size - 2) * scale + 1;
		arc_rect.left -= arc_right_offset;
		arc_rect.right -= arc_right_offset;
		arc_mid_x -= arc_right_offset;
	}

	// 1.4.1. Дырка в шарике под дугой
	AsConfig::BG_Color.Select(hdc);
	Ellipse(hdc, arc_rect.left, arc_rect.top, arc_rect.right - 1, arc_rect.bottom - 1);

	// 1.4.2. Сама дуга
	Truss_Color.Select(hdc);
	Arc(hdc, arc_rect.left, arc_rect.top, arc_rect.right - 1, arc_rect.bottom - 1,  arc_mid_x, arc_start_y, arc_mid_x, arc_end_y);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Expanding_Truss(HDC hdc, RECT &inner_rect, bool is_left)
{// Рисуем фермы расширяющейся платформы

	int truss_x;
	int truss_top_y, truss_low_y;
	int truss_x_offset;
	double extantion_ratio; // [ 1.0 ... 0.0 ] коэффициент расширения платформы
	const int scale = AsConfig::Global_Scale;

	extantion_ratio = (Max_Expanding_Platform_Width - Expanding_Platform_Width) / (Max_Expanding_Platform_Width - Min_Expanding_Platform_Width);
	truss_x_offset = (int)(6.0 * extantion_ratio * AsConfig::D_Global_Scale);
	
	truss_x = inner_rect.left + 1;

	if (is_left)
		truss_x += truss_x_offset;
	else
	{
		truss_x += (Expanding_Platform_Inner_Width + 8 - 1) * scale + 1;
		truss_x -= truss_x_offset;
	}
	truss_top_y = inner_rect.top + 1;
	truss_low_y = inner_rect.bottom - scale + 1;

	Truss_Color.Select(hdc);
	MoveToEx(hdc, truss_x, truss_top_y, 0);
	LineTo(hdc, truss_x - 4 * scale - 1, truss_low_y);
	LineTo(hdc, truss_x - 8 * scale, truss_top_y);

	MoveToEx(hdc, truss_x, truss_low_y, 0);
	LineTo(hdc, truss_x - 4 * scale - 1, truss_top_y);
	LineTo(hdc, truss_x - 8 * scale, truss_low_y);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Laser_State(HDC hdc, RECT &paint_area)
{
	HRGN region; //--- указывает область обрезки изображения, которые выводится на экран
	
	region = CreateRectRgnIndirect(&Platform_Rect);
	SelectClipRgn(hdc, region); //--- установка области обрезки

	// 1. Левое крыло
	Draw_Laser_Wing(hdc, true);

	// 2. Правое крыло
	Draw_Laser_Wing(hdc, false);

	// 3. Центральная часть
	// 3.0 Нормальная средняя часть
	Draw_Laser_Inner_part(hdc);
	
	// 3.1. Левая нога
	Draw_Laser_Leg(hdc, true);

	// 3.2. Правая нога
	Draw_Laser_Leg(hdc, false);

	// 3.3. Кабина
	Draw_Laser_Cabin(hdc);

	SelectClipRgn(hdc, 0); //--- удаление области обрезки
	DeleteObject(region); //--- удалить объект нужно обязательно!!
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Laser_Inner_part(HDC hdc)
{// рисует уменьшающуюся часть обычной платформы
 // Размер 20 х 5 => 8 x 1

	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;
	int x = (int)X_Pos;
	int y = AsConfig::Platform_Y_Pos;

	Platform_Inner_Color.Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_type::Round_Rect_3x, x + 4, y + 1, 20, 5, ratio, x + 10, y + 1, 8, 1);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Laser_Wing(HDC hdc, bool is_left)
{
	double x, y;
	int x_offset;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;
	double height;
	int half_max_step = Max_Laser_Transformation_Step / 2;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;


	Platform_Circle_Color.Select(hdc); //--- установка цвета полуэллипса

	y = AsConfig::Platform_Y_Pos;
	x = X_Pos;

	if (!is_left)
		x += Normal_Width - Circle_Size;

	// 1. Крыло //--- в форме полуэллипса
	Draw_Expanding_Figure(hdc, EFigure_type::Ellipse, x, y, 7, 7, ratio, x, y + 1, 7, 12);


	// 1.1. Перемычка
	if (is_left)
		x_offset = 5;
	else
		x_offset = -4;

	Draw_Expanding_Figure(hdc, EFigure_type::Rectangle, x + 3, y + 6, 1, 1, ratio, x + x_offset, y + 2, 6, 5);


	// 1.2. Пушка
	Gun_Color.Select(hdc);

	if (Laser_Transformation_Step >= half_max_step)
	{
		ratio = (double)(Laser_Transformation_Step - half_max_step) / (double)half_max_step;

		if (is_left)
			x = X_Pos + 3.0;
		else
			x = X_Pos + (Normal_Width - 4);

		height = 3.0 * (1.0 - ratio) * d_scale;

		MoveToEx(hdc, (int)(x * d_scale + 1.0), (int)(y * d_scale + 3.0 * d_scale + 1.0), 0);
		LineTo(hdc, (int)(x * d_scale + 1.0), (int)(y * d_scale + height + 1.0)); 

		// 1.3. Хвост
		Draw_Expanding_Figure(hdc, EFigure_type::Ellipse, x + 1, y + 5, 0, 0, ratio, x - 1, y + 5 + 1.0 / d_scale, 3, 6);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Laser_Leg(HDC hdc, bool is_left)
{
	double x, y;
	double x_scale;
	int scale = AsConfig::Global_Scale;
	double d_scale = AsConfig::D_Global_Scale;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

	Platform_Inner_Color.Select(hdc);

	if (is_left)
	{
		x = (X_Pos + 6.0) * d_scale;
		x_scale = scale;
	}
	else
	{
		x = X_Pos * d_scale + (Normal_Width - 6) * d_scale - 1.0;
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
void AsPlatform::Draw_Laser_Cabin(HDC hdc)
{
	double x = X_Pos;
	double y = AsConfig::Platform_Y_Pos;
	int scale = AsConfig::Global_Scale;
	double one_pixel = 1.0 / AsConfig::D_Global_Scale;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;
	
	// 3.3.1. Внешняя часть
	Platform_Inner_Color.Select(hdc);
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
void AsPlatform::Draw_Expanding_Figure(HDC hdc, EFigure_type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height)
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

		AsConfig::Round_Rect(hdc, rect, 3);
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
int AsPlatform::Get_Expanding_Value(double start, double end, double ratio)
{
	double delta;
	int value;

	delta = end - start;
	value = (int)( (start + delta * ratio) * AsConfig::Global_Scale);

	return value;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall *ball)
{
	double dx, dy;
	double platform_ball_x, platform_ball_y, platform_ball_radius;
	double distance, two_radiuses;
	double alpha, beta, gamma;
	double related_ball_direction;
	const double pi_2 = 2.0 * M_PI;

	platform_ball_radius = (double)Circle_Size / 2.0;
	platform_ball_x = (double)X_Pos + platform_ball_radius + platform_ball_x_offset;
	platform_ball_y = (double)AsConfig::Platform_Y_Pos + platform_ball_radius;

	dx = next_x_pos - platform_ball_x;
	dy = next_y_pos - platform_ball_y;

	distance = sqrt(dx * dx + dy * dy);
	two_radiuses = platform_ball_radius + ball->Radius;

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

	if (color_value == Highlight_Color.Get_RGB() )
		*color = &Highlight_Color;
	else if (color_value == Platform_Circle_Color.Get_RGB() )
		*color = &Platform_Circle_Color;
	else if (color_value == Platform_Inner_Color.Get_RGB() )
		*color = &Platform_Inner_Color;
	else if (color_value == AsConfig::BG_Color.Get_RGB() )
		*color = &AsConfig::BG_Color;
	else
		AsConfig::Throw();

	return true;
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Current_Platform_Width()
{
	//--- вычисляем ширину платформы для разных состояний
	if (Platform_State == EPlatform_State::Rolling && Platform_State.Rolling == EPlatform_Substate_Rolling::Roll_In)
		return (double)Circle_Size;
	else if (Platform_State == EPlatform_State::Expanding)
		return Expanding_Platform_Width;
	else
		return (double)Normal_Width;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Correct_Platform_Pos()
{ //--- проверка что край платформы не залез за край рамки слева и справа
	double 	max_platform_x = AsConfig::Max_X_Pos - Get_Current_Platform_Width() + 1;
	bool got_correction = false;

	if (X_Pos <= AsConfig::Border_X_Offset)
	{
		X_Pos = AsConfig::Border_X_Offset;
		got_correction = true;
	}

	if (X_Pos >= max_platform_x)
		X_Pos = max_platform_x;

	return got_correction;
}
//------------------------------------------------------------------------------------------------------------
//EPlatform_State AsPlatform::Set_Next_Or_Regular_State(EPlatform_Substate_Regular new_regular_state)
//{// Возврат: если не Unknown, надо перейти в это состояние
//
//	EPlatform_State next_state;
//
//	Platform_State = EPlatform_State::Regular;
//
//	// Если есть отложенное состояние, то переведем в него, иначе в Regular
//	next_state = Platform_State.Get_Next_State();
//	
//	if (next_state == EPlatform_State::Unknown)
//		Platform_State.Regular = new_regular_state;
//
//	return next_state;
//}
////------------------------------------------------------------------------------------------------------------
