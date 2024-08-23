#include "Border.h"

AsBorder::AsBorder()
: Clear_Floor(false), Floor_Rect{}
{
	Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
	Floor_Rect.right = (AsConfig::Max_X_Pos - 1) * AsConfig::Global_Scale;
	Floor_Rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;

	Gates.push_back(new AGate(1, 28, 0, 3) );
	Gates.push_back(new AGate(AsConfig::Max_X_Pos, 28, AsConfig::Level_Width - 1, 3) );

	Gates.push_back(new AGate(1, 76, 0, 9) );
	Gates.push_back(new AGate(AsConfig::Max_X_Pos, 76, AsConfig::Level_Width - 1, 9) );

	Gates.push_back(new AGate(1, 129) );
	Gates.push_back(new AGate(AsConfig::Max_X_Pos, 129) );

	Gates.push_back(new AGate(1, 178) );
	Gates.push_back(new AGate(AsConfig::Max_X_Pos, 178) );

	if (Gates.size() != AsConfig::Gates_Count)
		AsConfig::Throw();
}
//------------------------------------------------------------------------------------------------------------
AsBorder::~AsBorder()
{
	for (auto &gate : Gates)
		delete gate; // возвращаем память в кучу

	Gates.erase(Gates.begin(), Gates.end()); // очищаем вектор
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Redraw_Floor()
{
	Clear_Floor = true;
	AsTools::Invalidate_Rect(Floor_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Open_Gate(int gate_index, bool short_open)
{
	if (gate_index != Gates.size() - 1 and short_open)
		AsConfig::Throw(); // самый последний гейт может открываться только short_open, т.е. мячик может выкатываться только из правого нижнего гейта
	
	if (gate_index >= 0 and gate_index < (int)Gates.size() )
		Gates[gate_index]->Open_Gate(short_open);
	else
		AsConfig::Throw();
}
//------------------------------------------------------------------------------------------------------------
int AsBorder::Long_Open_Gate()
{
	int gate_index;
	bool got_gate = false;
	AGate *gate;

	gate_index = AsTools::Rand( (int)Gates.size() );

	for (int i = 0; i < (int)Gates.size(); i++)
	{
		gate = Gates[gate_index];

		if (gate_index != Gates.size() - 1) // Гейт, из которого выкатывается платформа не выпускает монстров!!
		{
			if (gate->Is_Closed()) // проверяем доступность гейта для выхода монстра
			{
				if (gate->Level_X_Pos == -1)
				{
					got_gate = true; // нашли свободный гейт
					break;
				}

				if (!AsLevel::Has_Brick_At(gate->Level_X_Pos, gate->Level_Y_Pos) // если кирпича у гейта нет 
					and !AsLevel::Has_Brick_At(gate->Level_X_Pos, gate->Level_Y_Pos + 1)) // и по той же позиции по х кирпича нет = такой гейт нам подходит
				{
					got_gate = true; // нашли свободный гейт
					break;
				}
			}
			++gate_index;

			if (gate_index >= Gates.size() )
				gate_index = 0;
		}
	}

	if (!got_gate)
		AsConfig::Throw(); // гейт не был найден

	Open_Gate(gate_index, false);

	return gate_index;
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{// Корректируем позицию при отражении от рамки

	bool got_hit = false;
	
	// 1. Левый край
	if (next_x_pos - AsConfig::Ball_Radius < AsConfig::Border_X_Offset)
	{
		got_hit = true;
		ball->Reflect(false);

	}

	// 2. Верхний край
	if (next_y_pos - AsConfig::Ball_Radius < AsConfig::Border_Y_Offset)
	{
		got_hit = true;
		ball->Reflect(true);

	}

	// 3. Правый край
	if (next_x_pos + AsConfig::Ball_Radius > AsConfig::Max_X_Pos + 1)
	{
		got_hit = true;
		ball->Reflect(false);
	}

	// 4. Нижний край
	if (AsConfig::Level_Has_Floor and next_y_pos + AsConfig::Ball_Radius > AsConfig::Floor_Y_Pos)
	{
		got_hit = true;
		ball->Reflect(true);

	}

	// Чтобы шарик смог улететь ниже пола, проверяем его max_y_pos ниже видимой границы
	if (next_y_pos + AsConfig::Ball_Radius > (double)AsConfig::Max_Y_Pos + AsConfig::Ball_Radius * 4.0)
		ball->Set_State(EBall_State::Lost);

	return got_hit;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Begin_Movement()
{
	// Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Finish_Movement()
{
	// Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Advance(double max_speed)
{
	// Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
double AsBorder::Get_Speed()
{
	return 0.0;  // Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Act()
{
		for (auto &gate : Gates)
			gate->Act();
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	// 1. Стираем гейты
	for (auto *gate : Gates)
		gate->Clear(hdc, paint_area);

	// 2. Стираем пол (если надо)
	if (Clear_Floor)
	{
		if (IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
			AsTools::Rect(hdc, Floor_Rect, AsConfig::BG_Color);

		Clear_Floor = false;
	}
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

	// 5. Гейты
	for (auto *gate : Gates)
		gate->Draw(hdc, paint_area);
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
	const int scale = AsConfig::Global_Scale;

	rect.left = x * scale;
	rect.top = y * scale;
	rect.right = (x + 4) * scale;
	rect.bottom = (y + 4) * scale;

	if (top_border)
	{
		for (auto &gate : Gates)
		{
			gate->Get_Y_Size(gate_top_y, gate_low_y);

			if (rect.top >= gate_top_y and rect.bottom <= gate_low_y) // элемент рамки целиком внутри гейта
				return; // Гейт целиком закроет элемент рамки
		}
	}

	if (! IntersectRect(&intersection_rect, &paint_area, &rect) )
		return;

	// Основная линия
	AsConfig::Blue_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * scale, (y + 1) * scale, (x + 4) * scale - 1, (y + 4) * scale - 1);
	else
		Rectangle(hdc, (x + 1) * scale, y * scale, (x + 4) * scale - 1, (y + 4) * scale - 1);

	// Белая кайма
	AsConfig::White_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * scale, y * scale, (x + 4) * scale - 1, (y + 1) * scale - 1);
	else
		Rectangle(hdc, x * scale, y * scale, (x + 1) * scale - 1, (y + 4) * scale - 1);

	// Перфорация
	AsConfig::BG_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, (x + 2) * scale, (y + 2) * scale, (x + 3) * scale - 1, (y + 3) * scale - 1);
	else
		Rectangle(hdc, (x + 2) * scale, (y + 1) * scale, (x + 3) * scale - 1, (y + 2) * scale - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor(HDC hdc, RECT &paint_area)
{
	int strokes_count;
	int x_pos, y_pos;
	const int scale = AsConfig::Global_Scale;


	int line_len = 4 * scale;
	int gap_len = 2 * scale;
	int stroke_len = line_len + gap_len;
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	strokes_count = (Floor_Rect.right - Floor_Rect.left + scale) / stroke_len;
	x_pos = Floor_Rect.left + 1;
	y_pos = Floor_Rect.top + (Floor_Rect.bottom - Floor_Rect.top) / 2;
	AsConfig::Letter_Color.Select(hdc);

	for (int i = 0; i < strokes_count; i++)
	{
		MoveToEx(hdc, x_pos, y_pos, 0);
		LineTo(hdc, x_pos + line_len, y_pos);

		x_pos += stroke_len;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Opened(int gates_index)
{
	if (gates_index >= 0 and gates_index < Gates.size() )
		return Gates[gates_index]->Is_Opened();
	else
	{
		AsConfig::Throw();
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Closed(int gates_index)
{
	if (gates_index >= 0 and gates_index < Gates.size() )
		return Gates[gates_index]->Is_Closed();
	else
	{
		AsConfig::Throw();
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Get_Gate_Pos(int gate_index, int &gate_x_pos, int &gate_y_pos)
{
	if (gate_index >= 0 and gate_index < Gates.size() )
		Gates[gate_index]->Get_Pos(gate_x_pos, gate_y_pos);
	else
		AsConfig::Throw();
}
//------------------------------------------------------------------------------------------------------------
