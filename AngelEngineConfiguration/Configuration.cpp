#include"stdafx.h"
#include <windows.h>
#include "Resource.h"
#include"Configuration.h"
#include<string>

#define CHECKBOX_TOGGLE_WINDOWED	1002
#define COMBO_BOX_RESOLUTION		1003
#define BUTTON_FINISH				1004
#define BUTTON_FEATURES				1006
#define COMBO_BOX_MSAA				1005


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static HWND ToggleWindowd;
static HWND ResolutionComboBox;
static HWND MSAAComboBox;
static HWND LabelResolution;
static HWND FinishButton;
static HWND FeaturesButton;
bool AngelCore::AngelConfiguraion::Configuration::m_windowed = false;
int AngelCore::AngelConfiguraion::Configuration::m_selectedResolution = 0;
int AngelCore::AngelConfiguraion::Configuration::m_selectedMultiSampleQuality = 0;
bool AngelCore::AngelConfiguraion::Configuration::m_successed = false;

AngelCore::AngelConfiguraion::Configuration::Configuration()
{

}

AngelCore::AngelConfiguraion::Configuration::~Configuration()
{
	
}

UINT AngelCore::AngelConfiguraion::Configuration::GetSelectedWidth()
{
	if (m_selectedResolution == 0)
	{
		return 640;
	}
	if (m_selectedResolution == 1)
	{
		return 800;
	}
	if (m_selectedResolution == 2)
	{
		return 1024;
	}
	if (m_selectedResolution == 3)
	{
		return 1280;
	}
	if (m_selectedResolution == 4)
	{
		return 1360;
	}
	
	return 1366;
}

UINT AngelCore::AngelConfiguraion::Configuration::GetSelectedHeight()
{
	if (m_selectedResolution == 0)
	{
		return 480;
	}
	if (m_selectedResolution == 1)
	{
		return 600;
	}
	if (m_selectedResolution == 3)
	{
		return 720;
	}
	return 768;
}

UINT AngelCore::AngelConfiguraion::Configuration::GetSelectedMultiSampleQuality()
{
	if (m_selectedMultiSampleQuality == 1)
		return 2;
	if (m_selectedMultiSampleQuality == 2)
		return 4;
	if (m_selectedMultiSampleQuality == 3)
		return 8;

	return 1;
}

HINSTANCE hInst;
void AngelCore::AngelConfiguraion::Configuration::InitializeWindow(HINSTANCE hInstance, int showCmd)
{

	WNDCLASSEX  WndCls;
	static char szAppName[] = "AngelEngineConfiguration";
	MSG         Msg;

	hInst = hInstance;
	WndCls.cbSize = sizeof(WndCls);
	WndCls.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	WndCls.lpfnWndProc = WndProc;
	WndCls.cbClsExtra = 0;
	WndCls.cbWndExtra = 0;
	WndCls.hInstance = hInst;
	WndCls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndCls.lpszMenuName = NULL;
	WndCls.lpszClassName = szAppName;
	WndCls.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassEx(&WndCls);

	CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		szAppName,
		"AngelEngineConfiguration",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		400,
		500,
		NULL,
		NULL,
		hInstance,
		NULL);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

int AngelCore::AngelConfiguraion::Configuration::MessageLoop()
{
	MSG msg;	//Create a new message structure
	ZeroMemory(&msg, sizeof(MSG));
	while (true)	//while there is a message
	{
		//if there was a windows message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)	//if the message was WM_QUIT
				break;	//Exit the message loop
			TranslateMessage(&msg);	//Translate the message
			DispatchMessage(&msg);
		}

		else {
		
			
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK AngelCore::AngelConfiguraion::Configuration::SWndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{
		case CHECKBOX_TOGGLE_WINDOWED:
			m_windowed = IsDlgButtonChecked(hWnd, CHECKBOX_TOGGLE_WINDOWED);
			if (m_windowed) {
				CheckDlgButton(hWnd, CHECKBOX_TOGGLE_WINDOWED, BST_UNCHECKED);
			}
			else {
				CheckDlgButton(hWnd, CHECKBOX_TOGGLE_WINDOWED, BST_CHECKED);
				break;
			}
			break;

		case COMBO_BOX_RESOLUTION:
			switch (HIWORD(wParam)) // Find out what message it was
			{
			case CBN_SELCHANGE:
				m_selectedResolution = SendMessage(ResolutionComboBox, CB_GETCURSEL, 0, 0);
				break;
			}
			break;

		case COMBO_BOX_MSAA:
			switch (HIWORD(wParam)) // Find out what message it was
			{
			case CBN_SELCHANGE:
				m_selectedMultiSampleQuality = SendMessage(MSAAComboBox, CB_GETCURSEL, 0, 0);
				break;
			}
			break;

		case BUTTON_FINISH:
			m_successed = true;
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;

		case BUTTON_FEATURES:
			MessageBox(NULL, "-VR Ready \n-Support for physics with bullet Physics\n-Scripting with AngelScript\n-Fully component based\n-Direct3D 11.1 Support\n-AngelGUI2D for 2d gui rendering\n-AngelGUI3D for 3d gui rendering\n-Terrain editor\n-Support for animation and skinned meshes\n \tand many more...!\nFor full source code visit : https://github.com/PeymanTahghighi/AngelEngine ", "Features!", MB_OK);
			break;
		}

	case WM_SIZE:

		break;
		// Check if the window is being closed.
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	HDC hDC, MemDCExercising;
	PAINTSTRUCT Ps;
	HBITMAP bmpExercising;
	
	const char *ListOfResolutions[] = { "640 X 480", "800 X 600", "1024 X 768",
		"1280 X 720", "1360 X 768" , "1366 X 768" };

	const char *ListOfMSAA[] = { "Off", "2X", "4X",
		"8X" };
	switch (umessage)
	{
	case WM_CREATE:
		
		FinishButton = CreateWindowW(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Play",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			170,         // x position 
			420,         // y position 
			50,        // Button width
			20,        // Button height
			hwnd,     // Parent window
			(HMENU)BUTTON_FINISH,       // No menu.
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		FinishButton = CreateWindowW(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Angel Engine Features",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			110,         // x position 
			350,         // y position 
			180,        // Button width
			20,        // Button height
			hwnd,     // Parent window
			(HMENU)BUTTON_FEATURES,       // No menu.
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		ToggleWindowd = CreateWindowW(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Windowed",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX,  // Styles 
			250,         // x position 
			230,         // y position 
			100,        // Button width
			20,        // Button height
			hwnd,     // Parent window
			(HMENU)CHECKBOX_TOGGLE_WINDOWED,       // No menu.
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		ResolutionComboBox = CreateWindow("COMBOBOX",
			NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN|WS_VSCROLL,
			80, 230, 136, 60,
			hwnd,
			(HMENU)COMBO_BOX_RESOLUTION,
			hInst,
			NULL);

		CheckDlgButton(hwnd, CHECKBOX_TOGGLE_WINDOWED, BST_CHECKED);

		MSAAComboBox = CreateWindow("COMBOBOX",
			NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
			80, 280, 136, 60,
			hwnd,
			(HMENU)COMBO_BOX_MSAA,
			hInst,
			NULL);

		for (int i = 0;i < ARRAYSIZE(ListOfResolutions);i++)
		{
			SendMessage(ResolutionComboBox,
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>((LPCTSTR)ListOfResolutions[i]));
		}

		for (int i = 0;i < ARRAYSIZE(ListOfMSAA);i++)
		{
			SendMessage(MSAAComboBox,
				CB_ADDSTRING,
				0,
				reinterpret_cast<LPARAM>((LPCTSTR)ListOfMSAA[i]));
		}

		SendMessage(ResolutionComboBox, CB_SETCURSEL, 0, 0);
		SendMessage(MSAAComboBox, CB_SETCURSEL, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &Ps);

		RECT rec;

		SetRect(&rec, 0, 232,
			100, 350);
		DrawTextW(hDC, L"Resolution:", strlen("Resolution:"), &rec, DT_TOP | DT_LEFT);

		SetRect(&rec, 0, 282,
			100, 299);
		DrawTextW(hDC, L"MSAA:", strlen("MSAA:"), &rec, DT_TOP | DT_LEFT);

		// Load the bitmap from the resource
		bmpExercising = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ANGELLOGO));
		// Create a memory device compatible with the above DC variable
		MemDCExercising = CreateCompatibleDC(hDC);
		// Select the new bitmap
		SelectObject(MemDCExercising, bmpExercising);

		// Copy the bits from the memory DC into the current dc
		BitBlt(hDC, 10, 10, 450, 200, MemDCExercising, 0, 0, SRCCOPY);

		// Restore the old bitmap
		DeleteDC(MemDCExercising);
		DeleteObject(bmpExercising);
		EndPaint(hwnd, &Ps);
		break;
	default:
		break;
	}
	return AngelCore::AngelConfiguraion::Configuration::SWndProc(hwnd, umessage, wparam, lparam);
}