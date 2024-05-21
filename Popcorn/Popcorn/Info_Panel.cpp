﻿#include "Info_Panel.h"


// class AsInfo_Panel
//------------------------------------------------------------------------------------------------------------
AsInfo_Panel::~AsInfo_Panel()
{
	delete Shadow_Color;
	delete Highlight_Color;
	delete Dark_Blue;

	if (Logo_Pop_Font != 0)
		DeleteObject(Logo_Pop_Font);

	if (Logo_Corn_Font != 0)
		DeleteObject(Logo_Corn_Font);

	if (Player_Name_Font != 0)
		DeleteObject(Player_Name_Font);

	if (Score_Font != 0)
		DeleteObject(Score_Font);
}
//------------------------------------------------------------------------------------------------------------
AsInfo_Panel::AsInfo_Panel()
: Logo_Pop_Font{}, Logo_Corn_Font{}, Player_Name_Font{}, Score_Font{}, Shadow_Color(nullptr), Highlight_Color(nullptr), Dark_Blue(nullptr),
	Letter_P(EBrick_Type::Blue, ELetter_Type::P, 214 * AsConfig::Global_Scale + 1, 153 * AsConfig::Global_Scale),
	Letter_G(EBrick_Type::Blue, ELetter_Type::G, 256 * AsConfig::Global_Scale, 153 * AsConfig::Global_Scale),
	Letter_M(EBrick_Type::Blue, ELetter_Type::M, 297 * AsConfig::Global_Scale - 1, 153 * AsConfig::Global_Scale)
{
	Letter_P.Show_Full_Size();
	Letter_G.Show_Full_Size();
	Letter_M.Show_Full_Size();
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Begin_Movement() // Заглушка, не используется.
{}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Finish_Movement() // Заглушка, не используется.
{}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Advance(double max_speed) // Заглушка, не используется.
{}
//------------------------------------------------------------------------------------------------------------
bool AsInfo_Panel::Is_Finished() // Заглушка, не используется.
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
double AsInfo_Panel::Get_Speed() // Заглушка, не используется.
{
	return 0.0;
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Act()
{
	//const int scale = AsConfig::Global_Scale;
	//RECT rect{};

	//rect.left = 211 * scale;
	//rect.top = 5 * scale;
	//rect.right = rect.left + 104 * scale;
	//rect.bottom = 199 * scale;

	//AsTools::Invalidate_Rect(rect);
}

//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Clear(HDC hdc, RECT &paint_area)
{}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	int logo_x_pos = 212 * scale;
	int logo_y_pos = 0;
	int shadow_x_offset = 5 * scale;
	int shadow_y_offset = 5 * scale;
	const wchar_t *pop_str = L"POP"; // задаем строку в 2 байта
	const wchar_t *corn_str = L"CORN";
	AString score_str = L"SCORE:";
	RECT rect;

	// 1. Logo
	AsTools::Rect(hdc, 211, 5, 104, 100, AsConfig::Blue_Color);

	SelectObject(hdc, Logo_Corn_Font);
	SetBkMode(hdc, TRANSPARENT);

	// 1.1. "POP"
	SelectObject(hdc, Logo_Pop_Font);
	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());
	TextOut(hdc, logo_x_pos + shadow_x_offset, logo_y_pos + shadow_y_offset, pop_str, wcslen(pop_str)); // shadow
	SetTextColor(hdc, AsConfig::Red_Color.Get_RGB());
	TextOut(hdc, logo_x_pos, logo_y_pos, pop_str, 3); // main letters

	// 1.2. "CORN"
	SelectObject(hdc, Logo_Corn_Font);
	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());
	TextOut(hdc, logo_x_pos + shadow_x_offset - 5 * scale, logo_y_pos + shadow_y_offset + 44 * scale, corn_str, 4); // shadow
	SetTextColor(hdc, AsConfig::Red_Color.Get_RGB());
	TextOut(hdc, logo_x_pos - 5 * scale, logo_y_pos + 44 * scale, corn_str, wcslen(corn_str)); // main letters


	// 2. Таблица счета
	// 2.1. Рамка
	AsTools::Rect(hdc, Score_X, Score_Y, Score_Width, 2, AsConfig::Red_Color);
	AsTools::Rect(hdc, Score_X, Score_Y + Score_Height - 2, Score_Width, 2, AsConfig::Red_Color);
	AsTools::Rect(hdc, Score_X, Score_Y, 2, Score_Height, AsConfig::Red_Color);
	AsTools::Rect(hdc, Score_X + Score_Width - 2, Score_Y, 2, Score_Height, AsConfig::Red_Color);

	AsTools::Rect(hdc, Score_X + 2, Score_Y + 2, Score_Width - 4, Score_Height - 4, *Dark_Blue);

	// 2.2. Бордюр
	Highlight_Color->Select_Pen(hdc);
	MoveToEx(hdc, (Score_X + 2) * scale, (Score_Y + Score_Height - 2) * scale, 0);
	LineTo(hdc, (Score_X + 2) * scale, (Score_Y + 2) * scale);
	LineTo(hdc, (Score_X + Score_Width - 2) * scale, (Score_Y + 2) * scale);

	Shadow_Color->Select_Pen(hdc);
	MoveToEx(hdc, (Score_X + Score_Width - 2) * scale, (Score_Y + 2) * scale, 0);
	LineTo(hdc, (Score_X + Score_Width - 2) * scale, (Score_Y + Score_Height - 2) * scale);
	LineTo(hdc, (Score_X + 2) * scale, (Score_Y + Score_Height - 2) * scale);

	// 2.3. Имя игрока
	rect.left = (Score_X + 5) * scale;
	rect.top = (Score_Y + 5) * scale;
	rect.right = rect.left + (Score_Width - 2 * 5) * scale;
	rect.bottom = rect.top + 16 * scale;

	Player_Name = L"Computer";
	Draw_String(hdc, rect, Player_Name, true);

	// 3. Считаем очки
	//AsTools::Rect(hdc, Score_X + 5, Score_Y + 27, Score_Width - 2 * 5, 16, AsConfig::Red_Color);
	rect.top += Score_Value_Offset * scale;
	rect.bottom += Score_Value_Offset * scale;

	score_str.Append(AsConfig::Current_Score);
	Draw_String(hdc, rect, score_str, false);

	// 4. Буквы индикаторы
	Letter_P.Draw(hdc, paint_area);
	Letter_G.Draw(hdc, paint_area);
	Letter_M.Draw(hdc, paint_area);

	// 5. Индикаторы
	AsTools::Rect(hdc, Score_X + 8, Score_Y + 55, 12, 30, AsConfig::Teleport_Portal_Color); // left
	AsTools::Rect(hdc, Score_X + 27, Score_Y + 55, 56, 30, AsConfig::Teleport_Portal_Color); // right
	AsTools::Rect(hdc, Score_X + 90, Score_Y + 54, 12, 30, AsConfig::Teleport_Portal_Color); // middle

	// 6. Выыод кол-ва жизней на индикатор
	Show_Extra_Lifes(hdc);
	//for (int i = 0; i < 4; i++) // y
	//	for (int j = 0; j < 3; j++) // x
	//		Draw_Extra_Life(hdc, 33 + j * 16, 57 + i * 7);
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Show_Extra_Lifes(HDC hdc)
{
	int lifes_to_draw = AsConfig::Extra_Lives_Count;

	for (int j = 0; j < 3; j++) // x
		for (int i = 0; i < 4; i++) // y
		{
			if (lifes_to_draw <= 0)
				break;

			Draw_Extra_Life(hdc, 33 + j * 16, 57 + i * 7);
			--lifes_to_draw;
		}
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_Extra_Life(HDC hdc, int x_pos, int y_pos)
{
	const int scale = AsConfig::Global_Scale;
	RECT rect;

	rect.left = (Score_X + x_pos) * scale;
	rect.top = (Score_Y + y_pos) * scale;
	rect.right = rect.left + 4 * scale;
	rect.bottom = rect.top + 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	rect.left += 8 * scale;
	rect.right += 8 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	rect.left = (Score_X + x_pos + 2) * scale;
	rect.top = (Score_Y + y_pos + 1) * scale;
	rect.right = rect.left + 8 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsConfig::Blue_Color.Select(hdc);
	AsTools::Round_Rect(hdc, rect);
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_String(HDC hdc, RECT &rect, AString &name_str, bool draw_name)
{
	int str_left_offset, str_top_offset;
	const int scale = AsConfig::Global_Scale;
	SIZE str_size;

	// 1. Выводим прямоугольник фона
	AsTools::Rect(hdc, rect, AsConfig::Red_Color);

	// 2. Выводим строку
	if (draw_name)
		SelectObject(hdc, Player_Name_Font);
	else
		SelectObject(hdc, Score_Font);

	SetTextColor(hdc, AsConfig::White_Color.Get_RGB());

	GetTextExtentPoint32(hdc, name_str.Get_Content(), name_str.Get_Length(), &str_size);

	str_left_offset = rect.left + (rect.right - rect.left) / 2 - str_size.cx / 2;
	str_top_offset = rect.top + (rect.bottom - rect.top) / 2 - str_size.cy / 2 - scale;

	// 2.1. Вывод тени
	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());
	TextOut(hdc, str_left_offset + 2 * scale, str_top_offset + 2 * scale, name_str.Get_Content(), name_str.Get_Length() );

	// 2.2. Вывод строки
	if (draw_name)
		SetTextColor(hdc, AsConfig::Blue_Color.Get_RGB());
	else
		SetTextColor(hdc, AsConfig::White_Color.Get_RGB());

	TextOut(hdc, str_left_offset, str_top_offset, name_str.Get_Content(), name_str.Get_Length() );
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Init()
{
	Shadow_Color = new AColor(AsConfig::BG_Color, AsConfig::Global_Scale);
	Highlight_Color = new AColor(AsConfig::White_Color, AsConfig::Global_Scale);
	Dark_Blue = new AColor(0, 170, 170);

	// Установка шрифта для Лого
	LOGFONT log_font{};

	log_font.lfHeight = -128;
	log_font.lfWeight = 900;
	log_font.lfOutPrecision = 3;
	log_font.lfClipPrecision = 2;
	log_font.lfQuality = 1;
	log_font.lfPitchAndFamily = 34;
	wcscpy_s(log_font.lfFaceName, L"Arial Black");

	Logo_Pop_Font = CreateFontIndirect(&log_font);

	log_font.lfHeight = -96;
	Logo_Corn_Font = CreateFontIndirect(&log_font);


	// Установка шрифта для имени Игрока

	log_font.lfHeight = -48;
	log_font.lfWeight = 700;
	log_font.lfOutPrecision = 3;
	log_font.lfClipPrecision = 2;
	log_font.lfQuality = 1;
	log_font.lfPitchAndFamily = 49;
	wcscpy_s(log_font.lfFaceName, L"Consolas");

	log_font.lfHeight = -46;
	Player_Name_Font = CreateFontIndirect(&log_font);

	// Установка шрифта для счета
	log_font.lfHeight = -44;
	Score_Font = CreateFontIndirect(&log_font);

}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Choose_Font()
{
	CHOOSEFONT cf{};
	LOGFONT lf{};

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.lpLogFont = &lf;
	cf.Flags = CF_SCREENFONTS;
	cf.nFontType = SCREEN_FONTTYPE;

	ChooseFont(&cf);
}
//------------------------------------------------------------------------------------------------------------