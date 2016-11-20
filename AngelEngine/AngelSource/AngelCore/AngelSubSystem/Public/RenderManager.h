#pragma once


#include<d3d11_1.h>
#include<dxgi.h>
#include<DirectXMath.h>
#include<wrl\client.h>
#include<dxgi1_3.h>
#include"SubSystemBase.h"

#define DEFAULT_DPI 96

namespace AngelCore
{
	namespace AngelSubSystem
	{
		class RenderManager : public ISubSystem
		{
		public:

			bool StartUp(...) override;

			bool ShutDown() override;

			bool InitVR(int vpW, int vpH, const LUID* pLuid, bool windowed = true);

			~RenderManager();

			void OnResize();

			void SetAndClearRenderTarget(ID3D11RenderTargetView * rendertarget,
				ID3D11DepthStencilView * dsv, float R = 0, float G = 0, float B = 0, float A = 0);

			void SetViewport(float vpX, float vpY, float vpW, float vpH);

		private:

			D3D_FEATURE_LEVEL 								m_suppurtedFeatureLevel;
			Microsoft::WRL::ComPtr<IDXGIFactory2>			m_dxgiFactory;
			//All post process techniques to use inside script.
			bool		m_enableMotionBlur;
			bool		m_enableDepthOfField;
			bool		m_enableBloom;

		public:
			void SetMotionBlurPostProcess(bool &b);
			void SetMotionBlurPostProcess(bool b);;
			void SetDepthOfDieldPostProcess(bool &b);
			void SetDepthOfDieldPostProcess(bool b);
			void SetBloomPostProcess(bool &b);
			void SetBloomPostProcess(bool b);
		public:
			bool CreateDevice();

			void RenderFrame();

			void PresentScene();

		};	
	}
}