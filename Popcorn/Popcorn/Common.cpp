﻿#include "Common.h"

// AMover
//------------------------------------------------------------------------------------------------------------
AMover::~AMover()
{}
//------------------------------------------------------------------------------------------------------------




// AGraphics_Object
//------------------------------------------------------------------------------------------------------------
AGraphics_Object::~AGraphics_Object()
{}
//------------------------------------------------------------------------------------------------------------




// AGame_Objects_Set
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Begin_Movement()
{
	int index = 0;
	AGame_Object *object;

	while (Get_Next_Game_Object(index, &object) )
		object->Begin_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Finish_Movement()
{
	int index = 0;
	AGame_Object *object;

	while (Get_Next_Game_Object(index, &object) )
		object->Finish_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Advance(double max_speed)
{
	int index = 0;
	AGame_Object *object;

	while (Get_Next_Game_Object(index, &object) )
		object->Advance(max_speed);
}
//------------------------------------------------------------------------------------------------------------
double AGame_Objects_Set::Get_Speed()
{
	int index = 0;
	AGame_Object *object;
	double curr_speed, max_speed = 0.0;

	while (Get_Next_Game_Object(index, &object) )
	{
		curr_speed = object->Get_Speed();

		if (curr_speed > max_speed)
			max_speed = curr_speed;
	}

	return max_speed;
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Act()
{
	int index = 0;
	AGame_Object *object;

	while (Get_Next_Game_Object(index, &object) )
		object->Act();
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Clear(HDC hdc, RECT &paint_area)
{
	int index = 0;
	AGame_Object *object;

	while (Get_Next_Game_Object(index, &object) )
		object->Clear(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Draw(HDC hdc, RECT &paint_area)
{
	int index = 0;
	AGame_Object *object;

	while (Get_Next_Game_Object(index, &object) )
		object->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AGame_Objects_Set::Is_Finished()
{
	return false;  // Заглушка, т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------



// class AString
//------------------------------------------------------------------------------------------------------------
AString::AString()
{}
//------------------------------------------------------------------------------------------------------------
AString::AString(const wchar_t *str) : Content(str)
{}
//------------------------------------------------------------------------------------------------------------
void AString::Append(int value, int digits)
{
	const int size = 32;
	wchar_t format[size];
	wchar_t buf[size];

	swprintf(format, size, L"%%.%ii", digits);
	swprintf(buf, size, format, value);
	Content += buf;
}
//------------------------------------------------------------------------------------------------------------
const wchar_t *AString::Get_Content()
{
	return Content.c_str();
}
//------------------------------------------------------------------------------------------------------------
int AString::Get_Length()
{
	return Content.length();
}
//------------------------------------------------------------------------------------------------------------
void AString::Clear()
{
	Content = L"";
}
//------------------------------------------------------------------------------------------------------------




// class AMessage
//------------------------------------------------------------------------------------------------------------
AMessage::AMessage(EMessage_Type message_type) : Message_Type(message_type)
{}
//------------------------------------------------------------------------------------------------------------




// class AsMessage_Manager
std::queue<AMessage *> AsMessage_Manager::Messages_Queue;
//------------------------------------------------------------------------------------------------------------
void AsMessage_Manager::Add_Message(AMessage *message)
{
	Messages_Queue.push(message);
}
//------------------------------------------------------------------------------------------------------------
bool AsMessage_Manager::Get_Message(AMessage **message)
{
	if (Messages_Queue.size() == 0)
		return false;

	*message = Messages_Queue.front();

	Messages_Queue.pop();

	return true;
}
//------------------------------------------------------------------------------------------------------------
