#pragma once

#include<Windows.h>
#include<d3d11_1.h>
#include<d3d11.h>
#include<wrl\client.h>
#include<vector>
#include"../../../AngelCore/../AngelCore/AngelSubSystem/AngelRendering/Public/BaseCamera.h"
#include"../Private/Sound/Audio.h"



class BaseCamera;

namespace AngelCore
{
	namespace AngelSubSystemResources
	{
		struct BaseCameraProperties
		{
			static BaseCamera *BCamera;
		};

		struct AudioEngineProperties
		{
			static DirectX::AudioManager * aduioManager;
		};

		struct WindowProperties
		{
		public:

			static void SetHWND(HWND _hwnd)											{ hwnd = _hwnd; }
			static void SetWidth(unsigned int _width)								{ width = _width; }
			static void SetHeight(unsigned int _heigth)								{ height = _heigth; }
			static void SetHINSTANCE(HINSTANCE _h)									{ hInstance = _h; }

		public:

			static HWND GetHWND()													{ return hwnd; }
			static float GetWidth()													{ return width; }
			static float GetHeight()												{ return height; }
			static HINSTANCE GetHINSTANCE()											{ return hInstance; }
				
		private:

			static HWND																hwnd;
			static unsigned int														width;
			static unsigned int														height;
			static HINSTANCE														hInstance;

		private:

			WindowProperties();
		};

		struct GraphicDeviceResources
		{
		public:
			static Microsoft::WRL::ComPtr<ID3D11Device1> GetDevice3D()						{ return Device; }
			static Microsoft::WRL::ComPtr<ID3D11DeviceContext1> GetDeviceContext3D()		{ return DeviceContext; }
			static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTarget()			{ return RenderTargetView; }
			static Microsoft::WRL::ComPtr<IDXGISwapChain1> GetSwapChain()					{ return SwapChain; }
			static Microsoft::WRL::ComPtr<IDXGIAdapter> GetDXGIAdaptor()					{ return DXGIAdaptor; }
			static D3D_FEATURE_LEVEL GetSupportedFeatureLevel()								{ return FeatureLevel; }
			static Microsoft::WRL::ComPtr<IDXGIFactory2> GetDXGIFactory()					{ return DXGIFactory; }
			static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthStencilView()		{ return DepthStencilView; }
			static UINT GetSampleCount()													{ return SampleCount; }

		public:

			static void SetSupportedFeatureLevel(D3D_FEATURE_LEVEL _SFL)					{ FeatureLevel = _SFL; }
			static void SetSampleCount(UINT _sampleLevel)									{ SampleCount = _sampleLevel; }

		public:

			static Microsoft::WRL::ComPtr<ID3D11Device1>									Device;
			static Microsoft::WRL::ComPtr<ID3D11DeviceContext1>								DeviceContext;
			static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>							RenderTargetView;
			static Microsoft::WRL::ComPtr<ID3D11RenderTargetView>							RenderTargetViewVR;
			static Microsoft::WRL::ComPtr<IDXGISwapChain1>									SwapChain;
			static Microsoft::WRL::ComPtr<IDXGISwapChain>									SwapChainVR;
			static Microsoft::WRL::ComPtr<IDXGIAdapter>										DXGIAdaptor;
			static D3D_FEATURE_LEVEL														FeatureLevel;
			static Microsoft::WRL::ComPtr<IDXGIFactory2>									DXGIFactory;
			static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>							DepthStencilView;
			static Microsoft::WRL::ComPtr<ID3D11DepthStencilView>							DepthStencilViewVR;
			static UINT																		SampleCount;
			static std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>>					Samplers;
			static Microsoft::WRL::ComPtr<ID3D11RasterizerState>							SolidRasterizer;
			static Microsoft::WRL::ComPtr<ID3D11RasterizerState>							WireFrameRasterizer;
			static Microsoft::WRL::ComPtr<ID3D11BlendState>									TransparentBlending;
			static Microsoft::WRL::ComPtr<ID3D11BlendState>									AdditiveBlending;
			static Microsoft::WRL::ComPtr<ID3D11DepthStencilState>							DisableDepth;
			static Microsoft::WRL::ComPtr<ID3D11Texture2D>									BackBufferVR;

		private:

			GraphicDeviceResources();
		};


	}
}
