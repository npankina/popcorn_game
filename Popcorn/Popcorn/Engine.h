#pragma once

#include <Windows.h>

#include "Border.h"
#include "Level.h"
#include "Platform.h"

//------------------------------------------------------------------------------------------------------------
enum EKey_Type
{
	EKT_Left,
	EKT_Right,
	EKT_Space
};
//------------------------------------------------------------------------------------------------------------
enum EGame_State
{
	EGS_Test_Ball,

	EGS_Play_Level,
	EGS_Lost_Ball,
	EGS_Restart_Level
};
//------------------------------------------------------------------------------------------------------------
class AsBall_Set: public AMover
{
public:
	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Begin_Movement();
	virtual void Finish_Movement();

	void Draw(HDC hdc, RECT &paint_area);
	void Release_From_Platform(double platform_x_pos);
	void Set_On_Platform(double platform_x_pos);
	bool All_Balls_Are_Lost();
	void Set_For_Test();
	bool Is_Test_Finished();

private:
	ABall Balls[AsConfig::Max_Balls_Count];
};
//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
	AsEngine();

	void Init_Engine(HWND hwnd);
	void Draw_Frame(HDC hdc, RECT &paint_area);
	int On_Key(EKey_Type key_type, bool is_key_down);
	int On_Timer();

	static const int Timer_ID = WM_USER + 1;

private:
	void Act();
	void Play_Level();
	void Advance_Mover();
	void On_Falling_letter(AFalling_Letter *falling_letter);

	EGame_State Game_State;
	double Rest_Distanse;

	AsLevel Level;
	AsPlatform Platform;
	AsBorder Border;
	AsBall_Set Ball_Set;
	
	AMover *Movers[AsConfig::Max_Movres_Count];
};
//------------------------------------------------------------------------------------------------------------
