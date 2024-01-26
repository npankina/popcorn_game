#pragma once
#include "Monster.h"


//------------------------------------------------------------------------------------------------------------
enum class EMonster_Set_State : unsigned char
{
	Idle, // ничего не делаем
	Selecting_Next_Gate,
	Waiting_Gate_Openening,
	Waiting_Gate_Closing
};
//------------------------------------------------------------------------------------------------------------
class AsMonster_Set: public AHit_Checker, public AGame_Objects_Set
{
public:
	AsMonster_Set();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);

	virtual void Act();
	void Init(AsBorder *border);
	void Activate(int max_alive_monsters_count);
	void Emit_At_Gate(int gate_index);

private:
	bool Get_Next_Game_Object(int &index, AGame_Object **game_obj); // iterator

	AsBorder *Border; // UNO
	static const int Max_Monsters_Count = 10;
	int Current_Gate_Index;
	int Max_Alive_Monsters_Count;
	AMonster Monsters[Max_Monsters_Count];
	EMonster_Set_State Monster_Set_State;
};