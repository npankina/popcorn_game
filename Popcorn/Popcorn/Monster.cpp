#include "Monster.hpp"

// AExplosive_Ball
AExplosive_Ball::AExplosive_Ball()
: X_Pos(0), Y_Pos(0),Size(0), Step_Count(0)
{}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Act()
{}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Clear(HDC hdc, RECT &paint_area)
{}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Draw(HDC hdc, RECT &paint_area)
{}
//------------------------------------------------------------------------------------------------------------
bool AExplosive_Ball::Is_Finished()
{}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Explode(int x_pos, int y_pos, int size, int step_count)
{
	X_Pos = x_pos;
	Y_Pos = y_pos;
	Size = size; 
	Step_Count = step_count;
}




// AMonster
const double AMonster::Max_Cornea_Height = 11.0;
const double AMonster::Blink_Timeouts[Blink_Stages_Count] = {0.4, 0.3, 0.8, 0.4, 0.4, 0.4, 0.8};
const EEye_State AMonster::Blinks_States[Blink_Stages_Count] = {
	EEye_State::Closed, EEye_State::Opening, EEye_State::Staring, EEye_State::Closing,
						EEye_State::Opening, EEye_State::Staring, EEye_State::Closing};
//------------------------------------------------------------------------------------------------------------
AMonster::AMonster()
: X_Pos(0), Y_Pos(0), Start_Blink_Timeout(0), Total_Animation_Timeout(0), Cornea_Height(Max_Cornea_Height),
  Eye_State(EEye_State::Closed), Monster_State(EMonster_State::Missing), Monster_Rect{}, Blink_Ticks{}
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
{
	int current_tick_offset, prev_tick;
	int i;
	double ratio;

	if (Monster_State == EMonster_State::Missing)
		return;

	current_tick_offset = (AsConfig::Current_Timer_Tick - Start_Blink_Timeout) % Total_Animation_Timeout;

	for (i = 0; i < Blink_Stages_Count; i++)
		if (current_tick_offset < Blink_Ticks[i])
		{
			Eye_State = Blinks_States[i];
			break;
		}

	if (i == 0)
		prev_tick = 0;
	else
		prev_tick = Blink_Ticks[i-1];

	ratio = (double)( (current_tick_offset - prev_tick) / (Blink_Ticks[i] - prev_tick) ); // �������� ����� ������� � ���������� �������� ������� �� 0 �� 1

	switch(Eye_State)
	{
	case EEye_State::Closed:
		Cornea_Height = 0.0;
		break;

	case EEye_State::Opening:
		Cornea_Height = Max_Cornea_Height * ratio;
		break;

	case EEye_State::Staring:
		Cornea_Height = Max_Cornea_Height;
		break;

	case EEye_State::Closing:
		Cornea_Height = Max_Cornea_Height * (1.0 - ratio);
		break;

	default:
		AsConfig::Throw();
	}

	AsTools::Invalidate_Rect(Monster_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT &paint_area)
{}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect{};

	if (! IntersectRect(&intersection_rect, &paint_area, &Monster_Rect) )
		return;

	switch(Monster_State)
	{
	case EMonster_State::Missing:
		break;
		
	case EMonster_State::Alive:
		Draw_Alive(hdc);
		break;

	case EMonster_State::Destroing:
		Draw_Destroing(hdc);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Alive(HDC hdc)
{
	const int scale = AsConfig::Global_Scale;
	RECT rect{}, cornea_rect{};
	HRGN region{};


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
	
	if (Eye_State == EEye_State::Closed)
		return;

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
bool AMonster::Is_Active()
{
	if (Monster_State == EMonster_State::Missing)
		return false;
	return true;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Activate(int x_pos, int y_pos)
{
	const int scale = AsConfig::Global_Scale;
	double current_timeout = 0.0;
	int tick_offset;
	Monster_State = EMonster_State::Alive;

	X_Pos = x_pos + 10;
	Y_Pos = y_pos;

	Monster_Rect.left = X_Pos * scale;
	Monster_Rect.top = Y_Pos * scale;
	Monster_Rect.right = Monster_Rect.left + Width * scale;
	Monster_Rect.bottom = Monster_Rect.top + Height * scale;

	// ������������ ���� ������� ��� �������� ��������

	Start_Blink_Timeout = AsConfig::Current_Timer_Tick;

	for (int i = 0; i < Blink_Stages_Count; i++)
	{
		current_timeout += Blink_Timeouts[i];
		tick_offset = (int)((double)AsConfig::FPS * current_timeout);
		Blink_Ticks[i] = tick_offset;
	}

	Total_Animation_Timeout = tick_offset;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Destroy()
{
	Monster_State == EMonster_State::Destroing;

	Explosive_Balls[0].Explode(Monster_Rect.left + 20, Monster_Rect.right + 20, 30, 10);

}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Destroing(HDC hdc)
{
	for (int i = 0; i < Explosive_Balls_�ount; i++)
		Explosive_Balls[i].Draw();
}