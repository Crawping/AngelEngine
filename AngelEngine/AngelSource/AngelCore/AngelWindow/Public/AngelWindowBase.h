#pragma once
#include<Windows.h>
#include"../../AngelSubSystem/Public/SubSystemManager.h"
#include"../../../AngelCore/AngelSubSystem/AngelRendering/Public/DefferedRenderTexture.h"
#include"../../AngelSubSystem/AngelRendering/Public/RenderTexture.h"

namespace AngelCore
{
	namespace AngelWindow
	{
		class AngelWindowBase : public ComBase<QiList<IUnknown> >
		{

		public:

			AngelWindowBase();

			~AngelWindowBase();

			bool InitializeWindow(HINSTANCE hInstance, int ShowWnd);

			static LRESULT CALLBACK SWndProc(HWND hWnd,
				UINT msg,
				WPARAM wParam,
				LPARAM lParam);

		protected:

			virtual bool Initialize() = 0;
			
		public:
#pragma region Setter

			void SetWindowName(LPCTSTR Name)					{ this->m_windowName = Name; }

			void SetHeight(int _Height)							{ this->m_height = _Height; }

			void SetWidth(int _width)							{ this->m_width = _width; }

			void SetWindowed(bool _windowed)					{ this->m_windowed = _windowed; }

			void ShowCurser(bool _curser)						{ this->m_showCurser = _curser; }

			void SetHWND(HWND h) { this->m_hwnd = h; }

#pragma endregion

#pragma region Getter

			LPCTSTR GetWindowName() const						{ return this->m_windowName; };

			int GetHeight() const								{ return this->m_height; }

			int GetWidth() const								{ return this->m_width; }

			bool GetWindowed() const							{ return this->m_windowed; }

			HWND GetHwnd() const								{ return this->m_hwnd; }

			bool GetShowCurser() const							{ return this->m_showCurser; }

#pragma endregion

		private:
			LPCTSTR															m_windowName;
			HWND															m_hwnd;
			int																m_width;
			int																m_height;
			bool															m_windowed;
			bool															m_showCurser;
			static AngelSubSystem::SubSystemManager *						m_subSystem;
			AngelRendering::RenderTexture									m_DOF;
		private:

			void Update();
			void Render();
			int MessageLoop();
		};
	}
}
