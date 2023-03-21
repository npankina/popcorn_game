#include "Border.hpp"

// AGate
const double AGate::Max_Gap_Short_Height = 9.0;
const double AGate::Max_Gap_Long_Height = 18.0;
const double AGate::Gap_Height_Short_Step = Max_Gap_Short_Height / ( (double)AsConfig::FPS / 2.0); // Для анимации за 1/2 сек.
const double AGate::Gap_Height_Long_Step = Max_Gap_Long_Height / ((double)AsConfig::FPS * 3.0); // За 3 сек.

AGate::AGate(int x, int y)
: Gate_State(EGate_State::Closed), Gate_Transformation(EGate_Transformation::Unknown), 
  X_Pos(x), Y_Pos(y), Origin_Y_Pos(y), Edge_Count(5), Gate_Close_Tick(0), Gap_Height(0.0), Gate_Rect{}
{
	const int scale = AsConfig::Global_Scale;

	Gate_Rect.left = X_Pos * scale;
	Gate_Rect.top = (int)Y_Pos * scale;
	Gate_Rect.right = Gate_Rect.left + Width * scale;
	Gate_Rect.bottom = Gate_Rect.top + Height * scale;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Act()
{
	bool correct_pos;

	switch (Gate_State)
	{
	case EGate_State::Closed:
		break;

	case EGate_State::Short_Open:
		if (Act_For_Short_Open() )
			Redraw_Gate();
		break;

	case EGate_State::Long_Open:
		 if (Act_For_Long_Open(correct_pos) )
		 {
			 if (correct_pos)
			 {
				 Y_Pos = Origin_Y_Pos - Gap_Height / 2.0;

				 Gate_Rect.top = (int)round(Y_Pos * AsConfig::D_Global_Scale);
				 Gate_Rect.bottom = (int)round((Y_Pos + (double)Height + Gap_Height) * AsConfig::D_Global_Scale);
			 }

			 Redraw_Gate();
		 }
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Gate_Rect) )
		return;

	AsConfig::Rect(hdc, Gate_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Gate_Rect) )
		return;

	Clear(hdc, paint_area);

	Draw_Cup(hdc, true);
	Draw_Cup(hdc, false);

	if (Gate_State == EGate_State::Long_Open && 
	   (Gate_Transformation == EGate_Transformation::Init || Gate_Transformation == EGate_Transformation::Finalize) )
		Draw_Discharge(hdc); // Рисование "разряда" между чашами лонг гейта
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Discharge(HDC hdc)
{
	double ratio = Gap_Height / Max_Gap_Long_Height;
	int field_y;
	int dots_count = 4;
	int dot_x, dot_y;

	if (ratio < 0.2 || ratio > 0.9)
		return; // Не рисуется "разряд" в начале и конце анимации гейтов

	field_y = (int)round(Origin_Y_Pos + (double)Height / 2.0 - Gap_Height / 2.0) + 1;

	// отрисовка точек формирующих "разряд"
	for (int i = 0; i < dots_count; i++)
	{
		dot_x = 1 + AsConfig::Rand(4); // область смещена на 1 от края
		dot_y = AsConfig::Rand( (int)Gap_Height - 1);

		AsConfig::Rect(hdc, X_Pos + dot_x, field_y + dot_y, 1, 1, AsConfig::White_Color);

	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Cup(HDC hdc, bool is_top)
{
	RECT rect{}, gate_rect{};
	HRGN region;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	const int half_size = scale / 2;
	int x = 0, y = 0;
	double cup_y_offset;
	XFORM xform{}, old_xform{};

	xform.eM11 = 1.0f;
	xform.eM12 = 0.0f;
	xform.eM21 = 0.0f;

	if (is_top)
	{
		xform.eM22 = 1.0f;
		cup_y_offset = 0.0;
	}
	else
	{
		xform.eM22 = -1.0f;
		if (Gate_State == EGate_State::Long_Open)
			cup_y_offset = (Height + Gap_Height) * d_scale - 1.0;
		else
			cup_y_offset = (double)Height * d_scale - 1.0;
	}

	xform.eDx = (float)(X_Pos * scale);
	xform.eDy = (float)round(Y_Pos * d_scale + cup_y_offset);

	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	// 1. Полукруглая часть чаши
	rect.left = x * scale;
	rect.top = (y + 1) * scale;
	rect.right = rect.left + 6 * scale;
	rect.bottom = rect.top + 4 * scale;

	// 1.1. Основа
	AsConfig::Blue_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, rect, 3);

	// 1.2. Блик слева (область обрезки)
	rect.left = X_Pos * scale;
	rect.right = rect.left + 3 * scale;

	if (is_top)
	{
		rect.top = (int)round((Y_Pos + 1.0) * d_scale);
		rect.bottom = rect.top + 4 * scale;
	}
	else
	{
		rect.top = (int)round((Y_Pos - 1.0) * d_scale + cup_y_offset + 1.0);
		rect.bottom = rect.top - 4 * scale;
	}

	region = CreateRectRgnIndirect(&rect);
	SelectClipRgn(hdc, region);

	AsConfig::Gate_Color.Select_Pen(hdc);

	rect.left = x * scale + half_size;
	rect.top = (y + 1) * scale + half_size;
	rect.right = rect.left + 5 * scale + half_size;
	rect.bottom = rect.top + 5 * scale + half_size;

	AsConfig::Round_Rect(hdc, rect, 3);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	// 1.3. Блик снизу
	AsConfig::Rect(hdc, x, y + 4, 4, 1, AsConfig::White_Color);

	// 1.4 "Заплатка" в правом нижнем углу
	AsConfig::Rect(hdc, x + 4, y + 3, 2, 2, AsConfig::Blue_Color);

	// 1.5 Перфорация
	AsConfig::Rect(hdc, x + 4, y + 3, 1, 1, AsConfig::BG_Color);

	// 1.6 Перемычка перед чашей
	AsConfig::Rect(hdc, x + 2, y, 2, 1, AsConfig::Blue_Color);

	Draw_Edges(hdc); // Рисуем перемычки

	SetWorldTransform(hdc, &old_xform);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Edges(HDC hdc)
{
	if (Gate_State == EGate_State::Long_Open)
		Draw_Long_Opening_Edges(hdc);
	else
		Draw_Short_Opening_Edges(hdc);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Long_Opening_Edges(HDC hdc)
{
	double ratio = Gap_Height / Max_Gap_Long_Height;
	bool is_long_edge = false;
	int i;

	if (ratio < 0.3)
	{
		for (i = 0; i < 4; i++)
		{
			Draw_One_Edge(hdc, 5 + i, is_long_edge);
			is_long_edge = !is_long_edge;
		}

		if (ratio > 0.1)
			Draw_Red_Edge(hdc, 9, false, false);
	}
	else if (ratio < 0.5)
	{
		Draw_One_Edge(hdc, 5, false);
		Draw_One_Edge(hdc, 6, true);
		Draw_One_Edge(hdc, 7, true);

		Draw_Red_Edge(hdc, 8, true, true);
		Draw_Red_Edge(hdc, 9, false, false);
	}
	else
	{
		for (i = 0; i < 2; i++)
			Draw_One_Edge(hdc, 5 + i, true);

		Draw_Red_Edge(hdc, 7, true, false);
		Draw_Red_Edge(hdc, 8, true, true);
		Draw_Red_Edge(hdc, 9, false, false);
	}	 
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Short_Opening_Edges(HDC hdc)
{
	bool is_long_edge = false;
	double ratio = 1.0 - Gap_Height / Max_Gap_Short_Height; // коэфф-т от 0..1
	int count = Edge_Count * ratio;

	for (int i = 0; i < count; i++)
	{
		Draw_One_Edge(hdc, 5 + i, is_long_edge);
		is_long_edge = !is_long_edge;
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_One_Edge(HDC hdc, int edge_y_offset, bool is_long)
{
	if (is_long)
	{// Длинное ребро
		AsConfig::Rect(hdc, 0, edge_y_offset, 4, 1, AsConfig::White_Color);
		AsConfig::Rect(hdc, 4, edge_y_offset, 2, 1, AsConfig::Blue_Color);
	}
	else
	{// Короткое ребро
		AsConfig::Rect(hdc, 1, edge_y_offset, 2, 1, AsConfig::Blue_Color);
		AsConfig::Rect(hdc, 4, edge_y_offset, 1, 1, AsConfig::Blue_Color);
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Red_Edge(HDC hdc, int edge_y_offset, bool is_long, bool has_highlight)
{
	if (is_long)
	{// Длинное ребро
		AsConfig::Rect(hdc, 0, edge_y_offset, 6, 1, AsConfig::Red_Color);
		if (has_highlight)
			AsConfig::Rect(hdc, 1, edge_y_offset, 1, 1, AsConfig::White_Color);
	}
	else
	{// Короткое ребро
		AsConfig::Rect(hdc, 1, edge_y_offset, 4, 1, AsConfig::Red_Color);
	}
}//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Open_Gate(bool short_open)
{
	if (Gate_State != EGate_State::Closed)
		return;

	if (short_open)
		Gate_State = EGate_State::Short_Open;
	else
		Gate_State = EGate_State::Long_Open;

	Gate_Transformation = EGate_Transformation::Init;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Opened()
{
	if (Gate_State == EGate_State::Short_Open || Gate_State == EGate_State::Long_Open)
	{
		if (Gate_Transformation == EGate_Transformation::Active)
			return true;
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Get_Y_Size(int &gate_top_y, int &gate_low_y)
{
	gate_top_y = Gate_Rect.top;
	gate_low_y = Gate_Rect.bottom;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Act_For_Short_Open()
{
	switch (Gate_Transformation)
	{
	case EGate_Transformation::Unknown:
		break;

	case EGate_Transformation::Init:
		if (Gap_Height < Max_Gap_Short_Height)
		{
			Gap_Height += Gap_Height_Short_Step;
		}
		else
		{
			Gate_Close_Tick = AsConfig::Current_Timer_Tick + Short_Opening_Timeout;
			Gate_Transformation = EGate_Transformation::Active;
		}
		break;

	case EGate_Transformation::Active:
		if (AsConfig::Current_Timer_Tick >= Gate_Close_Tick)
			Gate_Transformation = EGate_Transformation::Finalize;
		break;

	case EGate_Transformation::Finalize:
		if (Gap_Height > 0.0)
		{
			Gap_Height -= Gap_Height_Short_Step;
		}
		else
		{
			Gate_Transformation = EGate_Transformation::Unknown;
			Gate_State = EGate_State::Closed;
		}
		break;

	default:
		AsConfig::Throw();
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Act_For_Long_Open(bool &correct_pos)
{
	correct_pos = false;

	switch (Gate_Transformation)
	{
	case EGate_Transformation::Unknown:
		break;

	case EGate_Transformation::Init:
		if (Gap_Height < Max_Gap_Long_Height)
		{
			Gap_Height += Gap_Height_Long_Step;
			//Gate_Rect.top = (int)((Y_Pos - (Gap_Height / 2.0) ) * AsConfig::D_Global_Scale);
			//Gate_Rect.bottom = (int)((Y_Pos + (double)Height + (Gap_Height / 2.0) ) * AsConfig::D_Global_Scale);
			correct_pos = true;
		}
		else
		{
			Gate_Close_Tick = AsConfig::Current_Timer_Tick + Long_Opening_Timeout;
			Gate_Transformation = EGate_Transformation::Active;
		}
		break;

	case EGate_Transformation::Active:
		if (AsConfig::Current_Timer_Tick >= Gate_Close_Tick)
			Gate_Transformation = EGate_Transformation::Finalize;
		break;

	case EGate_Transformation::Finalize:
		if (Gap_Height > 0.0)
		{
			Gap_Height -= Gap_Height_Long_Step;
			//Gate_Rect.top = (int)((Y_Pos - (Gap_Height / 2.0) ) * AsConfig::D_Global_Scale);
			//Gate_Rect.bottom = (int)((Y_Pos + (double)Height + (Gap_Height / 2.0) ) * AsConfig::D_Global_Scale);
			correct_pos = true;
		}
		else
		{
			Gate_Transformation = EGate_Transformation::Unknown;
			Gate_State = EGate_State::Closed;
		}
		break;

	default:
		AsConfig::Throw();
	}

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Redraw_Gate()
{
	--Gate_Rect.top;
	++Gate_Rect.bottom;

	AsConfig::Invalidate_Rect(Gate_Rect);

	++Gate_Rect.top;
	--Gate_Rect.bottom;
}
//------------------------------------------------------------------------------------------------------------




// AsBorder
//------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
: Floor_Rect{}, Gates{} 
{
	Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
	Floor_Rect.right = (AsConfig::Max_X_Pos - 1) * AsConfig::Global_Scale;
	Floor_Rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;

	Gates[0] = new AGate(1, 29);
	Gates[1] = new AGate(AsConfig::Max_X_Pos, 29);

	Gates[2] = new AGate(1, 77);
	Gates[3] = new AGate(AsConfig::Max_X_Pos, 77);

	Gates[4] = new AGate(1, 129);
	Gates[5] = new AGate(AsConfig::Max_X_Pos, 129);

	Gates[6] = new AGate(1, 178);
	Gates[7] = new AGate(AsConfig::Max_X_Pos, 178);
}
//------------------------------------------------------------------------------------------------------------
AsBorder::~AsBorder()
{
	for(int i = 0; i < AsConfig::Gates_Number; i++)
		delete Gates[i];
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Redraw_Floor()
{
	AsConfig::Invalidate_Rect(Floor_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Open_Gate(int gate_index, bool short_open)
{
	if (gate_index < 0 || gate_index >= AsConfig::Gates_Number)
		AsConfig::Throw();

	if (gate_index != AsConfig::Gates_Number - 1 && short_open)
		AsConfig::Throw(); // самый последний гейт может открываться только short_open

	Gates[gate_index]->Open_Gate(short_open);
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{// Корректируем позицию при отражении от рамки

	bool got_hit = false;
	
	// 1. Левый край
	if (next_x_pos - ball->Radius < AsConfig::Border_X_Offset)
	{
		got_hit = true;
		ball->Reflect(false);

	}

	// 2. Верхний край
	if (next_y_pos - ball->Radius < AsConfig::Border_Y_Offset)
	{
		got_hit = true;
		ball->Reflect(true);

	}

	// 3. Правый край
	if (next_x_pos + ball->Radius > AsConfig::Max_X_Pos + 1)
	{
		got_hit = true;
		ball->Reflect(false);
	}

	// 4. Нижний край
	if (AsConfig::Level_Has_Floor && next_y_pos + ball->Radius > AsConfig::Floor_Y_Pos)
	{
		got_hit = true;
		ball->Reflect(true);

	}

	// Чтобы шарик смог улететь ниже пола, проверяем его max_y_pos ниже видимой границы
	if (next_y_pos + ball->Radius > (double)AsConfig::Max_Y_Pos + ball->Radius * 4.0)  
		ball->Set_State(EBS_Lost);

	return got_hit;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Act()
{
	for(int i = 0; i < AsConfig::Gates_Number; i++)
		Gates[i]->Act();
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	for (int i = 0; i < AsConfig::Gates_Number; i++)
		Gates[i]->Clear(hdc, paint_area);

	if (! AsConfig::Level_Has_Floor)
		return;

	if (! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	AsConfig::BG_Color.Select(hdc);

	Rectangle(hdc, Floor_Rect.left, Floor_Rect.top, Floor_Rect.right - 1, Floor_Rect.bottom - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT &paint_area)
{// Рисует рамку уровня

	int i;

	// 1. Линия слева
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 2, 1 + i * 4, false);

	// 2. Линия справа
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, AsConfig::Max_X_Pos + 1, 1 + i * 4, false);

	// 3. Линия сверху
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 3 + i * 4, 0, true);

	// 4. Пол (если есть)
	if (AsConfig::Level_Has_Floor)
		Draw_Floor(hdc, paint_area);

	// 5. Gates
	for(i = 0; i < AsConfig::Gates_Number; i++)
		Gates[i]->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Finished()
{
	return false;  // Заглушка, т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{// Рисует элемент рамки уровня

	RECT intersection_rect{}, rect{};
	int gate_top_y, gate_low_y;

	rect.left = x * AsConfig::Global_Scale;
	rect.top = y * AsConfig::Global_Scale;
	rect.right = (x + 4) * AsConfig::Global_Scale;
	rect.bottom = (y + 4) * AsConfig::Global_Scale;

	if (top_border)
	{
		for(int i = 0; i < AsConfig::Gates_Number; i++)
		{
			Gates[i]->Get_Y_Size(gate_top_y, gate_low_y);

			if (rect.top >= gate_top_y && rect.bottom <= gate_low_y) // элемент рамки целиком внутри гейта
				return; // Гейт целиком закроет элемент рамки
		}


	}

	if (! IntersectRect(&intersection_rect, &paint_area, &rect) )
		return;

	// Основная линия
	AsConfig::Blue_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);
	else
		Rectangle(hdc, (x + 1) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);

	// Белая кайма
	AsConfig::White_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 1) * AsConfig::Global_Scale - 1);
	else
		Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 1) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);

	// Перфорация
	AsConfig::BG_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 3) * AsConfig::Global_Scale - 1);
	else
		Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 2) * AsConfig::Global_Scale - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor(HDC hdc, RECT &paint_area)
{
	int i, strokes_count;
	int x_pos, y_pos;

	int line_len = 4 * AsConfig::Global_Scale;
	int gap_len = 2 * AsConfig::Global_Scale;
	int stroke_len = line_len + gap_len;
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	strokes_count = (Floor_Rect.right - Floor_Rect.left + AsConfig::Global_Scale) / stroke_len;
	x_pos = Floor_Rect.left + 1;
	y_pos = Floor_Rect.top + (Floor_Rect.bottom - Floor_Rect.top) / 2;
	AsConfig::Letter_Color.Select(hdc);

	for (i = 0; i < strokes_count; i++)
	{
		MoveToEx(hdc, x_pos, y_pos, 0);
		LineTo(hdc, x_pos + line_len, y_pos);

		x_pos += stroke_len;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Open(int gates_index)
{
	if (gates_index < 0 || gates_index >= AsConfig::Gates_Number)
		return false;
	return Gates[gates_index]->Is_Opened();
}