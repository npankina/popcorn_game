#include "Monster.hpp"

const double AMonster::Max_Cornea_Height = 11.0;
const double AMonster::Blink_Timeouts[Blink_Stages_Count] = {0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 0.5};
const EEye_State AMonster::Blinks_States[Blink_Stages_Count] = {
	EEye_State::Closed, EEye_State::Opening, EEye_State::Staring, EEye_State::Closing,
						EEye_State::Opening, EEye_State::Staring, EEye_State::Closing};
//------------------------------------------------------------------------------------------------------------
AMonster::AMonster()
: X_Pos(0), Y_Pos(0), Start_Blink_Timeout(0), Cornea_Height(Max_Cornea_Height), Is_Active(false), Monster_Rect{}, 
  Eye_State(EEye_State::Closed), Blink_Ticks{}
{}
//------------------------------------------------------------------------------------------------------------
void AMonster::Begin_Movement()
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Finish_Movement()
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Advance(double max_speed)
{
}
//------------------------------------------------------------------------------------------------------------
double AMonster::Get_Speed()
{
	return 0.0;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act()
{}
//------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT &paint_area)
{}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	RECT intersection_rect{}, rect{}, cornea_rect{};
	HRGN region{};

	if (!Is_Active)
		return;

	if (! IntersectRect(&intersection_rect, &paint_area, &Monster_Rect) )
		return;

	// 1. ������ ���
	// 1.1. ����������� ������ ����
	rect = Monster_Rect;
	++rect.right;
	++rect.bottom;

	// 1.2. ������ ���
	AsTools::Ellipse(hdc, Monster_Rect, AsConfig::Monster_Dark_Red_Color);

 	region = CreateEllipticRgnIndirect(&rect);
	SelectClipRgn(hdc, region); // ���������� ������� �������

	// 1.3. ���������
	rect.left -= 2 * scale;
	rect.top -= 3 * scale;
	rect.right -= 2 * scale;
	rect.bottom -= 3 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	// ������ ����
	// 2.1. ��������
	cornea_rect = Monster_Rect;

	cornea_rect.left += scale;
	cornea_rect.top += 2 * scale + (int)( (Max_Cornea_Height / 2.0 - Cornea_Height / 2.0) * AsConfig::D_Global_Scale);
	cornea_rect.right -= scale;
	cornea_rect.bottom = cornea_rect.top + Cornea_Height * scale;

	// 2.3. ������������ ����� ���������� �����
	region = CreateEllipticRgnIndirect(&cornea_rect);
	SelectClipRgn(hdc, region);

	AsTools::Ellipse(hdc, cornea_rect, AsConfig::Monster_Cornea_Color);
	
	// 2.3. �������
	rect = Monster_Rect;

	rect.left += 4 * scale;
	rect.top += 4 * scale;
	rect.right -= 4 * scale;
	rect.bottom -= 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Monster_Iris_Color);

	// 2.4. ������
	rect = Monster_Rect;

	rect.left += 7 * scale;
	rect.top += 6 * scale;
	rect.right = rect.left + 2 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsTools::Rect(hdc, rect, AsConfig::BG_Color);

	SelectClipRgn(hdc, 0); // ������ ������� �������
	DeleteObject(region); // �������� ������� �������

	// 2.5. ������� ��������
	AsConfig::BG_Outcome_Color.Select(hdc);

	Arc(hdc, cornea_rect.left, cornea_rect.top, cornea_rect.right - 1, cornea_rect.bottom - 1, 0, 0, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Activate(int x_pos, int y_pos)
{
	const int scale = AsConfig::Global_Scale;
	double current_timeout = 0.0;
	int tick_offset;
	Is_Active = true;

	X_Pos = x_pos;
	Y_Pos = y_pos;

	Monster_Rect.left = X_Pos * scale;
	Monster_Rect.top = Y_Pos * scale;
	Monster_Rect.right = Monster_Rect.left + Width * scale;
	Monster_Rect.bottom = Monster_Rect.top + Height * scale;

	// ������������ ���� ������� ��� �������� ��������

	Start_Blink_Timeout;
	for (int i = 0; i < Blink_Stages_Count; i++)
	{
		current_timeout += Blink_Timeouts[i];
		tick_offset = (int)((double)AsConfig::FPS * current_timeout);
		Blink_Ticks[i] = tick_offset;
	}
}
