#include "Monster_Set.h"


// class AsMonster_Set
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::~AsMonster_Set()
{
	for (auto *monster : Monsters)
		delete monster;

	Monsters.erase(Monsters.begin(), Monsters.end());
}
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
: Border(0), Is_Frozen(false), Monster_Set_State(EMonster_Set_State::Idle), Current_Gate_Index(0), Max_Alive_Monsters_Count(0)
{}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
	for (auto *monster : Monsters)
		if (monster->Check_Hit(next_x_pos, next_y_pos, ball) )
			return true;
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos)
{
	for (auto *monster : Monsters)
		if (monster->Check_Hit(next_x_pos, next_y_pos))
			return true;
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(RECT &rect)
{
	for (auto *monster : Monsters)
		if (monster->Check_Hit(rect) )
			return true;
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Act()
{
	int current_alive_count; // для подсчета живых монстров

	switch (Monster_Set_State)
	{
	case EMonster_Set_State::Idle:
		break;

	case EMonster_Set_State::Selecting_Next_Gate:
		if (Is_Frozen)
			break;

	    current_alive_count = 0;

		for (auto *monster : Monsters)
			if (monster->Is_Finished())
				++current_alive_count;

		// добавляем нового монстра, если можно
		if (current_alive_count < Max_Alive_Monsters_Count)
		{
			Current_Gate_Index = Border->Long_Open_Gate();
			Monster_Set_State = EMonster_Set_State::Waiting_Gate_Opening; // ждем открытия гейта
		}
		break;


	case EMonster_Set_State::Waiting_Gate_Opening:
		if (Border->Is_Gate_Opened(Current_Gate_Index) )
		{
			Emit_At_Gate(Current_Gate_Index); // выпускаем монстра из гейта
			Monster_Set_State = EMonster_Set_State::Waiting_Gate_Closing; // перевод состояния в ожидание закрытия гейта
		}
		break;


	case EMonster_Set_State::Waiting_Gate_Closing:
		if (Border->Is_Gate_Closed(Current_Gate_Index)) // только если гейт закрыт
			Monster_Set_State = EMonster_Set_State::Selecting_Next_Gate;
		break;


	default: // если добавятся новые состояния - ловим ошибку из-за отсутствия реализованной логики
		AsConfig::Throw();
	}

	if (Monster_Set_State != EMonster_Set_State::Idle)
	{
		auto it = Monsters.begin();

		while (it != Monsters.end())
			if ((*it)->Is_Finished())
			{
				delete *it;
				it = Monsters.erase(Monsters.begin(), Monsters.end());
			}
			else
				++it;
	}

	AGame_Objects_Set::Act(); // выполняется вызов метода базового класса
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder *border)
{
	Border = border;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
	AMonster *monster = 0;
	bool gate_is_left = false; // задаем движение монстра (все четные гейты находятся справа, а нечетные слева)
	int gate_x_pos, gate_y_pos;
	int monster_type;
	double x_pos, y_pos;

	if (Is_Frozen)
		return;

	if (gate_index < 0 or gate_index >= AsConfig::Gates_Count)
		AsConfig::Throw();


	// Все монстры уже в игре
	if (Monsters.size() >= Max_Monsters_Count)
		return;


	// Выбираем тип монстра и добавляем в коллекцию
	monster_type = AsTools::Rand(2);

	if (monster_type == 0)
		monster = new AMonster_Eye();
	else 
		monster = new AMonster_Comet();
	Monsters.push_back(monster);

	if (monster == 0)
		return; // все монстры заняты - уе на поле

	Border->Get_Gate_Pos(gate_index, gate_x_pos, gate_y_pos);

	x_pos = (double)gate_x_pos;
	y_pos = (double)gate_y_pos;

	if (gate_index % 2 == 0)
		gate_is_left = true; // гейт левый, иначе правый

	if (!gate_is_left)
		x_pos -= (double)monster->Width - AGate::Width; // вычисление положения монстра справа

	monster->Activate(x_pos, y_pos + 1.5, gate_is_left);

	//monster->Destroy();
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Activate(int max_alive_monsters_count)
{
	Monster_Set_State = EMonster_Set_State::Selecting_Next_Gate;
	Max_Alive_Monsters_Count = max_alive_monsters_count;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Destroy_All()
{
	for (auto &item : Monsters)
		item->Destroy();

	Monster_Set_State = EMonster_Set_State::Idle;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Set_Freeze_State(bool freeze)
{
	Is_Frozen = freeze;

	for (auto &item : Monsters)
		item->Set_Freeze_State(freeze);
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Are_All_Destroyed()
{
	if (Monsters.size() == 0)
		return true;
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Get_Next_Game_Object(int &index, AGame_Object **game_obj) // **game_obj указатель на указатель
{
	if (index < 0 or index >= Monsters.size() )
		return false;


	*game_obj = Monsters[index++]; //  в указатель помещается адрес объекта, index по ссылке инкрементируется
	return true;
}