﻿#include "Level.h"

// AsLevel
//------------------------------------------------------------------------------------------------------------
char AsLevel::Level_01[AsConfig::Level_Height][AsConfig::Level_Width] =
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

char AsLevel::Test_Level[AsConfig::Level_Height][AsConfig::Level_Width] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
//------------------------------------------------------------------------------------------------------------




// AsLevel
//------------------------------------------------------------------------------------------------------------
AsLevel::AsLevel()
: Level_Rect{}, 
  Active_Bricks_Counter(0), Falling_Letters_Counter(0)
{}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{// Корректируем позицию при отражении от кирпичей

	int i, j;
	double direction;
	double min_ball_x, max_ball_x;
	double min_ball_y, max_ball_y;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;
	bool got_horizontal_hit, got_vertical_hit;
	double horizontal_reflection_pos, vertical_reflection_pos;

	if (next_y_pos + ball->Radius > AsConfig::Level_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
		return false;

	direction = ball->Get_Direction();

	min_ball_x = next_x_pos - ball->Radius;
	max_ball_x = next_x_pos + ball->Radius;
	min_ball_y = next_y_pos - ball->Radius;
	max_ball_y = next_y_pos + ball->Radius;

	min_level_x = (int)( (min_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	max_level_x = (int)( (max_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	min_level_y = (int)( (min_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);
	max_level_y = (int)( (max_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);

	for (i = max_level_y; i >= min_level_y; i--)
	{
		Current_Brick_Top_Y = AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height;
		Current_Brick_Low_Y = Current_Brick_Top_Y + AsConfig::Brick_Height;

		for (j = min_level_x; j <= max_level_x; j++)
		{
			if (Current_Level[i][j] == 0)
				continue;

			Current_Brick_Left_X = AsConfig::Level_X_Offset + j * AsConfig::Cell_Width;
			Current_Brick_Right_X = Current_Brick_Left_X + AsConfig::Brick_Width;

			got_horizontal_hit = Check_Horizontal_Hit(next_x_pos, next_y_pos, j, i, ball, horizontal_reflection_pos);

			got_vertical_hit = Check_Vertical_Hit(next_x_pos, next_y_pos, j, i, ball, vertical_reflection_pos);

			if (got_horizontal_hit and got_vertical_hit)
			{
				if (vertical_reflection_pos < horizontal_reflection_pos)
					ball->Reflect(true);
				else
					ball->Reflect(false);

				On_Hit(j, i);
				return true;
			}
			else
			{
				if (got_horizontal_hit)
				{
					ball->Reflect(false);
					On_Hit(j, i);
					return true;
				}
				else
				{
					if (got_vertical_hit)
					{
						ball->Reflect(true);
						On_Hit(j, i);
						return true;
					}
				}
			}
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Init()
{
	Level_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Level_Rect.top = AsConfig::Level_Y_Offset * AsConfig::Global_Scale;
	Level_Rect.right = Level_Rect.left + AsConfig::Cell_Width * AsConfig::Level_Width * AsConfig::Global_Scale;
	Level_Rect.bottom = Level_Rect.top + AsConfig::Cell_Height * AsConfig::Level_Height * AsConfig::Global_Scale;

	memset(Current_Level, 0, sizeof(Current_Level) );
	memset(Active_Bricks, 0, sizeof(Active_Bricks) );
	memset(Falling_Letters, 0, sizeof(Falling_Letters) );
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width])
{
	memcpy(Current_Level, level, sizeof(Current_Level) );
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act()
{
	Act_Objects((AGraphics_Object**)&Active_Bricks, AsConfig::Max_Active_Bricks_Count);
	Act_Objects((AGraphics_Object**)&Falling_Letters, AsConfig::Max_Falling_Letters_Count);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw(HDC hdc, RECT &paint_area)
{// Вывод всех кирпичей уровня

	int i, j;
	RECT intersection_rect{};
	RECT brick_rect{};

	if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect) )
	{
		for (i = 0; i < AsConfig::Level_Height; i++)
		{
			for (j = 0; j < AsConfig::Level_Width; j++)
			{
				brick_rect.left = (AsConfig::Level_X_Offset + j * AsConfig::Cell_Width) * AsConfig::Global_Scale;
				brick_rect.top = (AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height) * AsConfig::Global_Scale;
				brick_rect.right = brick_rect.left + AsConfig::Brick_Width  * AsConfig::Global_Scale;
				brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height  * AsConfig::Global_Scale;

				if (IntersectRect(&intersection_rect, &paint_area, &brick_rect) )	
					Draw_Brick(hdc, brick_rect, (EBrick_Type)Current_Level[i][j]);
			}
		}

		Draw_Objects(hdc, paint_area, (AGraphics_Object **)&Active_Bricks, AsConfig::Max_Active_Bricks_Count);
	}

	Draw_Objects(hdc, paint_area, (AGraphics_Object **)&Falling_Letters, AsConfig::Max_Falling_Letters_Count);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Get_Next_Falling_letter(int &index, AFalling_Letter **falling_letter)
{
	AFalling_Letter *current_letter;

	if (Falling_Letters_Counter < 1)
		return false;

	if (index < 0 or index >= AsConfig::Max_Falling_Letters_Count)
		return false;

	while (index < AsConfig::Max_Falling_Letters_Count)
	{
		current_letter = Falling_Letters[index++];

		if (current_letter != 0)
		{
			*falling_letter = current_letter;
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();


	if (ball->Is_Moving_Up() )
	{// Проверяем попадание в нижнюю грань

		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Low_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, ball->Radius, reflection_pos) )
		{// Проверяем возможность отражения вниз

			if (level_y < AsConfig::Level_Height - 1 and Current_Level[level_y + 1][level_x] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{// Проверяем попадание в верхнюю грань

		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Top_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, ball->Radius, reflection_pos) )
		{// Проверяем возможность отражения вверх

			if (level_y > 0 and Current_Level[level_y - 1][level_x] == 0)
				return true;
			else
				return false;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	if (! ball->Is_Moving_Left() )
	{// Проверяем попадание в левую грань

		if (Hit_Circle_On_Line(Current_Brick_Left_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, ball->Radius, reflection_pos) )
		{// Проверяем возможность отскока влево

			if (level_x > 0 and Current_Level[level_y][level_x - 1] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{// Проверяем попадание в правую грань

		if (Hit_Circle_On_Line(Current_Brick_Right_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, ball->Radius, reflection_pos) )
		{// Проверяем возможность отскока вправо

			if (level_x < AsConfig::Level_Width - 1 and Current_Level[level_y][level_x + 1] == 0)
				return true;
			else
				return false;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Brick(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{// Вывод "кирпича"

	HPEN pen;
	HBRUSH brush;

	switch (brick_type)
	{
	case EBT_None:
		return;

	case EBT_Red:
		pen = AsConfig::Brick_Red_Pen;
		brush = AsConfig::Brick_Red_Brush;
		break;

	case EBT_Blue:
		pen = AsConfig::Brick_Blue_Pen;
		brush = AsConfig::Brick_Blue_Brush;
		break;

	default:
		return;
	}

	SelectObject(hdc, pen);
	SelectObject(hdc, brush);

	RoundRect(hdc, brick_rect.left, brick_rect.top, brick_rect.right - 1, brick_rect.bottom - 1, 2 * AsConfig::Global_Scale, 2 * AsConfig::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::On_Hit(int brick_x, int brick_y)
{
	EBrick_Type brick_type;

	brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];

	if (Add_Falling_Letter(brick_x, brick_y, brick_type))
		Current_Level[brick_y][brick_x] = EBT_None;
	else
		Add_Active_Brick(brick_x, brick_y, brick_type);
	
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Add_Falling_Letter(int brick_x, int brick_y, EBrick_Type brick_type)
{// Создает падающую букву, если возможно

	int i;
	int letter_x, letter_y;
	ELetter_Type letter_type;
	AFalling_Letter *falling_letter;

	if (brick_type == EBT_Blue or brick_type == EBT_Red)
	{
		if (AsConfig::Rand(AsConfig::Hits_Per_Letter) == 0)
		{
			if (Falling_Letters_Counter < AsConfig::Max_Falling_Letters_Count)
			{
				letter_type = ELT_O;
				for (i = 0; i < AsConfig::Max_Falling_Letters_Count; i++)
				{
					if (Falling_Letters[i] == 0)
					{
						letter_x = (brick_x * AsConfig::Cell_Width + AsConfig::Level_X_Offset) * AsConfig::Global_Scale;
						letter_y = (brick_y * AsConfig::Cell_Height + AsConfig::Level_Y_Offset) * AsConfig::Global_Scale;

						falling_letter = new AFalling_Letter(brick_type, letter_type, letter_x, letter_y);
						Falling_Letters[i] = falling_letter;
						++Falling_Letters_Counter;
						break;
					}
				}
				return true;
			}
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Add_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type)
{// Создает активный кирпич, если возможно

	int i;
	AActive_Brick *active_brick;

	if (Active_Bricks_Counter >= AsConfig::Max_Active_Bricks_Count)
		return; // когда активных кирпичей слишком много!

	switch (brick_type)
	{
	case EBT_None:
		return;

	case EBT_Red:
	case EBT_Blue:
		active_brick = new AActive_Brick(brick_type, brick_x, brick_y);
		Current_Level[brick_y][brick_x] = EBT_None;
		break;

	default:
		return;
	}

	// Добавляем новый активный кирпич на первое свободное место
	for (i = 0; i < AsConfig::Max_Active_Bricks_Count; i++)
	{
		if (Active_Bricks[i] == 0)
		{
			Active_Bricks[i] = active_brick;
			++Active_Bricks_Counter;
			break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Objects(HDC hdc, RECT &paint_area, AGraphics_Object **objects_array, int objects_max_count)
{
	int i;

	for (i = 0; i < objects_max_count; i++)
	{
		if (objects_array[i] != 0)
			objects_array[i]->Draw(hdc, paint_area);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act_Objects(AGraphics_Object **objects_array, int objects_max_count)
{
	int i;

	for (i = 0; i < objects_max_count; i++)
	{
		if (objects_array[i] != 0)
		{
			objects_array[i]->Act();

			if(objects_array[i]->Is_Finished() )
			{
				delete objects_array[i];
				--Falling_Letters_Counter;
				objects_array[i] = 0;
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------------
