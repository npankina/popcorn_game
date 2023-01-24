#include "Ball_Set.h"

// AsBall_Set
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Advance(double max_speed)
{
	int i;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
		Balls[i].Advance(max_speed);
}
//------------------------------------------------------------------------------------------------------------
double AsBall_Set::Get_Speed()
{
	int i;
	double max_speed = 0.0;
	double current_speed;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		if (&Balls[i] != 0)
		{
			current_speed = Balls[i].Get_Speed();

			if (current_speed > max_speed)
				max_speed = current_speed;
		}
	}

	return max_speed;
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Begin_Movement()
{
	int i;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
		Balls[i].Begin_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Finish_Movement()
{
	int i;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
		Balls[i].Finish_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Act() {/* Заглушка! т.к. мячик сам не анимируется */}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Draw(HDC hdc, RECT& paint_area)
{
	int i;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
		Balls[i].Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Clear(HDC hdc, RECT& paint_area)
{
	int i;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
		Balls[i].Clear(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsBall_Set::Is_Finished() { return false; /* Заглушка! этот метод не используется */ }
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Release_From_Platform(double platform_x_pos)
{
	int i;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
		if (Balls[i].Get_State() == EBS_On_Platform)
			Balls[i].Set_State(EBS_Normal, platform_x_pos, AsConfig::Start_Ball_Y_Pos);
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Release_Next_Ball()
{
	int i;
	ABall *current_ball;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		current_ball = &Balls[i];
	
		if (current_ball->Get_State() == EBS_On_Platform)
		{
			current_ball->Release();
			break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Set_On_Platform(double platform_x_pos)
{
	int i;

	for (i = 0; i < 5; i++)	
		Balls[i].Set_State(EBS_On_Platform, platform_x_pos, AsConfig::Start_Ball_Y_Pos);

	for (; i < AsConfig::Max_Balls_Count; i++)	
		Balls[i].Set_State(EBS_Disabled);
}
//------------------------------------------------------------------------------------------------------------
bool AsBall_Set::All_Balls_Are_Lost()
{
	int i;
	int active_balls_counter = 0;
	int lost_balls_counter = 0;

	// Смещаем мячики
	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		if (Balls[i].Get_State() == EBS_Disabled)
			continue;

		++active_balls_counter;

		if (Balls[i].Get_State() == EBS_Lost)
		{
			++lost_balls_counter;
			continue;
		}

		//Balls[i].Move();
	}	

	if (active_balls_counter == lost_balls_counter)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Set_For_Test()
{
	Balls[0].Set_For_Test(); // В повторяющихся тестах участвует только 0-й мячик
}
//------------------------------------------------------------------------------------------------------------
bool AsBall_Set::Is_Test_Finished()
{
	return Balls[0].Is_Test_Finished(); // В повторяющихся тестах участвует только 0-й мячик
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Tripple_Balls()
{ // "Растроить" самый дальний мячик

	int i;
	double current_ball_x, current_ball_y;
	double further_ball_x, further_ball_y;
	ABall *further_ball = 0;
	ABall *current_ball = 0;
	ABall *left_ball = 0, *right_ball = 0;

	// 1. Выбираем самый дальний по Y мячик
	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		if (Balls[i].Get_State() != EBS_Normal)
			continue;

		current_ball = &Balls[i];

		if (further_ball == 0)
			further_ball = current_ball;
		else
		{
			current_ball->Get_Center(current_ball_x, current_ball_y);
			further_ball->Get_Center(further_ball_x, further_ball_y);
			if (current_ball_y < further_ball_y)
				further_ball = current_ball;
		}
	}

	// 2. Если есть мячик в нормальном состоянии, растроить его
	if (further_ball == 0)
		return;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		current_ball = &Balls[i];

		switch (current_ball->Get_State() )
		{
		case EBS_Disabled:
		case EBS_Lost:
			if (left_ball == 0)
				left_ball = current_ball;
			else
			{
				if (right_ball == 0)
				{
					right_ball = current_ball;
					break; // Оба мяча найдены
				}
			}
		}
	}

	// 3. Разводим боковые мячи в стороны
	if (left_ball != 0)
	{
		*left_ball = *further_ball;
		left_ball->Set_Direction(left_ball->Get_Direction() + M_PI / 8.0);
	}

	if (right_ball != 0)
	{
		*right_ball = *further_ball;
		right_ball->Set_Direction(right_ball->Get_Direction() - M_PI / 8.0);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Inverse()
{
	int i;
	ABall *current_ball = 0;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		current_ball = &Balls[i];

		if (current_ball->Get_State() == EBS_Normal)
			current_ball->Set_Direction( current_ball->Get_Direction() + M_PI );
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Accelerate()
{
	int i;
	ABall *current_ball = 0;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		current_ball = &Balls[i];

		if (current_ball->Get_State() == EBS_Normal and current_ball->Get_Speed() <= 6.5)
			current_ball->Set_Speed( current_ball->Get_Speed() * AsConfig::Ball_Acseleration);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Reset_Speed()
{
	int i;
	ABall *current_ball = 0;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		current_ball = &Balls[i];

		if (current_ball->Get_State() == EBS_Normal)
			current_ball->Set_Speed(AsConfig::Normal_Ball_Speed);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Advance_On_Platform(double direction, double max_speed)
{
	int i;
	ABall *current_ball = 0;

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
	{
		current_ball = &Balls[i];

		if (current_ball->Get_State() == EBS_On_Platform)
			current_ball->Forced_Advance(direction, max_speed);
	}
}
//------------------------------------------------------------------------------------------------------------
