#pragma once

#include <Windows.h> // имена переменных Windows

enum EKey_Type
{
	EKT_Left,
	EKT_Right,
	EKT_Space
};

// Интерфейс модуля Engine.cpp
void Init_Engine(HWND hWnd);
void Draw_Frame(HDC hdc, RECT &paint_area);
int On_Key_Down(EKey_Type key_type);