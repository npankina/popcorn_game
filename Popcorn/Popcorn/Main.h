#pragma once

#include "resource.h"
#include "Engine.h"

//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	AsFrame_DC() : Width(0), Height(0), Frame_DC(0), Frame_Bitmap(0) {};
	HDC Get_DC(HWND hwnd, HDC hdc);

	int Width, Height;

private:
	HDC Frame_DC;
	HBITMAP Frame_Bitmap;
};
//------------------------------------------------------------------------------------------------------------
HDC AsFrame_DC::Get_DC(HWND hwnd, HDC hdc)
{
	int dc_width, dc_height;
	RECT rect{};

	GetClientRect(hwnd, &rect);
	dc_width = rect.right - rect.left;
	dc_height = rect.bottom - rect.top;

	if (dc_width != Width and dc_height != Height)
	{
		if (Frame_Bitmap != 0)
			DeleteObject(Frame_Bitmap);

		if (Frame_DC != 0)
			DeleteObject(Frame_DC);

		Width = dc_width;
		Height = dc_height;

		Frame_DC = CreateCompatibleDC(hdc);
		Frame_Bitmap = CreateCompatibleBitmap(hdc, Width, Height);
		SelectObject(Frame_DC, Frame_Bitmap);
	}

	return Frame_DC;
}