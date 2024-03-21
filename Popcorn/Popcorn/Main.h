#pragma once

#include "resource.h"
#include "Engine.h"

//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	AsFrame_DC() : Width(0), Height(0), Hdc(0), Bitmap(0) {};
	~AsFrame_DC();
	HDC Get_DC(HWND hwnd, HDC hdc);

	int Width, Height;

private:
	HDC Hdc;
	HBITMAP Bitmap;
};
//------------------------------------------------------------------------------------------------------------
class AsMain_Window
{
public:
	AsMain_Window();
	int APIENTRY Main(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR command_line, int command_show);

private:
	ATOM Register_Class();
	BOOL Init_Instance(int command_show);
	void On_Paint(HWND hwnd);

	static LRESULT CALLBACK Window_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static AsMain_Window *Self;

	static const int MAX_String_Size = 100;

	AsFrame_DC Frame_DC;
	AsEngine Engine;
	HINSTANCE Instance;                                // current instance
	WCHAR szTitle[MAX_String_Size];                  // The title bar text
	WCHAR szWindowClass[MAX_String_Size];            // the main window class name
};
//------------------------------------------------------------------------------------------------------------