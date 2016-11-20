#pragma once
#include<Windows.h>

#ifdef ANGELENGINECONFIGURATION_EXPORTS
#define ANGELENGINECONFIGURATION_API __declspec(dllexport) 
#else
#define ANGELENGINECONFIGURATION_API __declspec(dllimport) 
#endif

namespace AngelCore
{
	namespace AngelConfiguraion
	{
		class Configuration
		{

		public:

			Configuration();

			~Configuration();

			static ANGELENGINECONFIGURATION_API void InitializeWindow(HINSTANCE hInstance, int ShowWnd);

			static ANGELENGINECONFIGURATION_API LRESULT CALLBACK SWndProc(HWND hWnd,
				UINT msg,
				WPARAM wParam,
				LPARAM lParam);

			static ANGELENGINECONFIGURATION_API UINT GetSelectedWidth();
			static ANGELENGINECONFIGURATION_API UINT GetSelectedHeight();
			static ANGELENGINECONFIGURATION_API UINT GetSelectedMultiSampleQuality();
			static ANGELENGINECONFIGURATION_API bool GetSuccessed() { return m_successed; }
		public:
			static bool ANGELENGINECONFIGURATION_API GetWindowed()	{ return m_windowed; }

		private:
			static bool		m_windowed;
			float			m_width;
			float			m_height;
			static int		m_selectedResolution;
			static int		m_selectedMultiSampleQuality;
			static bool		m_successed;
		private:
			int MessageLoop();
		};
	}
}