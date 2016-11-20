#include<windowsx.h>
#include"../Public/AngelWindowBase.h"
#include"../../AngelSubSystem/AngelLog/Public/AngelLog.h"
#include"../../AngelSubSystem/Public/RenderManager.h"
#include"../../AngelSubSystem/Public/TimeManager.h"
#include <stdlib.h>
#include <stdio.h>
#include<tchar.h>
#include"../../AngelSubSystem/Public/InputManager.h"
#include"../../AngelSubSystem/Public/WorldManager.h"
#include"../../AngelSubSystem/AngelRendering/Public/LightManager.h"
#include"../../AngelSubSystem/AngelRendering/Public/PostProcess.h"
#include<AntTweakBar.h>
#include"../../AngelSubSystem/AngelRendering/Public/TerrainManager.h"
#include"../../AngelSubSystem/Public/PhysicManager.h"
#include"../../AngelSubSystem/Public/LevelManager.h"
#include"../../AngelSubSystem/Public/MediaManager.h"
#include"../../AngelSubSystem/Public/VRManager.h"
#include<Configuration.h>
#include<Source\Include.h>//AngelGUI
#include"../../../../resource.h"
using namespace DirectX;

AngelCore::AngelSubSystem::SubSystemManager * AngelCore::AngelWindow::AngelWindowBase::m_subSystem = nullptr;
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

AngelCore::AngelWindow::AngelWindowBase::AngelWindowBase() : m_windowed(true) , m_showCurser(true) , m_width(800)
															,m_height(600) , m_hwnd(NULL) , m_windowName(L"Unnamed")
{

}

AngelCore::AngelWindow::AngelWindowBase::~AngelWindowBase()
{
	if (this->m_subSystem)
	{
		if (!this->m_subSystem->ShutDownEngineSubSystem())
		{
			MessageBoxW(NULL, L"ShutDown Engine Failed", L"Failed", MB_OK | MB_ICONERROR);
		}
		delete this->m_subSystem;
	}
}

bool AngelCore::AngelWindow::AngelWindowBase::InitializeWindow(HINSTANCE hInstance, int showCmd)
{
	//show config
	AngelConfiguraion::Configuration::InitializeWindow(hInstance, showCmd);
	if (!AngelConfiguraion::Configuration::GetSuccessed())
		PostQuitMessage(0);

	m_width = AngelConfiguraion::Configuration::GetSelectedWidth();
	m_height = AngelConfiguraion::Configuration::GetSelectedHeight();
	m_windowed = !AngelConfiguraion::Configuration::GetWindowed();
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::SetSampleCount(
		AngelConfiguraion::Configuration::GetSelectedMultiSampleQuality());
	
#pragma region Create Window
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = HINST_THISCOMPONENT;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, NULL);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszClassName = this->m_windowName;
	wc.lpszMenuName = MAKEINTRESOURCE(1);


	if (!RegisterClassEx(&wc))
	{
		MessageBoxW(NULL, L"Registering Window Failed", L"Failed", MB_OK);
		return false;
	}

	if (!this->m_windowed)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)this->m_width;
		dmScreenSettings.dmPelsHeight = (unsigned long)this->m_height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
		this->m_hwnd = CreateWindowEx(	//Create our Extended Window
			WS_EX_APPWINDOW,	//Extended style
			this->m_windowName,	//Name of our windows class
			this->m_windowName,	//Name in the title bar of our window
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,	//style of our window
			posX, posY,	//Top left corner of window
			this->m_width,	//Width of our window
			this->m_height,	//Height of our window
			NULL,	//Handle to parent window
			NULL,	//Handle to a Menu
			hInstance,	//Specifies instance of current program
			NULL	//used for an MDI client window
			);
	}
	else
	{
		RECT R = { 0, 0, m_width, m_height };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;
		posX = (GetSystemMetrics(SM_CXSCREEN) - this->m_width) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - this->m_height) / 2;
		std::wstring mMainWndCaption;
		this->m_hwnd = CreateWindowW(this->m_windowName, mMainWndCaption.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0);

	}

	if (!this->m_hwnd)
	{
		MessageBoxW(NULL, L"Creating Window Failed", L"Failed", MB_OK);
		return false;
	}

	ShowWindow(this->m_hwnd, SW_SHOW);
	//SetFocus(this->m_hwnd);
	UpdateWindow(this->m_hwnd);

	///ShowCursor(this->m_showCurser);
#pragma endregion

#pragma region SubSystemInitialize

	AngelSubSystemResources::WindowProperties::SetHeight(this->m_height);
	AngelSubSystemResources::WindowProperties::SetWidth(this->m_width);
	AngelSubSystemResources::WindowProperties::SetHWND(this->m_hwnd);
	AngelSubSystemResources::WindowProperties::SetHINSTANCE(hInstance);

	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera = new BaseCamera();
	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->UpdateView
		(DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->RefreshProjection();

	
	 this->m_subSystem = AngelSubSystem::SubSystemManager::GetInstance();

	 if (!this->m_subSystem->StartUpEngineSubSystem())
	 {
		 AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Error Initialize Engine");
		 MessageBoxW(NULL, L"StartUp Engine Failed", L"Failed", MB_OK | MB_ICONERROR);
		 return false;
	 }
#pragma endregion

#pragma region Initialize Game Scene
	if (!Initialize())
	{
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Error Initialize Game Scene");
		MessageBoxW(NULL, L"Initializing Game Failed!", L"Failed", MB_ICONERROR | MB_OK);
		return false;
	}
	AngelRendering::LightManager::GetInstance()->Initialize();
	AngelRendering::TerrainManager::GetInstance()->Initialize();
	AngelRendering::PostProcess::GetInstance()->Initialize(
		AngelSubSystemResources::GraphicDeviceResources::Device.Get());

	//Set default texture path for resource loading.
	std::wstring curPath = m_subSystem->GetFileSystemManager()->GetContentDirectory();
	curPath += L"\\AngelData\\Texture";
	AngelGUISystem::AngelGUI::SetTextureFolderPath(curPath);

	//Load main level of this program.
	m_subSystem->GetLevelManager()->LoadLevel("sampleMain");

	//We fix frame rat to 30FPS
#ifndef _DEBUG
	this->m_subSystem->GetTimeManager()->SetFixedTimeStep(true);
	this->m_subSystem->GetTimeManager()->SetTargetElapsedSeconds(0.033f);
#endif

#pragma endregion

	MessageLoop();
	return true;
}

int AngelCore::AngelWindow::AngelWindowBase::MessageLoop()
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
			this->m_subSystem->GetTimeManager()->Tick([&, this]
			{
				this->m_subSystem->GetInputManager()->DetectInput();
				Update();
				Render();
			});
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK AngelCore::AngelWindow::AngelWindowBase::SWndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		AngelSubSystemResources::WindowProperties::SetHeight(HIWORD(lParam));
		AngelSubSystemResources::WindowProperties::SetWidth(LOWORD(lParam));
		//should be called before resize
		AngelGUISystem::AngelGUI::PreResize();
		if(m_subSystem)
			m_subSystem->GetRenderManager()->OnResize();
		AngelGUISystem::AngelGUI::Resize(AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChain.Get());

		break;
		// Check if the window is being closed.
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void AngelCore::AngelWindow::AngelWindowBase::Update()
{
	if(m_subSystem->GetVRManager()->GetEnable())
		this->m_subSystem->GetWorldManager()->Update();
	this->m_subSystem->GetActorFactory()->Update(*this->m_subSystem->GetTimeManager());
	if (!m_subSystem->GetVRManager()->GetEnable())
		this->m_subSystem->GetWorldManager()->Update();
	this->m_subSystem->GetPhysicManager()->Update();
	if(AngelRendering::TerrainManager::GetInstance()->GetEnable())
		AngelRendering::TerrainManager::GetInstance()->Update();
	
	AngelGUISystem::AngelGUI::UpdateGUI2D(m_subSystem->GetTimeManager()->GetElapsedSeconds());
	AngelGUISystem::AngelGUI::UpdateGUI3D(m_subSystem->GetTimeManager()->GetElapsedSeconds()
		, AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView(),
		AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection());
}

void AngelCore::AngelWindow::AngelWindowBase::Render()
{
	if (!m_subSystem->GetVRManager()->GetEnable())
	{
		AngelGUISystem::AngelGUI::SetUsingAddtionalRenderTarget(false);
		AngelGUISystem::AngelGUI::SetRenderMouse(false);
		AngelRendering::LightManager::GetInstance()->RenderLightsShadowMap();
		
		AngelRendering::PostProcess::GetInstance()->
			RenderCurrentFrameToTexture(AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D().Get());
		if (AngelRendering::TerrainManager::GetInstance()->GetEnable())
			AngelRendering::TerrainManager::GetInstance()->Render();

		this->m_subSystem->GetWorldManager()->Render();
		this->m_subSystem->GetActorFactory()->Render();
		

		float blendFactor[] = { 0.75f, 0.0f, 0.75f, 0.0f };
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->OMSetBlendState
			(AngelSubSystemResources::GraphicDeviceResources::TransparentBlending.Get(), blendFactor, 0xffffffff);
		AngelGUISystem::AngelGUI::RenderGUI3D();
		
		this->m_subSystem->GetRenderManager()->RenderFrame();
		AngelRendering::PostProcess::GetInstance()->ProcessTechniques();
		AngelRendering::PostProcess::GetInstance()->RenderFinalImage();
		AngelGUISystem::AngelGUI::RenderGUI2D();
		if (AngelRendering::TerrainManager::GetInstance()->GetEnable())
			TwDraw();

		this->m_subSystem->GetRenderManager()->PresentScene();
	}
	else
		//render on vr.
	{
		AngelGUISystem::AngelGUI::SetUsingAddtionalRenderTarget(true);
		AngelGUISystem::AngelGUI::SetRenderMouse(true);
		m_subSystem->GetVRManager()->Render();
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	if (TwEventWin(hwnd, umessage, wparam, lparam))
	{
		if (AngelCore::AngelRendering::TerrainManager::GetInstance()->GetEnable())
		{
			return 0;
		}
	}

	switch (umessage)
	{
	case WM_MOUSEWHEEL:
	{
		// Retrieve the lines-to-scroll or characters-to-scroll user setting,
		// using a default value if the API failed.
		UINT userSetting;
		BOOL success = SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &userSetting, 0);
		if (success == FALSE)
			userSetting = 1;

		// Set x,y scroll difference,
		// depending on whether horizontal or vertical scroll.
		float zDelta = GET_WHEEL_DELTA_WPARAM(wparam);
		float yScroll = (zDelta / WHEEL_DELTA) * userSetting;
		float xScroll = 0;
		if (umessage == WM_MOUSEHWHEEL)
		{
			xScroll = -yScroll;
			yScroll = 0;
		}

		AngelGUISystem::AngelGUI::OnMouseScroll(xScroll, yScroll);
	}
	break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
			if (MessageBoxW(NULL, L"Are you Sure?", L"Confirmation", MB_YESNO) == IDYES)
			{
				PostQuitMessage(0);
				return 0;
			}
		if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetInitialize())
			AngelGUISystem::AngelGUI::OnKeyPress(static_cast<UINT>(wparam));
		break;

	case WM_CHAR:
		if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetInitialize())
			AngelGUISystem::AngelGUI::OnKeyCharacter(static_cast<UINT>(wparam));
		break;

	case WM_MOUSEMOVE:
		static LPRECT  rect = new tagRECT();
		if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetInitialize())
		{
			AngelCore::AngelSubSystem::inputManager->SetMousePosition(
				DirectX::XMFLOAT2(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)));
			static float x = GET_X_LPARAM(lparam);
			static float y = GET_Y_LPARAM(lparam);
			if (!AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->GetEnable())
				AngelGUISystem::AngelGUI::OnMouseMove(float(GET_X_LPARAM(lparam)), float(GET_Y_LPARAM(lparam)));
			else
			{
				AngelGUISystem::AngelGUI::OnMouseMove(float(GET_X_LPARAM(lparam))
					*1182.0f / (AngelCore::AngelSubSystemResources::WindowProperties::GetWidth())
					, float(GET_Y_LPARAM(lparam)
						*(1464.0f / AngelCore::AngelSubSystemResources::WindowProperties::GetHeight())));
			}
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetInitialize())
			AngelCore::AngelSubSystem::inputManager->SetRightClicked(true);
		if (!AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->GetEnable())
			AngelGUISystem::AngelGUI::OnMousePress(umessage,
				float(GET_X_LPARAM(lparam)), float(GET_Y_LPARAM(lparam)));
		else
		{
			AngelGUISystem::AngelGUI::OnMousePress(umessage,
				float(GET_X_LPARAM(lparam))
				*1182.0f / (AngelCore::AngelSubSystemResources::WindowProperties::GetWidth())
				, float(GET_Y_LPARAM(lparam)
					*(1464.0f / AngelCore::AngelSubSystemResources::WindowProperties::GetHeight())));
		}
		break;

	case WM_LBUTTONDOWN:
		if (!AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->GetEnable())
			AngelGUISystem::AngelGUI::OnMousePress(umessage,
				float(GET_X_LPARAM(lparam)), float(GET_Y_LPARAM(lparam)));
		else
		{
			AngelGUISystem::AngelGUI::OnMousePress(umessage,
				float(GET_X_LPARAM(lparam))
				*1182.0f / (AngelCore::AngelSubSystemResources::WindowProperties::GetWidth())
				, float(GET_Y_LPARAM(lparam)
					*(1464.0f / AngelCore::AngelSubSystemResources::WindowProperties::GetHeight())));
		}
		if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetInitialize())
			AngelCore::AngelSubSystem::inputManager->SetLeftClicked(true);
		break;

	
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		if (!AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->GetEnable())
			AngelGUISystem::AngelGUI::OnMouseRelease(umessage,
				float(GET_X_LPARAM(lparam)), float(GET_Y_LPARAM(lparam)));
		else
		{
			AngelGUISystem::AngelGUI::OnMouseRelease(umessage,
				float(GET_X_LPARAM(lparam))
				*1182.0f / (AngelCore::AngelSubSystemResources::WindowProperties::GetWidth())
				, float(GET_Y_LPARAM(lparam)
					*(1464.0f / AngelCore::AngelSubSystemResources::WindowProperties::GetHeight())));
		}
		if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetInitialize())
			AngelCore::AngelSubSystem::inputManager->SetRightClicked(false);
		break;

	case WM_LBUTTONUP:
		if (!AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->GetEnable())
			AngelGUISystem::AngelGUI::OnMouseRelease(umessage,
				float(GET_X_LPARAM(lparam)), float(GET_Y_LPARAM(lparam)));
		else
		{
			AngelGUISystem::AngelGUI::OnMouseRelease(umessage,
				float(GET_X_LPARAM(lparam))
				*1182.0f / (AngelCore::AngelSubSystemResources::WindowProperties::GetWidth())
				, float(GET_Y_LPARAM(lparam)
					*(1464.0f / AngelCore::AngelSubSystemResources::WindowProperties::GetHeight())));
		}
		if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetInitialize())
			AngelCore::AngelSubSystem::inputManager->SetLeftClicked(false);
		break;

	
	// Check if the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	// Check if the window is being closed.
	case WM_CLOSE:
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->SetInitialize(false);
		PostQuitMessage(0);
		return 0;

	//case WM_SETFOCUS:
	//	// Forward focus to the text editor.
	//	if (AngelGUI::GetInstance() != NULL)
	//		SetFocus(AngelGUI::GetInstance()->GetHwnd());
	//	break;


	// All other messages pass to the message handler in the system class.
	default:
		return AngelCore::AngelWindow::AngelWindowBase::SWndProc(hwnd, umessage, wparam, lparam);
	}
}
