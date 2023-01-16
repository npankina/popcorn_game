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
class AsInfo_Panel
{
public:

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
	void Restart_Level();
	void Play_Level();
	void On_Falling_letter(AFalling_Letter *falling_letter);

	EGame_State Game_State;

	AsLevel Level;
	AsPlatform Platform;
	AsBorder Border;

	AsInfo_Panel Info_Panel;
	ABall Balls[AsConfig::Max_Balls_Count];
};
//------------------------------------------------------------------------------------------------------------
