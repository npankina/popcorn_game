#include "Engine.h"

// AsEngine
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
: Game_State(EGS_Lost_Ball) // EGS_Test_Ball EGS_Play_Level , EGS_Play_Level
{}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Init_Engine(HWND hwnd)
{// Настройка игры при старте

	SYSTEMTIME sys_time;
	FILETIME file_time;

	GetSystemTime(&sys_time);
	SystemTimeToFileTime(&sys_time, &file_time);
	srand(file_time.dwLowDateTime);

	AsConfig::Hwnd = hwnd;

	AActive_Brick_Red_Blue::Setup_Colors();

	Level.Init();

	AFalling_Letter::Init();

	ABall::Add_Hit_Checker(&Border);
	ABall::Add_Hit_Checker(&Level);
	ABall::Add_Hit_Checker(&Platform);

	//Level.Set_Current_Level(AsLevel::Test_Level);
	Level.Set_Current_Level(AsLevel::Level_02);


	//Ball.Set_State(EBS_Normal, (double)(Platform.X_Pos + Platform.Width / 2));
	//Platform.Set_State(EPS_Normal);

	Platform.Redraw_Platform();

	SetTimer(AsConfig::Hwnd, Timer_ID, 1000 / AsConfig::FPS, 0);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT &paint_area)
{// Отрисовка экрана игры

	int i;

	SetGraphicsMode(hdc, GM_ADVANCED);

	Level.Draw(hdc, paint_area);
	Border.Draw(hdc, paint_area);
	Platform.Draw(hdc, paint_area);

	for (i = 0; i < AsConfig::Max_Balls_Count; i++)
		Balls[i].Draw(hdc, paint_area);

	//for (i = 0; i < 84 * 21 * 100; i++)
	//{
		//pixel = GetPixel(hdc, 100, 100);
		//SetPixel(hdc, 100, 100, pixel);
	//}

	/*for (i = 0; i < 84 * 1000; i++)
	{
		MoveToEx(hdc, 100, 555, 0);

		SelectObject(hdc, AsConfig::BG_Pen);
		LineTo(hdc, 100, 557);*/

		/*SelectObject(hdc, AsConfig::Brick_Blue_Pen);
		LineTo(hdc, 100, 572);


		SelectObject(hdc, AsConfig::BG_Pen);
		LineTo(hdc, 100, 575);*/
	//}

	//int yy = 0;
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Key(EKey_Type key_type, bool is_key_down)
{
	int i;

	switch (key_type)
	{
	case EKT_Left:
		Platform.Move(true, is_key_down);
		break;


	case EKT_Right:
		Platform.Move(false, is_key_down);
		break;


	case EKT_Space:
		if (is_key_down)
			if (Platform.Get_State() == EPS_Ready)
			{
				for (i = 0; i < AsConfig::Max_Balls_Count; i++)
					if (Balls[i].Get_State() == EBS_On_Platform)
						Balls[i].Set_State(EBS_Normal, Platform.Get_Middle_Pos(), AsConfig::Start_Ball_Y_Pos);
			
				Platform.Set_State(EPS_Normal);
			}
		break;
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Timer()
{
	++AsConfig::Current_Timer_Tick;

	switch (Game_State)
	{
	case EGS_Test_Ball:

		Balls[0].Set_For_Test(); // для тестов предназначен только один мячик
		Game_State = EGS_Play_Level;
		break;


	case EGS_Play_Level:
		Play_Level();
		break;


	case EGS_Lost_Ball:
		if (Platform.Get_State() == EPS_Missing)
		{
			Game_State = EGS_Restart_Level;
			Platform.Set_State(EPS_Roll_In);
		}
		break;


	case EGS_Restart_Level:
		if (Platform.Get_State() == EPS_Ready)
			Restart_Level();
		break;
	}

	Act();

	return 0;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Act()
{
	int index = 0;
	AFalling_Letter *falling_letter;

	Platform.Act();
	Level.Act();

	while (Level.Get_Next_Falling_letter(index, &falling_letter) )
	{
		if (Platform.Hit_By(falling_letter) )
			On_Falling_letter(falling_letter);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Restart_Level()
{
	int i;

	Game_State = EGS_Play_Level;

	for (i = 0; i < 3; i++)
		Balls[i].Set_State(EBS_On_Platform, Platform.Get_Middle_Pos(), AsConfig::Start_Ball_Y_Pos);

	for (; i < AsConfig::Max_Balls_Count; i++)	
		Balls[i].Set_State(EBS_Disabled);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Play_Level()
{
	int i;
	int active_balls_counter = 0;
	int lost_balls_counter = 0;
	double rest_distance;
	double max_speed;

	// 1. Смещаем платформу
	max_speed = fabs(Platform.Speed);

	rest_distance = max_speed;

	while (rest_distance > 0.0)
	{
		Platform.Advance(max_speed);
		rest_distance -= AsConfig::Moving_Step_Size;
	}

	Platform.Redraw_Platform();

	// 2. Смещаем мячики
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

		Balls[i].Move();
	}	

	if (active_balls_counter == lost_balls_counter)
	{// все мячи потеряны
		Game_State = EGS_Lost_Ball;
		Level.Stop();
		Platform.Set_State(EPS_Meltdown);
	}

	if (active_balls_counter == 1)
		if (Balls[0].Is_Test_Finished() )
			Game_State = EGS_Test_Ball;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::On_Falling_letter(AFalling_Letter *falling_letter)
{
	switch(falling_letter->Letter_Type)
	{
	//case ELT_O: // "Отмена"
		//break;

	//case ELT_I: // "Инверсия"
		//break;

	//case ELT_C: // "Скорость"
		//break;

	//case ELT_M: // "Монстры"
		//break;

	//case ELT_G: // "Жизнь"
		//break;

	//case ELT_K: // "Клей"
		//break;

	//case ELT_W: // "Шире"
		//break;

	//case ELT_P: // "Пол"
		//break;

	//case ELT_L: // "Лазер"
		//break;

	case ELT_T: // "Три"
		break;

	//case ELT_Plus: // "Переход на след. уровень"
		//break;

	default:
		AsConfig::Throw();
	}

	falling_letter->Finalize();

}
//------------------------------------------------------------------------------------------------------------
