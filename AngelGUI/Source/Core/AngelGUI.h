#pragma once
#include<d2d1_2.h>
#include<d3d11_1.h>
#include<DirectXMath.h>
#include<wincodec.h>
#include<wrl\client.h>
#include<vector>
#include<dwrite.h>
#include<algorithm>
#include"GlobalTools.h"
#include"../Controls/3D/Public/IGUI3D.h"
#include"../Controls/2D/Public/IGUI2D.h"


#define __ANGEL_GUI

namespace AngelGUISystem
{
	class AngelGUI : 
		public AngelGUISystem::GUIGlobalTools::ComBase<AngelGUISystem::GUIGlobalTools::QiList<IUnknown> >
	{
	public:
		static void OnKeyPress(UINT32 keyCode);
		static void OnKeyCharacter(UINT32 keyCode);
		static void OnMousePress(UINT message, float x, float y);
		static void OnMouseRelease(UINT message, float x, float y);
		static void OnMouseMove(float x, float y);
		static void OnMouseScroll(float xScroll, float yScroll);

	public:
		static bool StartUp(HWND hwnd,
			ID3D11Device1 *device, ID3D11DeviceContext1 *deviceContext
			, IDXGISwapChain1* swapChain) ;
		static bool ShutDown() ;
		static void UpdateGUI2D(float dt);
		static void UpdateGUI3D(float dt,DirectX::CXMMATRIX view,DirectX::CXMMATRIX projection);
		static void RenderGUI2D();
		static void RenderGUI2DUsingAdditionalRenderTarget();
		static void RenderGUI3D();
		static void Resize(IDXGISwapChain1 *swapChain);
		static void PreResize();
		//if we have an other swapcahin in program and want to render with second swapchain.
		static bool CreateAdditionalRenderTarget(ID3D11Texture2D* swapChain);
		//Clear and delete all 2d and 3d gui items but not self pointer.
		static void Clear();
#pragma region Getters
		static Microsoft::WRL::ComPtr<ID2D1RenderTarget> GetRenderTarget2D() { return m_renderUsingAdditionalSwapChain ?  m_AdditionalRenderTarget2D : m_renderTarget2D; }
		static Microsoft::WRL::ComPtr<IWICImagingFactory> GetImageFactory() { return m_imageFactory; }
		static Microsoft::WRL::ComPtr<ID2D1Factory1> GetFactory() { return m_direct2DFactory; }
		static Microsoft::WRL::ComPtr<IDWriteFactory> GetDirectWriteFactory() { return m_directWriteFactory; }
		static Microsoft::WRL::ComPtr<ID3D11Device1> GetDevice3D() { return m_device3D; }
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext1> GetDeviceContext3D() { return m_deviceContext3D; }
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState() { return m_samplerState; }
		//when using additional Swapchain.
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetRenderedGUI() { return m_shaderResourceView; }
		static HWND GetHWND()								{ return m_hwnd; }
		static bool IsResize()								{ return m_resized; }
		static void SetUsingAddtionalRenderTarget(bool b)	{ m_renderUsingAdditionalSwapChain = b; }
		static void SetRenderMouse(bool b)					{ m_renderMouse = b; }
		static void SetTextureFolderPath(std::wstring path) { m_textureFolderPath = path; }
		static std::wstring GetTextureFolderPath()			{ return m_textureFolderPath; }
		
#pragma endregion

	public:

		static void AddGUI2DItem(AngelGUISystem::GUI2D::IGUI2D * item);
		static void AddGUI3DItem(AngelGUISystem::GUI3D::IGUI3D * item);

	private:
		static Microsoft::WRL::ComPtr<IDWriteFactory>				m_dWriteFactory;
		static Microsoft::WRL::ComPtr<ID2D1Factory1>				m_direct2DFactory;
		static Microsoft::WRL::ComPtr<IWICImagingFactory>			m_imageFactory;
		static Microsoft::WRL::ComPtr<ID2D1Device>					m_device2D;
		static Microsoft::WRL::ComPtr<ID2D1DeviceContext>			m_deviceContext2D;
		static Microsoft::WRL::ComPtr<ID2D1Bitmap1>					m_bitmap;
		static Microsoft::WRL::ComPtr<ID2D1RenderTarget>			m_renderTarget2D;
		static Microsoft::WRL::ComPtr<ID2D1RenderTarget>			m_AdditionalRenderTarget2D;
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		m_shaderResourceView;
		static Microsoft::WRL::ComPtr<IDWriteFactory>				m_directWriteFactory;
		static Microsoft::WRL::ComPtr<ID3D11Device1>				m_device3D;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext1>			m_deviceContext3D;
		static Microsoft::WRL::ComPtr<ID3D11SamplerState>			m_samplerState;
		static HWND													m_hwnd;
		static Microsoft::WRL::ComPtr<ID2D1Bitmap>					m_mouseCursor;
		static Microsoft::WRL::ComPtr<ID2D1Bitmap>					m_mouseCursorAdditionalRenderTarget;
		//All 2d GUI items.
		static std::vector<AngelGUISystem::GUI2D::IGUI2D * >	m_2dGuiItems;
		//All 3d GUI items.
		static std::vector<AngelGUISystem::GUI3D::IGUI3D * >	m_3dGuiItems;
		static bool m_resized;
		static bool m_renderUsingAdditionalSwapChain;
		static bool m_renderMouse;
		static DirectX::XMFLOAT2	m_mousePosition;
		//will load all textures from here.
		static std::wstring			m_textureFolderPath;
	};
}