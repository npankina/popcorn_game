#include "Info_Panel.h"

// class ALabel
//------------------------------------------------------------------------------------------------------------
ALabel::ALabel(int x, int y, int width, int height, const AFont &font)
: X_Pos(x), Y_Pos(y), Width(width), Height(height), Font(font), Content_Rect{}
{
	Content_Rect.left = X_Pos * scale_;
	Content_Rect.top = Y_Pos * scale_;
	Content_Rect.right = Content_Rect.left + Width * scale_;
	Content_Rect.bottom = Content_Rect.top + Height * scale_;
}
//------------------------------------------------------------------------------------------------------------
void ALabel::Draw(HDC hdc)
{
	int str_left_offset, str_top_offset;
	const int scale = AsConfig::Global_Scale;
	SIZE str_size;

	// 1. Выводим строку
	Font.Select(hdc);

	SetTextColor(hdc, AsConfig::White_Color.Get_RGB());

	GetTextExtentPoint32(hdc, Content.Get_Content(), Content.Get_Length(), &str_size);

	str_left_offset = Content_Rect.left + (Content_Rect.right - Content_Rect.left) / 2 - str_size.cx / 2;
	str_top_offset = Content_Rect.top + (Content_Rect.bottom - Content_Rect.top) / 2 - str_size.cy / 2 - scale;

	// 2.1. Вывод тени
	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());
	TextOut(hdc, str_left_offset + 2 * scale, str_top_offset + 2 * scale, Content.Get_Content(), Content.Get_Length());

	// 2.2. Вывод строки
	SetTextColor(hdc, AsConfig::Blue_Color.Get_RGB());

	TextOut(hdc, str_left_offset, str_top_offset, Content.Get_Content(), Content.Get_Length());
}
//------------------------------------------------------------------------------------------------------------
void ALabel::Set_Content(AString cont)
{
	Content = cont;
}
//------------------------------------------------------------------------------------------------------------
void ALabel::Set_Append(int score)
{
	Content.Append(score);
}
//------------------------------------------------------------------------------------------------------------
AString& const ALabel::Get_Content()
{
	return Content;
}
//------------------------------------------------------------------------------------------------------------
RECT& const ALabel::Get_Content_Rect()
{
	return Content_Rect;
}
//------------------------------------------------------------------------------------------------------------





// class AsInfo_Panel
//------------------------------------------------------------------------------------------------------------
int AsInfo_Panel::Score = 0;
RECT AsInfo_Panel::Logo_Rect;
RECT AsInfo_Panel::Data_Rect;
//------------------------------------------------------------------------------------------------------------
AsInfo_Panel::~AsInfo_Panel()
{
	delete Dark_Blue;

	if (Logo_Pop_Font != 0)
		DeleteObject(Logo_Pop_Font);

	if (Logo_Corn_Font != 0)
		DeleteObject(Logo_Corn_Font);
}
//------------------------------------------------------------------------------------------------------------
AsInfo_Panel::AsInfo_Panel()
: Logo_Pop_Font{}, Logo_Corn_Font{}, Dark_Blue(nullptr), Extra_Lives_Count(AsConfig::Initial_Life_Count),
  Letter_P(EBrick_Type::Blue, ELetter_Type::P, 214 * AsConfig::Global_Scale + 1, 153 * AsConfig::Global_Scale),
  Letter_G(EBrick_Type::Blue, ELetter_Type::G, 256 * AsConfig::Global_Scale, 153 * AsConfig::Global_Scale),
  Letter_M(EBrick_Type::Blue, ELetter_Type::M, 297 * AsConfig::Global_Scale - 1, 153 * AsConfig::Global_Scale), 
  Floor_Indicator(EMessage_Type::Floor_Is_Over, Score_X + 8, Score_Y + Indicator_Y_Offset), 
  Monster_Indicator(EMessage_Type::Unfreeze_Monsters, Score_X + 90, Score_Y + Indicator_Y_Offset),
  Player_Name_Label(Score_X + 5, Score_Y + 5, Score_Width - 2 * 5, 16, AsConfig::Name_Font),
  Score_Label(Score_X + 5, Score_Y + 5 + Score_Value_Offset, Score_Width - 2 * 5, 16, AsConfig::Score_Font)
{
	const int scale = AsConfig::Global_Scale;

	Logo_Rect.left = 208 * scale;
	Logo_Rect.top = 5 * scale;
	Logo_Rect.right = Logo_Rect.left + 104 * scale;
	Logo_Rect.bottom = Logo_Rect.top + 100 * scale;


	Data_Rect.left = Score_X * scale;
	Data_Rect.top = Score_Y * scale;
	Data_Rect.right = Data_Rect.left + Score_Width * scale;
	Data_Rect.bottom = Data_Rect.top + Score_Height * scale;

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
	Floor_Indicator.Act();
	Monster_Indicator.Act();
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Clear(HDC hdc, RECT &paint_area)
{}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw(HDC hdc, RECT &paint_area)
{
	const wchar_t *pop_str = L"POP"; // задаем строку в 2 байта
	const wchar_t *corn_str = L"CORN";
	AString score_str = L"SCORE:";
	RECT intersection_rect{};

	if (IntersectRect(&intersection_rect, &paint_area, &Logo_Rect) )
	{
		// 1. Logo
		AsTools::Rect(hdc, 211, 5, 104, 100, AsConfig::Blue_Color);

		SelectObject(hdc, Logo_Corn_Font);
		SetBkMode(hdc, TRANSPARENT);

		// 1.1. "POP"
		SelectObject(hdc, Logo_Pop_Font);
		SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());
		TextOut(hdc, (Logo_X_Pos + Shadow_X_Offset) * scale_, (Logo_Y_Pos + Shadow_Y_Offset) * scale_, pop_str, wcslen(pop_str)); // shadow
		SetTextColor(hdc, AsConfig::Red_Color.Get_RGB());
		TextOut(hdc, Logo_X_Pos * scale_, Logo_Y_Pos * scale_, pop_str, 3); // main letters

		// 1.2. "CORN"
		SelectObject(hdc, Logo_Corn_Font);
		SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());
		TextOut(hdc, (Logo_X_Pos + Shadow_X_Offset - 5) * scale_, (Logo_Y_Pos + Shadow_Y_Offset + 44) * scale_, corn_str, 4); // shadow
		SetTextColor(hdc, AsConfig::Red_Color.Get_RGB());
		TextOut(hdc, (Logo_X_Pos - 5) * scale_, (Logo_Y_Pos + 44) * scale_, corn_str, wcslen(corn_str)); // main letters
	}

	// 2. Таблица счета
	if (IntersectRect(&intersection_rect, &paint_area, &Data_Rect))
	{
		// 2.1. Рамка
		AsTools::Rect(hdc, Score_X, Score_Y, Score_Width, 2, AsConfig::Red_Color);
		AsTools::Rect(hdc, Score_X, Score_Y + Score_Height - 2, Score_Width, 2, AsConfig::Red_Color);
		AsTools::Rect(hdc, Score_X, Score_Y, 2, Score_Height, AsConfig::Red_Color);
		AsTools::Rect(hdc, Score_X + Score_Width - 2, Score_Y, 2, Score_Height, AsConfig::Red_Color);

		AsTools::Rect(hdc, Score_X + 2, Score_Y + 2, Score_Width - 4, Score_Height - 4, *Dark_Blue);

		// 2.2. Бордюр
		AsConfig::Highlight_Panel_Color.Select_Pen(hdc);
		MoveToEx(hdc, (Score_X + 2) * scale_, (Score_Y + Score_Height - 2) * scale_, 0);
		LineTo(hdc, (Score_X + 2) * scale_, (Score_Y + 2) * scale_);
		LineTo(hdc, (Score_X + Score_Width - 2) * scale_, (Score_Y + 2) * scale_);

		AsConfig::Shadow_Color.Select_Pen(hdc);
		MoveToEx(hdc, (Score_X + Score_Width - 2) * scale_, (Score_Y + 2) * scale_, 0);
		LineTo(hdc, (Score_X + Score_Width - 2) * scale_, (Score_Y + Score_Height - 2) * scale_);
		LineTo(hdc, (Score_X + 2) * scale_, (Score_Y + Score_Height - 2) * scale_);

		// 2.3. Имя игрока
		AsTools::Rect(hdc, Player_Name_Label.Get_Content_Rect(), AsConfig::Red_Color);


		Player_Name_Label.Set_Content(L"COMPUTER");
		Player_Name_Label.Draw(hdc);
		//Draw_String(hdc, rect, Player_Name, true);


		// 3. Считаем очки
		AsTools::Rect(hdc, Score_Label.Get_Content_Rect(), AsConfig::Red_Color);

		Score_Label.Set_Content(L"SCORE:");
		Score_Label.Set_Append(Score);
		Score_Label.Draw(hdc);

		// 4. Буквы индикаторы
		Letter_P.Draw(hdc, paint_area);
		Letter_G.Draw(hdc, paint_area);
		Letter_M.Draw(hdc, paint_area);

		// 5. Индикаторы
		Floor_Indicator.Draw(hdc, paint_area); // left
		Monster_Indicator.Draw(hdc, paint_area); // right


		// 6. Выыод кол-ва жизней на индикатор
		Show_Extra_Lifes(hdc);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Show_Extra_Lifes(HDC hdc)
{
	int lifes_to_draw = Extra_Lives_Count;

	AsTools::Rect(hdc, Score_X + 27, Score_Y + Indicator_Y_Offset, 56, 30, AsConfig::Teleport_Portal_Color); // middle индикатор

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
	RECT rect{};

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
//void AsInfo_Panel::Draw_String(HDC hdc, RECT &rect, AString &name_str, bool draw_name)
//{
//	int str_left_offset, str_top_offset;
//	const int scale = AsConfig::Global_Scale;
//	SIZE str_size;
//
//	// 1. Выводим прямоугольник фона
//	AsTools::Rect(hdc, rect, AsConfig::Red_Color);
//
//	// 2. Выводим строку
//	if (draw_name)
//		AsConfig::Name_Font.Select(hdc);
//	else
//		SelectObject(hdc, Score_Font);
//
//	SetTextColor(hdc, AsConfig::White_Color.Get_RGB());
//
//	GetTextExtentPoint32(hdc, name_str.Get_Content(), name_str.Get_Length(), &str_size);
//
//	str_left_offset = rect.left + (rect.right - rect.left) / 2 - str_size.cx / 2;
//	str_top_offset = rect.top + (rect.bottom - rect.top) / 2 - str_size.cy / 2 - scale;
//
//	// 2.1. Вывод тени
//	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());
//	TextOut(hdc, str_left_offset + 2 * scale, str_top_offset + 2 * scale, name_str.Get_Content(), name_str.Get_Length() );
//
//	// 2.2. Вывод строки
//	if (draw_name)
//		SetTextColor(hdc, AsConfig::Blue_Color.Get_RGB());
//	else
//		SetTextColor(hdc, AsConfig::White_Color.Get_RGB());
//
//	TextOut(hdc, str_left_offset, str_top_offset, name_str.Get_Content(), name_str.Get_Length() );
//}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Init()
{
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

	//log_font.lfHeight = -48;
	//log_font.lfWeight = 700;
	//log_font.lfOutPrecision = 3;
	//log_font.lfClipPrecision = 2;
	//log_font.lfQuality = 1;
	//log_font.lfPitchAndFamily = 49;
	//wcscpy_s(log_font.lfFaceName, L"Consolas");

	//log_font.lfHeight = -46;
	//Player_Name_Label.Set_Font(CreateFontIndirect(&log_font) );

	// Установка шрифта для счета
	//log_font.lfHeight = -44;
	//Score_Font = CreateFontIndirect(&log_font);

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
void AsInfo_Panel::Update_Score(EScore_Event_Type event_type)
{
	switch (event_type)
	{
	case EScore_Event_Type::Hit_Brick:
		Score += 20;
		break;

	case EScore_Event_Type::Hit_Monster:
		Score += 37;
		break;

	case EScore_Event_Type::Catch_Letter:
		Score += 23;
		break;

	default:
		AsConfig::Throw();
	}

	AsTools::Invalidate_Rect(Data_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Increase_Life_Count()
{
	if (Extra_Lives_Count < AsConfig::Max_Life_Count)
	{
		++Extra_Lives_Count;
		AsTools::Invalidate_Rect(Data_Rect);
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsInfo_Panel::Decrease_Life_Count()
{
	if (Extra_Lives_Count == 0)
		return false;

	--Extra_Lives_Count;
	AsTools::Invalidate_Rect(Data_Rect);

	return true;
}
//------------------------------------------------------------------------------------------------------------