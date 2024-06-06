﻿#pragma once
#include "Monster.h"
#include "Border.h"


//------------------------------------------------------------------------------------------------------------
enum class EMonster_Set_State : unsigned char
{
	Idle, // ничего не делаем
	Selecting_Next_Gate,
	Waiting_Gate_Opening,
	Waiting_Gate_Closing
};
//------------------------------------------------------------------------------------------------------------
class AsMonster_Set: public AHit_Checker, public AGame_Objects_Set
{
public:
	~AsMonster_Set();
	AsMonster_Set();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);

	virtual void Act();

	void Init(AsBorder *border);
	void Emit_At_Gate(int gate_index);
	void Activate(int max_alive_monsters_count);
	void Destroy_All();
	void Freeze();
	void Unfreeze();

private:
	bool Get_Next_Game_Object(int &index, AGame_Object **game_obj); // iterator

	EMonster_Set_State Monster_Set_State;
	int Current_Gate_Index;
	int Max_Alive_Monsters_Count;
	AsBorder *Border;  // UNO

	static const int Max_Monsters_Count = 10;

	std::vector<AMonster *> Monsters;
}; 