#include "Platform.h"

// AsPlatform_State
//------------------------------------------------------------------------------------------------------------
AsPlatform_State::AsPlatform_State()
: Current_State(EPlatform_State::Regular), Regular(EPlatform_Substate_Regular::Missing), 
  Glue(EPlatform_Substate_Glue::Unknown), Meltdown(EPlatform_Substate_Meltdown::Unknown), 
  Rolling(EPlatform_Substate_Rolling::Unknown), Expanding(EPlatform_Substate_Expanding::Unknown), Moving(EPlatform_Moving_State::Stop)
{}
//------------------------------------------------------------------------------------------------------------
AsPlatform_State::operator EPlatform_State() const
{
	return Current_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_State::operator =(EPlatform_State new_state)
{
	Current_State = new_state;
}
//------------------------------------------------------------------------------------------------------------




// AsPlatform
//------------------------------------------------------------------------------------------------------------

const double AsPlatform::Max_Glue_Spot_Height_Ratio = 1.0;
const double AsPlatform::Min_Glue_Spot_Height_Ratio = 0.4;
const double AsPlatform::Glue_Spot_Height_Ratio_Step = 0.03;

const double AsPlatform::Min_Expanding_Platform_Width = (double)Normal_Width;
const double AsPlatform::Max_Expanding_Platform_Width = 40.0;
const double AsPlatform::Expanding_Platform_Width_Step = 1.0;
//------------------------------------------------------------------------------------------------------------
AsPlatform::~AsPlatform()
{
	delete [] Normal_Platform_Image;
}
//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
: X_Pos(AsConfig::Border_X_Offset), Inner_Width(Normal_Platform_Inner_Width),Rolling_Step(0), Speed(0.0), 
  Glue_Spot_Height_Ratio(0.0), Expanding_Platform_Width(0.0), Normal_Platform_Image_Width(0), Normal_Platform_Image_Height(0), 
  Normal_Platform_Image(0), Width(Normal_Width), Platform_Rect{}, Prev_Platform_Rect{}, Highlight_Color(255, 255, 255), 
  Platform_Circle_Color(230, 25, 229), Platform_Inner_Color(0, 255, 255), Ball_Set(0), Left_Key_Down(false), Right_Key_Down(false)
{}
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
	inner_right_x = (double)(X_Pos + Width - (Circle_Size - 1) );


	// 1. Проверяем отражение от боковых шариков
	if (Reflect_On_Circle(next_x_pos, next_y_pos, 0.0, ball) )
		goto _on_hit;  // От левого

	if (Reflect_On_Circle(next_x_pos, next_y_pos, Width - Circle_Size, ball) )
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
	if (ball->Get_State() == EBS_On_Parashute)
		ball->Set_State(EBS_Off_Parashute);

	if (Platform_State == EPlatform_State::Glue and Platform_State.Glue == EPlatform_Substate_Glue::Active)
	{
		ball->Get_Center(ball_x, ball_y);
		ball->Set_State(EBS_On_Platform, ball_x, ball_y);
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Advance(double max_speed) // смещение платформы по нажатию клавиш
{
	double next_step, max_platform_x;

	if (Platform_State.Moving == EPlatform_Moving_State::Stopping or Platform_State.Moving == EPlatform_Moving_State::Stop)
		return;

	max_platform_x = AsConfig::Max_X_Pos - Width + 1;
	next_step = Speed / max_speed * AsConfig::Moving_Step_Size;

	X_Pos += next_step;

	if (X_Pos <= AsConfig::Border_X_Offset)
	{
		X_Pos = AsConfig::Border_X_Offset;
		Speed = 0.0;
		Platform_State.Moving = EPlatform_Moving_State::Stopping;
	}

	if (X_Pos >= max_platform_x)
	{
		X_Pos = max_platform_x;
		Speed = 0.0;
		Platform_State.Moving = EPlatform_Moving_State::Stopping;
	}

	// Смещаем приклеенные мячики
	if ( (Platform_State == EPlatform_State::Regular and Platform_State.Regular == EPlatform_Substate_Regular::Ready) or 
		 (Platform_State == EPlatform_State::Glue and Platform_State.Glue == EPlatform_Substate_Glue::Active) )
	{
		if (Platform_State.Moving == EPlatform_Moving_State::Moving_Left)
			Ball_Set->Advance_On_Platform(M_PI, fabs(Speed), max_speed);
		else 
			if (Platform_State.Moving == EPlatform_Moving_State::Moving_Right)
				Ball_Set->Advance_On_Platform(0.0, fabs(Speed), max_speed);
	}
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Begin_Movement()
{}
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
void AsPlatform::Act()
{
	switch (Platform_State)
	{
	case EPlatform_State::Meltdown:
		Act_For_Meltdown_State();
		break;

	case EPlatform_State::Rolling:
		Act_For_Rolling_State();
		break;

	case EPlatform_State::Glue:
		Act_For_Glue_State();
		break;
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
		Draw_Glue_State(hdc, paint_area);
		break;

	case EPlatform_State::Expanding:
		Draw_Expanding_State(hdc, paint_area);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Clear(HDC hdc, RECT& paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Platform_Rect) )
		return;

	switch (Platform_State)
	{
	case EPlatform_State::Regular:
		if (Platform_State.Regular == EPlatform_Substate_Regular::Missing)
			break;
		// else - no break

	case EPlatform_State::Rolling:
	case EPlatform_State::Glue:
		AsConfig::BG_Color.Select(hdc);
		Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Is_Finished() 
{ 
	return false; /* Заглушка! метод не используется */ 
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
		AsConfig::Throw(); // состояние EPlatform_State::Regular неявно устанавливается Set_State(EPlatform_Substate_Regular new_state)
		break;

	case EPlatform_State::Meltdown:
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
		if (Platform_State.Glue == EPlatform_Substate_Glue::Finalize)
			return;
		else
		{
			Platform_State.Glue = EPlatform_Substate_Glue::Init;
			Glue_Spot_Height_Ratio = Min_Glue_Spot_Height_Ratio;
		}
		break;

	case EPlatform_State::Expanding:
		if (Platform_State.Expanding == EPlatform_Substate_Expanding::Finalize)
			return;
		else
		{
			Platform_State.Expanding = EPlatform_Substate_Expanding::Init;
			Expanding_Platform_Width = Max_Expanding_Platform_Width;
		}
		break;
	}

	Platform_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_Substate_Regular new_state)
{
	if (Platform_State == EPlatform_State::Regular and Platform_State.Regular == new_state)
		return;
	
	if (new_state == EPlatform_Substate_Regular::Normal)
	{
		if (Platform_State == EPlatform_State::Glue)
		{
			Platform_State.Glue = EPlatform_Substate_Glue::Finalize;
			while ( Ball_Set->Release_Next_Ball() ) {};
			return;
		}
	}
	
	Platform_State = EPlatform_State::Regular;
	Platform_State.Regular = new_state;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Has_State(EPlatform_Substate_Regular new_state)
{
	if (Platform_State != EPlatform_State::Regular)
		return false;

	if (Platform_State.Regular == new_state)
		return true;
	
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Redraw_Platform(bool update_rect)
{
	int platform_width;

	if (update_rect)
	{
		Prev_Platform_Rect = Platform_Rect;

		if (Platform_State == EPlatform_State::Rolling and Platform_State.Rolling == EPlatform_Substate_Rolling::Roll_In)
			platform_width = Circle_Size;
		else
			platform_width = Width;

		Platform_Rect.left = (int)(X_Pos * AsConfig::D_Global_Scale);
		Platform_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
		Platform_Rect.right = Platform_Rect.left + platform_width * AsConfig::Global_Scale;
		Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;

		if (Platform_State == EPlatform_State::Meltdown)
			Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;
	}

	AsConfig::Invalidate_Rect(Prev_Platform_Rect);
	AsConfig::Invalidate_Rect(Platform_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Move(bool to_left, bool is_key_down)
{
	if ( ! (Has_State(EPlatform_Substate_Regular::Normal) or Platform_State == EPlatform_State::Glue) )
		return;

	if (to_left)
		Left_Key_Down = is_key_down;
	else
		Right_Key_Down = is_key_down;
	
	if (Left_Key_Down and Right_Key_Down)
		return; // Игнорируем одновременное нажатие двух клавиш
	
	if ( !Left_Key_Down and !Right_Key_Down)
	{
		Platform_State.Moving = EPlatform_Moving_State::Stop;
		Speed = 0.0;
		return;
	}

	if(Left_Key_Down)
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
void AsPlatform::On_Space_Key(bool is_key_down)
{
	if (! is_key_down)
		return;

	if (Has_State(EPlatform_Substate_Regular::Ready) )
	{
		Ball_Set->Release_From_Platform(Get_Middle_Pos() );
		Set_State(EPlatform_Substate_Regular::Normal);
	}
	else
		if (Platform_State == EPlatform_State::Glue)
			Ball_Set->Release_Next_Ball();
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
	return X_Pos + (double)Width / 2.0;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Circle_Highlight(HDC hdc, int x, int y)
{// Рисуем блик на шарике

	Highlight_Color.Select_Pen(hdc);

	Arc(hdc, x + AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + (Circle_Size - 1) * AsConfig::Global_Scale - 1, y + (Circle_Size - 1) * AsConfig::Global_Scale  - 1,
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
	rect.right = (int)((x + Circle_Size) * d_scale - 1.0);
	rect.bottom = (y + Circle_Size) * scale - 1;

	Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

	rect.left = (int)((x + Inner_Width) * d_scale);
	rect.right = (int)((x + Circle_Size + Inner_Width) * d_scale - 1.0);

	Ellipse(hdc, rect.left,rect.top, rect.right, rect.bottom);

	// 2. Рисуем блик
	Draw_Circle_Highlight(hdc, (int)(x * d_scale), y * scale);

	// 3. Рисуем среднюю часть
	Platform_Inner_Color.Select(hdc);

	inner_rect.left = (int)( (x + 4) * d_scale);
	inner_rect.top = (y + 1) * scale;
	inner_rect.right = (int)( (x + 4 + Inner_Width - 1) * d_scale);
	inner_rect.bottom = (y + 1 + 5) * scale;

	AsConfig::Round_Rect(hdc, inner_rect, 3);

	if (Normal_Platform_Image == 0 and Has_State(EPlatform_Substate_Regular::Ready) )
		Get_Normal_Platform_Image(hdc);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Get_Normal_Platform_Image(HDC hdc)
{
	int offset = 0;
	int i, j;
	int x;
	int y = AsConfig::Platform_Y_Pos;
	

	x = (int)(X_Pos * AsConfig::D_Global_Scale);
	y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;

	Normal_Platform_Image_Width = Width * AsConfig::Global_Scale;
	Normal_Platform_Image_Height = Height * AsConfig::Global_Scale;

	Normal_Platform_Image = new int[Normal_Platform_Image_Width * Normal_Platform_Image_Height];

	for (i = 0; i < Normal_Platform_Image_Height; i++)
		for (j = 0; j < Normal_Platform_Image_Width; j++)
			Normal_Platform_Image[offset++] = GetPixel(hdc, (int)x + j, y + i);
	
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Meltdown_State(HDC hdc, RECT &paint_area)
{// Рисуем платформу в расплавленном состоянии

	int i, j;
	int x, y;
	int stroke_len;
	int y_offset;
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

		y = Meltdown_Platform_Y_Pos[i];
		j = 0;

		MoveToEx(hdc, x, y, 0);

		// Рисуем последовательность вертикальных штрихов разного цвета (по прообразу из Normal_Platform_Image)
		while( Get_Platform_Image_Stroke_Color(i, j, &color, stroke_len) )
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
		Set_State(EPlatform_Substate_Regular::Missing); // Вся платформа сдвинулась за пределы окна
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Rolling_State(HDC hdc, RECT &paint_area)
{
	switch(Platform_State.Rolling)
	{
	case EPlatform_Substate_Rolling::Roll_In:
		Draw_Roll_In_State(hdc, paint_area);
		break;

	case EPlatform_Substate_Rolling::Expand_Roll_In:
		Draw_Normal_State(hdc, paint_area);
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
void AsPlatform::Draw_Glue_State(HDC hdc, RECT &paint_area)
{// Рисуем растекающийся клей на платформе

	HRGN region;
	RECT glue_rect{};
	const int scale = AsConfig::Global_Scale;

	Draw_Normal_State(hdc, paint_area);

	glue_rect.left = (int)(X_Pos + 5) * scale;
	glue_rect.top = (AsConfig::Platform_Y_Pos + 1) * scale;
	glue_rect.right = glue_rect.left + Normal_Platform_Inner_Width * scale;
	glue_rect.bottom = glue_rect.top + (Height - 2) * scale;


	region = CreateRectRgnIndirect(&glue_rect);
	SelectClipRgn(hdc, region);

	AsConfig::BG_Color.Select(hdc);
	Draw_Glue_Spot(hdc, 9, 5);
	Draw_Glue_Spot(hdc, 6, 5, 6);
	Draw_Glue_Spot(hdc, 9, 6, 9);

	AsConfig::White_Color.Select(hdc);
	Draw_Glue_Spot(hdc, 9, 4);
	Draw_Glue_Spot(hdc, 6, 4, 6);
	Draw_Glue_Spot(hdc, 9, 5, 9);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Glue_Spot(HDC hdc, int width, int height, int x_offset)
{ // рисуем пятно клея

	RECT spot_rect{};
	const int scale = AsConfig::Global_Scale;
	int platform_top = (AsConfig::Platform_Y_Pos + 1) * scale;
	int spot_height = (int)( (double)height * AsConfig::D_Global_Scale * Glue_Spot_Height_Ratio );

	spot_rect.left = ( (int)X_Pos + 5 + x_offset) * scale;
	spot_rect.top = platform_top - spot_height;
	spot_rect.right = spot_rect.left + width * scale;
	spot_rect.bottom = platform_top + spot_height - scale;

	// рисуем эллипс как пятно клея
	// 4 координаты - ограничивающий прямоугольник
	Chord(hdc, spot_rect.left, spot_rect.top, spot_rect.right - 1, spot_rect.bottom - 1,
		spot_rect.left, platform_top - 1, spot_rect.right - 1, platform_top - 1); // по 2 координаты - хорды
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Expanding_State(HDC hdc, RECT &paint_area)
{
	double x = X_Pos;
	int y = AsConfig::Platform_Y_Pos;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	RECT inner_rect{}, rect{};

	// 1. Рисуем боковые шарики
	Platform_Circle_Color.Select(hdc);

	rect.left = (int)(x * d_scale);
	rect.top = y * scale;
	rect.right = (int)((x + Circle_Size) * d_scale - 1.0);
	rect.bottom = (y + Circle_Size) * scale - 1;

	Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

	rect.left = (int)((x + Expanding_Platform_Width - (double)Circle_Size) * d_scale);
	rect.right = rect.left + Circle_Size * scale;

	Ellipse(hdc, rect.left,rect.top, rect.right, rect.bottom);

	// 2. Рисуем блик
	Draw_Circle_Highlight(hdc, (int)(x * d_scale), y * scale);

	// 3. Рисуем среднюю часть
	Platform_Inner_Color.Select(hdc);

	inner_rect.left = (int)( (x + (Expanding_Platform_Width - (double)Expanding_Platform_Inner_Width) / 2.0) * d_scale);
	inner_rect.top = (y + 1) * scale;
	inner_rect.right = inner_rect.left + Expanding_Platform_Inner_Width * scale;
	inner_rect.bottom = (y + 1 + 5) * scale;

	Rectangle(hdc, inner_rect.left, inner_rect.top, inner_rect.right, inner_rect.bottom);

	// 1. Шарики по бокам

	// 2. Центральная часть

	// 3. Фермы

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
	switch(Platform_State.Rolling)
	{
	case EPlatform_Substate_Rolling::Roll_In:
		++Rolling_Step;

		if (Rolling_Step >= Max_Rolling_Step)
			Rolling_Step -= Max_Rolling_Step;

		X_Pos -= Rolling_Platform_Speed;

		if (X_Pos <= Roll_In_Platform_End_X_Pos)
		{
			X_Pos += Rolling_Platform_Speed;
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
void AsPlatform::Act_For_Glue_State()
{
	switch (Platform_State.Glue)
	{
	case EPlatform_Substate_Glue::Init:
		if (Glue_Spot_Height_Ratio < Max_Glue_Spot_Height_Ratio)
			Glue_Spot_Height_Ratio += Glue_Spot_Height_Ratio_Step;
		else
			Platform_State.Glue = EPlatform_Substate_Glue::Active;
		Redraw_Platform(false);
		break;

	case EPlatform_Substate_Glue::Finalize:
		if (Glue_Spot_Height_Ratio > Min_Glue_Spot_Height_Ratio)
			Glue_Spot_Height_Ratio -= Glue_Spot_Height_Ratio_Step;
		else
		{
			Set_State(EPlatform_Substate_Regular::Normal);
			Platform_State.Glue = EPlatform_Substate_Glue::Unknown;
		}
		Redraw_Platform(false);
		break;
	}
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

	//if (fabs(distance - two_radiuses) < AsConfig::Moving_Step_Size)
	if (distance + AsConfig::Moving_Step_Size < two_radiuses)
	{// Мячик коснулся бокового шарика

		beta = atan2(-dy, dx);

		related_ball_direction = ball->Get_Direction();
		related_ball_direction -= beta;

		if (related_ball_direction > pi_2)
			related_ball_direction -= pi_2;

		if (related_ball_direction < 0.0)
			related_ball_direction += pi_2;


		if (related_ball_direction > M_PI_2 and related_ball_direction < M_PI + M_PI_2)
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
{// Вычисляет длинну очередного вертикального штриха

	int i;
	int offset = y * Normal_Platform_Image_Width + x; // Позиция в массиве Normal_Platform_Image, соответствующая (x, y)
	int color_value = 0;
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
			if (color_value ==  Normal_Platform_Image[offset])
				++stroke_len;
			else
				break;
		}

		offset += Normal_Platform_Image_Width; // Переход на строку ниже
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