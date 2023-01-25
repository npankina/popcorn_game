#include "Engine.h"

// AsEngine
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
: Game_State(EGS_Lost_Ball), // EGS_Test_Ball EGS_Play_Level , EGS_Play_Level
  Rest_Distanse(0.0), Life_Counter(AsConfig::Initial_Life_Count)
{
	memset(Movers, 0, sizeof(Movers) );
	memset(Modules, 0, sizeof(Modules) );

}
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
	Platform.Init(&Ball_Set);

	AFalling_Letter::Init();

	ABall::Add_Hit_Checker(&Border);
	ABall::Add_Hit_Checker(&Level);
	ABall::Add_Hit_Checker(&Platform);

	//Level.Set_Current_Level(AsLevel::Test_Level);
	Level.Set_Current_Level(AsLevel::Level_02);


	//Ball.Set_State(EBS_Normal, (double)(Platform.X_Pos + Platform.Width / 2));
	//Platform.Set_State(EPS_Normal);
	//Platform.Set_State(EPS_Glue_Init);

	Platform.Redraw_Platform();

	SetTimer(AsConfig::Hwnd, Timer_ID, 1000 / AsConfig::FPS, 0);

	// Movers
	Movers[0] = &Platform;
	Movers[1] = &Ball_Set;

	// Modules
	Modules[0] = &Level;
	Modules[1] = &Border;
	Modules[2] = &Platform;
	Modules[4] = &Ball_Set;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT &paint_area)
{// Отрисовка экрана игры
	
	int i;
	SetGraphicsMode(hdc, GM_ADVANCED);

	for (i = 0; i < AsConfig::Max_Modules_Count; i++)
		if (Modules[i] != 0)
			Modules[i]->Clear(hdc, paint_area);
	
	for (i = 0; i < AsConfig::Max_Modules_Count; i++)
		if (Modules[i] != 0)
			Modules[i]->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Key(EKey_Type key_type, bool is_key_down)
{

	switch (key_type)
	{
	case EKT_Left:
		Platform.Move(true, is_key_down);
		break;


	case EKT_Right:
		Platform.Move(false, is_key_down);
		break;


	case EKT_Space:
		Platform.On_Space_Key(is_key_down);
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

		Ball_Set.Set_For_Test();
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
		{
			Game_State = EGS_Play_Level;
			Ball_Set.Set_On_Platform(Platform.Get_Middle_Pos() );
			//Platform.Set_State(EPS_Glue_Init);
		}
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

	if (Platform.Get_State() != EPS_Ready)
		Ball_Set.Act();

	while (Level.Get_Next_Falling_letter(index, &falling_letter) )
	{
		if (Platform.Hit_By(falling_letter) )
			On_Falling_letter(falling_letter);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Play_Level()
{

	Advance_Movers();
	if (Ball_Set.All_Balls_Are_Lost() )
	{
		Game_State = EGS_Lost_Ball;
		Level.Stop();
		Platform.Set_State(EPS_Pre_Meltdown);
	}
	else
		Ball_Set.Accelerate();

	if (Ball_Set.Is_Test_Finished() )
			Game_State = EGS_Test_Ball;

}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Advance_Movers()
{
	int i;
	double max_speed = 0.0;
	double current_speed;


	// 1. Получение максимальной скорости движущихся объектов
	for (i = 0; i < AsConfig::Max_Movers_Count; i++)
	{ 
		if (Movers[i] != 0)
		{
			current_speed = fabs(Movers[i]->Get_Speed() );
			Movers[i]->Begin_Movement();

			if (current_speed > max_speed)
				max_speed = current_speed;
		}
	}

	// 2. Смещаем все движущиеся объекты
	Rest_Distanse += max_speed;

	while (Rest_Distanse > 0.0)
	{
		for (i = 0; i < AsConfig::Max_Movers_Count; i++)
			if (Movers[i] != 0)
				Movers[i]->Advance(max_speed);

		//Platform.Advance(max_speed);
		Rest_Distanse -= AsConfig::Moving_Step_Size;
	}

	// 3. Заканчиваем все движения на этом кадре
	for (i = 0; i < AsConfig::Max_Movers_Count; i++)
		if (Movers[i] != 0)
			Movers[i]->Finish_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::On_Falling_letter(AFalling_Letter *falling_letter)
{
	switch(falling_letter->Letter_Type)
	{
	case ELT_O: // "Отмена"
		Platform.Set_State(EPS_Glue_Finalize); // Отменяет действие буквы Клей
		break;

	case ELT_I: // "Инверсия"
		Ball_Set.Inverse();
		break;

	case ELT_C: // "Скорость"
		Ball_Set.Reset_Speed();
		break;

	//case ELT_M: // "Монстры"
		//break;

	case ELT_G: // "Жизнь"
		if (Life_Counter < AsConfig::Max_Life_Count)
			++Life_Counter; // !!! отобразить на индикаторе
		break;

	case ELT_K: // "Клей"
		Platform.Set_State(EPS_Glue_Init);
		break;

	//case ELT_W: // "Шире"
		//break;

	case ELT_P: // "Пол"
		AsConfig::Level_Has_Floor = true;
		Border.Redraw_Floor();
		// !!! отобразить пол на индикаторе
		break;

	//case ELT_L: // "Лазер"
		//break;

	case ELT_T: // "Три"
		Ball_Set.Tripple_Balls();
		break;

	//case ELT_Plus: // "Переход на след. уровень"
		//break;

	default:
		AsConfig::Throw();
	}

	falling_letter->Finalize();

}
//------------------------------------------------------------------------------------------------------------
