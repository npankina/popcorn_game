﻿#pragma once
#include "Border.h"
#include "Level.h"
#include "Platform.h"
#include "Ball_Set.h"
#include "Monster_Set.h"
#include "Info_Panel.h"

//------------------------------------------------------------------------------------------------------------
enum class EKey_Type: unsigned char
{
	Left,
	Right,
	Space
};
//------------------------------------------------------------------------------------------------------------
enum class EGame_State: unsigned char
{
	Test_Ball,

	Mop_Level,
	Play_Level,
	Lost_Ball,
	Restart_Level,
	Finish_Level,
	Game_Over
};
//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
	AsEngine();

	void Init_Engine(HWND hwnd);
	void Draw_Frame(HDC hdc, RECT &paint_area);
	int On_Key(EKey_Type key_type, bool key_down);
	int On_Timer();

	const int Timer_ID;

private:
	void Restart_Level();
	void Play_Level();
	void Stop_Play();
	void Game_Over();
	void Game_Won();
	void Advance_Movers();
	void Act();
	void Handle_Message();
	void On_Falling_Letter(AFalling_Letter *falling_letter);
	bool Is_Destroying_Complete();

	EGame_State Game_State;
	double Rest_Distance;

	AsLevel Level;
	AsPlatform Platform;
	AsBorder Border;
	AsBall_Set Ball_Set;
	AsLaser_Beam_Set Laser_Beam_Set;
	AsMonster_Set Monster_Set;
	AsInfo_Panel Info_Panel;

	std::vector<AGame_Object *> Modules; // UNO; Главные графические объекты игры
};
//------------------------------------------------------------------------------------------------------------
