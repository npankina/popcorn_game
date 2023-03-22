#pragma once
#include "Monster.hpp"
#include "Border.hpp"

class AsMonster_Set: public AsGame_Objects_Set
{
public:
	AsMonster_Set();
	void Init(AsBorder *border);
	void Emit_At_Gate(int gate_index);

private:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **game_obj); // iterator

	AsBorder *Border; // UNO
	static const int Max_Monsters_Count = 10;
	AMonster Monsters[Max_Monsters_Count];
};