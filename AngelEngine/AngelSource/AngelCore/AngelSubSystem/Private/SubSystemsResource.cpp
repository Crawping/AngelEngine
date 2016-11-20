#include"../Public/SubSystemsResource.h"

Microsoft::WRL::ComPtr<ID3D11Device1>				   AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device = nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext1>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext = nullptr;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::RenderTargetView = nullptr;
Microsoft::WRL::ComPtr<IDXGISwapChain1>				   AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChain = nullptr;
Microsoft::WRL::ComPtr<IDXGIAdapter>				   AngelCore::AngelSubSystemResources::GraphicDeviceResources::DXGIAdaptor = nullptr;
D3D_FEATURE_LEVEL									   AngelCore::AngelSubSystemResources::GraphicDeviceResources::FeatureLevel = D3D_FEATURE_LEVEL_9_1;
Microsoft::WRL::ComPtr<IDXGIFactory2>				   AngelCore::AngelSubSystemResources::GraphicDeviceResources::DXGIFactory = nullptr;
Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::DepthStencilView = nullptr;
std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>>AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers;
UINT												   AngelCore::AngelSubSystemResources::GraphicDeviceResources::SampleCount = 1;
Microsoft::WRL::ComPtr<ID3D11RasterizerState>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::SolidRasterizer;
Microsoft::WRL::ComPtr<ID3D11RasterizerState>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::WireFrameRasterizer;
Microsoft::WRL::ComPtr<ID3D11BlendState>			   AngelCore::AngelSubSystemResources::GraphicDeviceResources::TransparentBlending = nullptr;
Microsoft::WRL::ComPtr<ID3D11BlendState>			   AngelCore::AngelSubSystemResources::GraphicDeviceResources::AdditiveBlending = nullptr;
Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::DisableDepth = nullptr;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::RenderTargetViewVR = nullptr;
Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		   AngelCore::AngelSubSystemResources::GraphicDeviceResources::DepthStencilViewVR = nullptr;
Microsoft::WRL::ComPtr<IDXGISwapChain>				   AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChainVR = nullptr;
Microsoft::WRL::ComPtr<ID3D11Texture2D>				   AngelCore::AngelSubSystemResources::GraphicDeviceResources::BackBufferVR = nullptr;

HWND AngelCore::AngelSubSystemResources::WindowProperties::hwnd = NULL;
unsigned int AngelCore::AngelSubSystemResources::WindowProperties::height = 0;
unsigned int AngelCore::AngelSubSystemResources::WindowProperties::width = 0;
HINSTANCE AngelCore::AngelSubSystemResources::WindowProperties::hInstance;


BaseCamera *AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera;


DirectX::AudioManager * AngelCore::AngelSubSystemResources::AudioEngineProperties::aduioManager = nullptr;