#include "Engine.h"

// Global Variables:
const int Global_Scale = 3;
const int Brick_Width = 15;
const int Brick_Height = 7;
const int Cell_Width = 16;
const int Cell_Height = 8;
const int Level_X_Offset = 8;
const int Level_Y_Offset = 6;
const int Circle_Size = 7;


int Inner_Width = 21;

HPEN Highlight_Pen, Brick_Red_Pen, Brick_Blue_Pen;
HBRUSH Brick_Red_Brush, Brick_Blue_Brush;

enum EBrick_Type
{
	EBT_None,
	EBT_Red,
	EBT_Blue
};

char Level_01[14][12] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//------------------------------------------------------------------------------------------------------------
void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush)
{
	pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
	brush = CreateSolidBrush(RGB(r, g, b));
}
//------------------------------------------------------------------------------------------------------------
void Init()
{// Ќастройка игры при старте

	Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
	Create_Pen_Brush(230, 25, 229, Brick_Red_Pen, Brick_Red_Brush); // 255, 129, 249
	Create_Pen_Brush(0, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush); // 85, 255, 255
}
//------------------------------------------------------------------------------------------------------------
void Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type)
{// ¬ывод "кирпича"

	HPEN pen;
	HBRUSH brush;

	switch (brick_type)
	{
	case EBT_None:
		return;
	case EBT_Red:
		pen = Brick_Red_Pen; // создать карандаш
		brush = Brick_Red_Brush; // создали кисть
		break;
	case EBT_Blue:
		pen = Brick_Blue_Pen;
		brush = Brick_Blue_Brush;
		break;
	default:
		return;
	}

	SelectObject(hdc, pen); // назначить карандаш текущим (дл€ отрисовки)
	SelectObject(hdc, brush);  // назначить кисть текущей (дл€ отрисовки)

	RoundRect(hdc, x * Global_Scale, y * Global_Scale, (x + Brick_Width) * Global_Scale, (y + Brick_Height) * Global_Scale, 2 * Global_Scale, 2 * Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
void Draw_Level(HDC hdc)
{// ¬ывод всех кирпичей уровн€

	int i, j;

	for (i = 0; i < 14; i++)
		for (j = 0; j < 12; j++)
			Draw_Brick(hdc, Level_X_Offset + j * Cell_Width, Level_Y_Offset + i * Cell_Height, (EBrick_Type)Level_01[i][j]);
}
//------------------------------------------------------------------------------------------------------------
void Draw_Platform(HDC hdc, int x, int y)
{// ќтрисовка платформы м€ча

	SelectObject(hdc, Brick_Red_Pen); // назначить карандаш текущим (дл€ отрисовки)
	SelectObject(hdc, Brick_Red_Brush);  // назначить кисть текущей (дл€ отрисовки)
	Ellipse(hdc, x * Global_Scale, y * Global_Scale, (x + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
	Ellipse(hdc, (x + Inner_Width) * Global_Scale, y * Global_Scale, (x + Inner_Width + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);

	SelectObject(hdc, Brick_Blue_Pen); // назначить карандаш текущим (дл€ отрисовки)
	SelectObject(hdc, Brick_Blue_Brush);  // назначить кисть текущей (дл€ отрисовки)

	RoundRect(hdc, (x + 4) * Global_Scale, (y + 1) * Global_Scale, (x + 4 + Inner_Width - 1) * Global_Scale, (y + 1 + 5) * Global_Scale, 3 * Global_Scale, 3 * Global_Scale);
	
	SelectObject(hdc, Highlight_Pen);
	Arc(hdc, (x + 1) * Global_Scale, (y + 1) * Global_Scale, (x + Circle_Size - 1) * Global_Scale, (y + Circle_Size - 1) * Global_Scale,
			 (x + 1 + 1) * Global_Scale, (y + 1) * Global_Scale, (x + 1) * Global_Scale, (y + 1 + 2) * Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
void Draw_Frame(HDC hdc)
{// ќтрисовка экрана игры

	Draw_Level(hdc);

	Draw_Platform(hdc, 50, 100);
}
//------------------------------------------------------------------------------------------------------------
