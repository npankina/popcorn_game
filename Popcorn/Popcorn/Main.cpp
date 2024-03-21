// Popcorn.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Main.h"

// Global Variables:
AsMain_Window Main_Window;
//------------------------------------------------------------------------------------------------------------




// class AsFrame_DC
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
		if (Bitmap != 0)
			DeleteObject(Bitmap);

		if (Hdc != 0)
			DeleteObject(Hdc);

		Width = dc_width;
		Height = dc_height;

		Hdc = CreateCompatibleDC(hdc);
		Bitmap = CreateCompatibleBitmap(hdc, Width, Height);
		SelectObject(Hdc, Bitmap);

		AsTools::Rect(Hdc, rect, AsConfig::BG_Color);
	}

	return Hdc;
}
//------------------------------------------------------------------------------------------------------------
AsFrame_DC::~AsFrame_DC()
{
	if (Bitmap != 0)
		DeleteObject(Bitmap);

	if (Hdc != 0)
		DeleteObject(Hdc);
}
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	return Main_Window.Main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
//------------------------------------------------------------------------------------------------------------




// class AsMain_Window
AsMain_Window *AsMain_Window::Self = 0;
//------------------------------------------------------------------------------------------------------------
AsMain_Window::AsMain_Window()
: Instance(0), szTitle{}, szWindowClass{}
{
	Self = this;
}
//------------------------------------------------------------------------------------------------------------
int APIENTRY AsMain_Window::Main(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR command_line, int command_show)
{
	MSG msg;
	HACCEL accel_table;

	Instance = instance; // Store instance handle in our global variable

	UNREFERENCED_PARAMETER(prev_instance);
	UNREFERENCED_PARAMETER(command_line);

	// Initialize global strings
	LoadStringW(instance, IDS_APP_TITLE, szTitle, MAX_String_Size);
	LoadStringW(instance, IDC_POPCORN, szWindowClass, MAX_String_Size);
	Register_Class();

	// Perform application initialization:
	if ( !Init_Instance(command_show))
		return FALSE;

	accel_table = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_POPCORN));

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, accel_table, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
//------------------------------------------------------------------------------------------------------------
ATOM AsMain_Window::Register_Class()
{ // Registers the window class.

	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Window_Proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = Instance;
	wcex.hIcon = LoadIcon(Instance, MAKEINTRESOURCE(IDI_POPCORN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = AsConfig::BG_Color.Get_Brush(); // цвет фона игры
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_POPCORN);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
//------------------------------------------------------------------------------------------------------------
BOOL AsMain_Window::Init_Instance(int command_show)
{ // Saves instance handle and creates main window

	RECT window_rect{};
	HWND hwnd;

	window_rect.left = 0;
	window_rect.top = 0;
	window_rect.right = 320 * 3;
	window_rect.bottom = 200 * 3;

	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, TRUE);

	hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0, 0, Instance, 0);

	if (hwnd == 0)
		return FALSE;

	Engine.Init_Engine(hwnd);

	ShowWindow(hwnd, command_show);
	UpdateWindow(hwnd);

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------
void AsMain_Window::On_Paint(HWND hwnd)
{
	HDC hdc, frame_dc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);
	frame_dc = Frame_DC.Get_DC(hwnd, hdc);

	Engine.Draw_Frame(frame_dc, ps.rcPaint); // рисование теперь происходит в новый контекст устройства (наш буфер)
	BitBlt(hdc, 0, 0, Frame_DC.Width, Frame_DC.Height, frame_dc, 0, 0, SRCCOPY); // скопировали пиксели (рисунок) из нашего буфера в windows

	EndPaint(hwnd, &ps);
}
//------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK AsMain_Window::Window_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{ // Processes messages for the main window.

	int wmId;
	
	switch (message)
	{
	case WM_COMMAND:

		wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(Self->Instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;


	case WM_PAINT:

		Self->On_Paint(hWnd);
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			return Self->Engine.On_Key(EKey_Type::Left, true);

		case VK_RIGHT:
			return Self->Engine.On_Key(EKey_Type::Right, true);

		case VK_SPACE:
			return Self->Engine.On_Key(EKey_Type::Space, true);
		}
		break;


	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:
			return Self->Engine.On_Key(EKey_Type::Left, false);

		case VK_RIGHT:
			return Self->Engine.On_Key(EKey_Type::Right, false);

		case VK_SPACE:
			return Self->Engine.On_Key(EKey_Type::Space, false);
		}
		break;


	case WM_TIMER:
		if (wParam == Timer_ID)
			return Self->Engine.On_Timer();
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------
// Message handler for about box.
INT_PTR CALLBACK AsMain_Window::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;


	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK or LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}
//------------------------------------------------------------------------------------------------------------
