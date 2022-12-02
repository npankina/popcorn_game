﻿#pragma once

#include "Border.h"
#include "Ball.h"
#include "Platform.h"
#include "Active_Brick.h"

//------------------------------------------------------------------------------------------------------------
// Global Variables:
const int Timer_ID = WM_USER + 1;
//------------------------------------------------------------------------------------------------------------
enum EGame_State
{
	EGS_Play_Level,
	EGS_Lost_Ball,
	EGS_Restart_Level
};
//------------------------------------------------------------------------------------------------------------
enum EKey_Type
{
	EKT_Left,
	EKT_Right,
	EKT_Space
};
//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
	AsEngine();

	void Init_Engine(HWND hWnd);
	void Draw_Frame(HDC hdc, RECT &paint_area);
	int On_Key_Down(EKey_Type key_type);
	int On_Timer();

private:
	EGame_State Game_State;
	ABall Ball;
	ALevel Level;
	APlatform Platform;
	AsBorder Border;
};
//------------------------------------------------------------------------------------------------------------
