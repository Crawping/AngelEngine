#include"../Public/RenderManager.h"
#include"../AngelLog/Public/AngelLog.h"
#include"../../AngelSubSystem/AngelRendering/Public/Texture2D.h"
#include"../../AngelSubSystem/AngelRendering/Public/PostProcess.h"
#include<dxgidebug.h>

AngelCore::AngelSubSystem::RenderManager::~RenderManager()
{

}

bool AngelCore::AngelSubSystem::RenderManager::CreateDevice()
{
	HRESULT hr;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)(&this->m_dxgiFactory));

	AngelSubSystemResources::GraphicDeviceResources::GetDXGIFactory() = this->m_dxgiFactory;
	if (FAILED(hr))
	{
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Creating DXGIFactory");
		return false;
	}

	AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("++++++++++++ Graphics devices Info Started +++++++++++++");

	Microsoft::WRL::ComPtr<IDXGIAdapter> _adapter = nullptr;
	for (int i = 0; DXGI_ERROR_NOT_FOUND != this->m_dxgiFactory->EnumAdapters(i, &_adapter); ++i)
	{
		DXGI_ADAPTER_DESC adapterDesc = { 0 };
		auto hr = _adapter->GetDesc(&adapterDesc);

		if (hr != S_OK)
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Getting Graphic Adaptor");
		}

		std::wstring adaptorName = adapterDesc.Description;
		std::string sAdaptorName(adaptorName.begin(), adaptorName.end());
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(std::string("DeviceId : " + std::to_string(adapterDesc.DeviceId)).c_str());
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(std::string("DedicatedSystemMemory : " + std::to_string(adapterDesc.DedicatedSystemMemory)).c_str());
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(std::string("DedicatedVideoMemory : " + std::to_string(adapterDesc.DedicatedVideoMemory)).c_str());
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(std::string("VideoCard : " + sAdaptorName).c_str());
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(std::string("VendorId : " + std::to_string(adapterDesc.VendorId)).c_str());
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(std::string("SubSysId : " + std::to_string(adapterDesc.SubSysId)).c_str());

		if (adapterDesc.DeviceId == 140)
		{
			AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Microsoft Basic Render Driver skipped");
			AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Finished Graphic Device.\n");
			AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
		}
		else
		{

			AngelSubSystemResources::GraphicDeviceResources::GetDXGIAdaptor() = _adapter;
			Microsoft::WRL::ComPtr<ID3D11Device>  device11;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext11;


			UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) | defined(_DEBUG)
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};

			HR(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags,
				featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device11, &this->m_suppurtedFeatureLevel,
				&deviceContext11));

			AngelSubSystemResources::GraphicDeviceResources::SetSupportedFeatureLevel(this->m_suppurtedFeatureLevel);

			switch (this->m_suppurtedFeatureLevel)
			{
			case D3D_FEATURE_LEVEL_10_0:
				AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Feature Level Supporeted 10_0");
				break;
			case D3D_FEATURE_LEVEL_10_1:
				AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Feature Level Supporeted 10_1");
				break;
			case D3D_FEATURE_LEVEL_11_0:
				AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Feature Level Supporeted 11_0");
				break;
			case D3D_FEATURE_LEVEL_11_1:
				AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Feature Level Supporeted 11_1");
				break;
			case D3D_FEATURE_LEVEL_9_1:
				AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Feature Level Supporeted 9_1");
				break;
			case D3D_FEATURE_LEVEL_9_2:
				AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Feature Level Supporeted 9_2");
				break;
			case D3D_FEATURE_LEVEL_9_3:
				AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Feature Level Supporeted 9_3");
				break;
			}


			HR(device11.As(&AngelSubSystemResources::GraphicDeviceResources::Device));
			HR(deviceContext11.As(&AngelSubSystemResources::GraphicDeviceResources::DeviceContext));

			COM_RELEASE(device11);
			COM_RELEASE(deviceContext11);
			ID3D10Multithread *multiThread;
			HR(AngelSubSystemResources::GraphicDeviceResources::Device.Get()->
				QueryInterface(__uuidof(ID3D10Multithread),(LPVOID*)&multiThread));

			multiThread->SetMultithreadProtected(true);
			
			UINT sampleLevel = 0;
			UINT sampleCount = AngelSubSystemResources::GraphicDeviceResources::GetSampleCount(); // You set this
			HR(AngelSubSystemResources::GraphicDeviceResources::Device->
				CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM
				, sampleCount, &sampleLevel));

			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

			swapChainDesc.Width = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth(); // Match the size of the window.
			swapChainDesc.Height = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
			swapChainDesc.Stereo = m_dxgiFactory->IsWindowedStereoEnabled();;
			swapChainDesc.SampleDesc.Count = sampleCount; // Don't use multi-sampling.
			swapChainDesc.SampleDesc.Quality = sampleLevel - 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; // All Windows Store apps must use this SwapEffect.
			swapChainDesc.Flags = 0;
			swapChainDesc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen;
			fullscreen.RefreshRate.Numerator = 0;
			fullscreen.RefreshRate.Denominator = 1;
			fullscreen.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			fullscreen.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;

			HR(this->m_dxgiFactory->CreateSwapChainForHwnd
				(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get()
					, AngelCore::AngelSubSystemResources::WindowProperties::GetHWND(),
					&swapChainDesc
					,&fullscreen,
					nullptr,
					&AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChain));

			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

			HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChain->
				GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

			HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
				CreateRenderTargetView(backBuffer.Get()
				, nullptr
				,
				&AngelCore::AngelSubSystemResources::GraphicDeviceResources::RenderTargetView));

			COM_RELEASE(backBuffer);

			AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Finished Graphic Device.\n");
			AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");

			Microsoft::WRL::ComPtr<ID3D11Texture2D> dephtTexture;
			D3D11_TEXTURE2D_DESC desc;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.Height = AngelSubSystemResources::WindowProperties::GetHeight();
			desc.MipLevels = 1;
			desc.MiscFlags = 0;
			desc.SampleDesc.Count = sampleCount;
			desc.SampleDesc.Quality = sampleLevel-1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.Width = AngelSubSystemResources::WindowProperties::GetWidth();

			HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateTexture2D
				(&desc, nullptr, dephtTexture.GetAddressOf()));

			HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateDepthStencilView(dephtTexture.Get(),
				nullptr, AngelSubSystemResources::GraphicDeviceResources::DepthStencilView.GetAddressOf()));

			COM_RELEASE(dephtTexture);

			D3D11_VIEWPORT view;
			view.Height = static_cast<FLOAT>(AngelSubSystemResources::WindowProperties::GetHeight());
			view.Width = static_cast<FLOAT>(AngelSubSystemResources::WindowProperties::GetWidth());
			view.MaxDepth = 1.0f;
			view.MinDepth = 0.0f;
			view.TopLeftX = 0.0f;
			view.TopLeftY = 0.0f;

			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetViewports(1, &view);

		}
	}

	AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("+++++++++++ Graphics devices Info Finished +++++++++++++");
	AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");


	Texture2D::CreateSampler(
		D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_WRAP
		, D3D11_TEXTURE_ADDRESS_WRAP
		, D3D11_TEXTURE_ADDRESS_WRAP, 
		D3D11_COMPARISON_NEVER);

	Texture2D::CreateSampler(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP
		, D3D11_TEXTURE_ADDRESS_CLAMP
		,D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_NEVER);

	Texture2D::CreateSampler(D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS);

	Texture2D::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_NEVER);

	Texture2D::CreateSampler(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, 
		D3D11_TEXTURE_ADDRESS_CLAMP
		, D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS);

	D3D11_RASTERIZER_DESC SolidDesc;
	ZeroMemory(&SolidDesc, sizeof(D3D11_RASTERIZER_DESC));
	SolidDesc.FillMode = D3D11_FILL_SOLID;
	SolidDesc.CullMode = D3D11_CULL_NONE;
	//SolidDesc.FrontCounterClockwise = false;
	//SolidDesc.DepthClipEnable = true;
	//SolidDesc.AntialiasedLineEnable = false;
	//SolidDesc.MultisampleEnable = true;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateRasterizerState(&SolidDesc,
		AngelSubSystemResources::GraphicDeviceResources::SolidRasterizer.GetAddressOf()));

	D3D11_RASTERIZER_DESC wireFrameDesc;
	ZeroMemory(&wireFrameDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameDesc.CullMode = D3D11_CULL_NONE;
	//wireFrameDesc.FrontCounterClockwise = false;
	//wireFrameDesc.DepthClipEnable = true;
	//wireFrameDesc.AntialiasedLineEnable = false;
	//wireFrameDesc.MultisampleEnable = true;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateRasterizerState(&wireFrameDesc,
		AngelSubSystemResources::GraphicDeviceResources::WireFrameRasterizer.GetAddressOf()));
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateBlendState(
		&blendStateDescription,
		AngelSubSystemResources::GraphicDeviceResources::TransparentBlending.GetAddressOf()));

	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateBlendState(
		&blendStateDescription,
		AngelSubSystemResources::GraphicDeviceResources::AdditiveBlending.GetAddressOf()));

	D3D11_DEPTH_STENCIL_DESC DSDesc;
	DSDesc.DepthEnable = 1;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	DSDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_ZERO;
	DSDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_ZERO;
	DSDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_ZERO;
	DSDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_ZERO;
	DSDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_ZERO;
	DSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
	DSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_ZERO;
	DSDesc.StencilEnable = false;
	DSDesc.StencilReadMask = 0;
	DSDesc.StencilWriteMask = 0;

	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateDepthStencilState(
		&DSDesc,
		AngelSubSystemResources::GraphicDeviceResources::DisableDepth.GetAddressOf()));
	return true;
}

bool AngelCore::AngelSubSystem::RenderManager::InitVR(int vpW, int vpH, const LUID* pLuid, bool windowed)
{
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)(&this->m_dxgiFactory)));
	Microsoft::WRL::ComPtr<IDXGIAdapter> _adapter = nullptr;
	for (int i = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters(i, &_adapter); ++i)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		_adapter->GetDesc(&adapterDesc);
		if ((pLuid == nullptr) || memcmp(&adapterDesc.AdapterLuid, pLuid, sizeof(LUID)) == 0)
			break;
		COM_RELEASE(_adapter);
	}


	// Create swap chain
	DXGI_SWAP_CHAIN_DESC scDesc;
	memset(&scDesc, 0, sizeof(scDesc));
	scDesc.BufferCount = 2;
	scDesc.BufferDesc.Width = vpW;
	scDesc.BufferDesc.Height = vpH;
	scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = AngelSubSystemResources::WindowProperties::GetHWND();
	scDesc.SampleDesc.Count = 1;
	scDesc.Windowed = windowed;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	this->m_dxgiFactory->CreateSwapChain(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get(),
		&scDesc, AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChainVR.GetAddressOf());

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChainVR
		->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::BackBufferVR);

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
		->CreateRenderTargetView(
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::BackBufferVR.Get(), NULL,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::RenderTargetViewVR.GetAddressOf
		()));

	DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Width = vpW;
	dsDesc.Height = vpH;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.Format = format;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.MiscFlags = 0;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ID3D11Texture2D * Tex;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateTexture2D(&dsDesc, NULL, &Tex));
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateDepthStencilView
		(Tex, NULL, AngelCore::AngelSubSystemResources::GraphicDeviceResources::DepthStencilViewVR.GetAddressOf()));

	IDXGIDevice1* DXGIDevice1 = nullptr;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get()->QueryInterface(__uuidof(IDXGIDevice1), (void**)&DXGIDevice1));
	DXGIDevice1->SetMaximumFrameLatency(1);


	return true;
}

bool AngelCore::AngelSubSystem::RenderManager::StartUp(...)
{
	if (!CreateDevice())
	{
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Creating Device Failed");
		return false;
	}
	m_enableMotionBlur = false;
	m_enableBloom = false;
	m_enableDepthOfField = false;
	return true;
}

bool AngelCore::AngelSubSystem::RenderManager::ShutDown()
{
	COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::Device);
	COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::DeviceContext);
	COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::SwapChain);
	COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::DepthStencilView);
	COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::DXGIAdaptor);
	COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::DXGIFactory);
	return true;
}

void AngelCore::AngelSubSystem::RenderManager::RenderFrame()
{
	float color[4] = { 0.1f, 0.1f, 0.1f, 0.1f };
	float blendFactor[] = { 0.75f, 0.0f, 0.75f, 0.0f };
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		RSSetState(AngelSubSystemResources::GraphicDeviceResources::SolidRasterizer.Get());
	D3D11_VIEWPORT viewPort;
	viewPort.Height = static_cast<FLOAT>(AngelSubSystemResources::WindowProperties::GetHeight());
	viewPort.Width = static_cast<FLOAT>(AngelSubSystemResources::WindowProperties::GetWidth());
	viewPort.MaxDepth = 1.0f;
	viewPort.MinDepth = 0.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetViewports(1, &viewPort);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->OMSetBlendState
		(AngelSubSystemResources::GraphicDeviceResources::TransparentBlending.Get(), blendFactor, 0xffffffff);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->OMSetRenderTargets(1,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetRenderTarget().GetAddressOf(),
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDepthStencilView().Get());
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->
		ClearDepthStencilView(
			AngelSubSystemResources::GraphicDeviceResources::GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH |
			D3D11_CLEAR_STENCIL, 1.0f, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->
		ClearRenderTargetView(AngelSubSystemResources::GraphicDeviceResources::GetRenderTarget().Get(), color);
}

void AngelCore::AngelSubSystem::RenderManager::PresentScene()
{
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetSwapChain()->Present(0, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->DiscardView(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::RenderTargetView.Get());
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->DiscardView(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DepthStencilView.Get());
}

void AngelCore::AngelSubSystem::RenderManager::OnResize()
{
	if (AngelSubSystemResources::GraphicDeviceResources::GetSwapChain())
	{
		COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::RenderTargetView);
		COM_RELEASE(AngelSubSystemResources::GraphicDeviceResources::DepthStencilView);

		HR(AngelSubSystemResources::GraphicDeviceResources::SwapChain->ResizeBuffers(2,
			0,
			800,
			DXGI_FORMAT_B8G8R8A8_UNORM, 0));
		DXGI_SWAP_CHAIN_DESC1 descswap;
		AngelSubSystemResources::GraphicDeviceResources::SwapChain->GetDesc1(&descswap);

		

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		HR(AngelSubSystemResources::GraphicDeviceResources::SwapChain->
			GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

		HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateRenderTargetView(backBuffer.Get(),
			nullptr,
			AngelSubSystemResources::GraphicDeviceResources::RenderTargetView.GetAddressOf()));

		COM_RELEASE(backBuffer);

		backBuffer = nullptr;

		UINT sampleLevel = 0;
		UINT sampleCount = AngelSubSystemResources::GraphicDeviceResources::GetSampleCount(); // You set this
		HR(AngelSubSystemResources::GraphicDeviceResources::Device->
			CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM
				, sampleCount, &sampleLevel));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.Height = descswap.Height;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = sampleCount;
		desc.SampleDesc.Quality = sampleLevel-1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Width = descswap.Width;

		HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateTexture2D(&desc, nullptr, depthTexture.GetAddressOf()));

		HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateDepthStencilView(depthTexture.Get()
			, nullptr, AngelSubSystemResources::GraphicDeviceResources::DepthStencilView.GetAddressOf()));

		COM_RELEASE(depthTexture);

		depthTexture = nullptr;

		D3D11_VIEWPORT view;
		view.Height = static_cast<FLOAT>(AngelSubSystemResources::WindowProperties::GetHeight());
		view.Width = static_cast<FLOAT>(AngelSubSystemResources::WindowProperties::GetWidth());
		view.MaxDepth = 1.0f;
		view.MinDepth = 0.0f;
		view.TopLeftX = 0.0f;
		view.TopLeftY = 0.0f;

		AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetViewports(1, &view);

		//AngelCore::AngelRendering::PostProcess::GetInstance()->DestroyInstance();
		AngelCore::AngelRendering::PostProcess::GetInstance()->Initialize(
			AngelSubSystemResources::GraphicDeviceResources::Device.Get());

		AngelLog::WriteLogMessage("AngelWindow Resized Successfully");
	}
}

void AngelCore::AngelSubSystem::RenderManager::SetAndClearRenderTarget(ID3D11RenderTargetView * rendertarget,
	ID3D11DepthStencilView * dsv, float R, float G, float B, float A)
{
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float blendFactor[] = { 0.75f, 0.0f, 0.75f, 1.0f };
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->OMSetRenderTargets
		(1, &rendertarget, (dsv ? dsv : nullptr));
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->ClearRenderTargetView
		(rendertarget, color);
	if (dsv)
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->ClearDepthStencilView
		(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void AngelCore::AngelSubSystem::RenderManager::SetViewport(float vpX, float vpY, float vpW, float vpH)
{
	D3D11_VIEWPORT D3Dvp;
	D3Dvp.Width = vpW;    D3Dvp.Height = vpH;
	D3Dvp.MinDepth = 0;   D3Dvp.MaxDepth = 1;
	D3Dvp.TopLeftX = vpX; D3Dvp.TopLeftY = vpY;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetViewports(1, &D3Dvp);
}
void AngelCore::AngelSubSystem::RenderManager::SetMotionBlurPostProcess(bool &b)
{
	if (b == true)
	{
		this->m_enableMotionBlur = true;
		AngelRendering::PostProcess::GetInstance()->PushTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::MOTION_BLUR);
	}
		
	else
	{
		AngelRendering::PostProcess::GetInstance()->PopTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::MOTION_BLUR);
		m_enableMotionBlur = false;
	}
}
void AngelCore::AngelSubSystem::RenderManager::SetMotionBlurPostProcess(bool b)
{
	if (b == true)
	{
		this->m_enableMotionBlur = true;
		AngelRendering::PostProcess::GetInstance()->PushTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::MOTION_BLUR);
	}

	else
	{
		AngelRendering::PostProcess::GetInstance()->PopTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::MOTION_BLUR);
		m_enableMotionBlur = false;
	}
}

void AngelCore::AngelSubSystem::RenderManager::SetDepthOfDieldPostProcess(bool b)
{
	if (b == true)
	{
		this->m_enableDepthOfField = true;
		AngelRendering::PostProcess::GetInstance()->PushTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::DEPTH_OF_FIELD);
	}

	else
	{
		AngelRendering::PostProcess::GetInstance()->PopTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::DEPTH_OF_FIELD);
		m_enableDepthOfField = false;
	}
}
void AngelCore::AngelSubSystem::RenderManager::SetDepthOfDieldPostProcess(bool &b)
{
	if (b == true)
	{
		this->m_enableDepthOfField = true;
		AngelRendering::PostProcess::GetInstance()->PushTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::DEPTH_OF_FIELD);
	}

	else
	{
		AngelRendering::PostProcess::GetInstance()->PopTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::DEPTH_OF_FIELD);
		m_enableDepthOfField = false;
	}
}

void AngelCore::AngelSubSystem::RenderManager::SetBloomPostProcess(bool b)
{
	if (b == true && !m_enableBloom)
	{
		this->m_enableBloom = true;
		AngelRendering::PostProcess::GetInstance()->PushTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::BLOOM);
	}

	else
	{
		AngelRendering::PostProcess::GetInstance()->PopTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::BLOOM);
		m_enableBloom = false;
	}
}

void AngelCore::AngelSubSystem::RenderManager::SetBloomPostProcess(bool &b)
{
	if (b == true)
	{
		this->m_enableBloom = true;
		AngelRendering::PostProcess::GetInstance()->PushTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::BLOOM);
	}

	else
	{
		AngelRendering::PostProcess::GetInstance()->PopTechnique(
			AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE::BLOOM);
		m_enableBloom = false;
	}
}
