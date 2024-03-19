#pragma once

#include "resource.h"
#include "Engine.h"

//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	AsFrame_DC() : Width(0), Height(0), Frame_DC(0), Frame_Bitmap(0) {};
	~AsFrame_DC();
	HDC Get_DC(HWND hwnd, HDC hdc);

	int Width, Height;

private:
	HDC Frame_DC;
	HBITMAP Frame_Bitmap;
};
//------------------------------------------------------------------------------------------------------------