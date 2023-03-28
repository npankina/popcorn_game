#include "Monster_Set.hpp"

AsMonster_Set::AsMonster_Set()
: Border(0)
{}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder *border)
{
	Border = border;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
	AMonster *monster = 0;
	int gate_x_pos, gate_y_pos;

	if (gate_index < 0 || gate_index > AsConfig::Gates_Number)
		AsConfig::Throw(); // несуществующий гейт

	for (int i = 0; i < Max_Monsters_Count; i++)
	{
		if (! Monsters[i].Is_Active() )
		{
			monster = &Monsters[i];
			break;
		}
	}

	if (monster == 0)
		return; // Все монстры уже в игре

	Border->Get_Gate_Pos(gate_index, gate_x_pos, gate_y_pos);

	monster->Activate(gate_x_pos, gate_y_pos);

	monster->Destroy();
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Get_Next_Game_Object(int &index, AGame_Object **game_obj) // **game_obj указатель на указатель
{
	if (index < 0 || index >= Max_Monsters_Count)
		return false;

	*game_obj = &Monsters[index++]; //  в указатель помещается адрес объекта, index по ссылке инкрементируется
	return true;
}
