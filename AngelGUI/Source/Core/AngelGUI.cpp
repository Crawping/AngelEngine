#include"AngelGUI.h"
#include<dwrite_2.h>

Microsoft::WRL::ComPtr<IDWriteFactory>			AngelGUISystem::AngelGUI::m_dWriteFactory					= nullptr;
Microsoft::WRL::ComPtr<ID2D1Factory1>			AngelGUISystem::AngelGUI::m_direct2DFactory					= nullptr;
Microsoft::WRL::ComPtr<IWICImagingFactory>		AngelGUISystem::AngelGUI::m_imageFactory					= nullptr;
Microsoft::WRL::ComPtr<ID2D1Device>				AngelGUISystem::AngelGUI::m_device2D						= nullptr;
Microsoft::WRL::ComPtr<ID2D1DeviceContext>		AngelGUISystem::AngelGUI::m_deviceContext2D					= nullptr;
Microsoft::WRL::ComPtr<ID2D1Bitmap1>			AngelGUISystem::AngelGUI::m_bitmap							= nullptr;
Microsoft::WRL::ComPtr<ID2D1RenderTarget>		AngelGUISystem::AngelGUI::m_renderTarget2D					= nullptr;
Microsoft::WRL::ComPtr<ID2D1RenderTarget>		AngelGUISystem::AngelGUI::m_AdditionalRenderTarget2D		= nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>AngelGUISystem::AngelGUI::m_shaderResourceView				= nullptr;
HWND											AngelGUISystem::AngelGUI::m_hwnd							= nullptr;
Microsoft::WRL::ComPtr<IDWriteFactory>			AngelGUISystem::AngelGUI::m_directWriteFactory				= nullptr;
Microsoft::WRL::ComPtr<ID3D11Device1>			AngelGUISystem::AngelGUI::m_device3D						= nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext1>	AngelGUISystem::AngelGUI::m_deviceContext3D					= nullptr;
Microsoft::WRL::ComPtr<ID3D11SamplerState>		AngelGUISystem::AngelGUI::m_samplerState					= nullptr;
bool											AngelGUISystem::AngelGUI::m_resized							= false;
bool											AngelGUISystem::AngelGUI::m_renderUsingAdditionalSwapChain	= false;
bool											AngelGUISystem::AngelGUI::m_renderMouse						= false;
std::vector<AngelGUISystem::GUI2D::IGUI2D * >	AngelGUISystem::AngelGUI::m_2dGuiItems;
std::vector<AngelGUISystem::GUI3D::IGUI3D * >	AngelGUISystem::AngelGUI::m_3dGuiItems;
Microsoft::WRL::ComPtr<ID2D1Bitmap>				AngelGUISystem::AngelGUI::m_mouseCursor = nullptr;
Microsoft::WRL::ComPtr<ID2D1Bitmap>				AngelGUISystem::AngelGUI::m_mouseCursorAdditionalRenderTarget = nullptr;
DirectX::XMFLOAT2								AngelGUISystem::AngelGUI::m_mousePosition = DirectX::XMFLOAT2(0, 0);
std::wstring									AngelGUISystem::AngelGUI::m_textureFolderPath;


bool AngelGUISystem::AngelGUI::StartUp(HWND hwnd, ID3D11Device1 *device
	, ID3D11DeviceContext1 *deviceContext,IDXGISwapChain1 *swapChain)
{
	m_hwnd = hwnd;
	m_device3D = device;
	m_deviceContext3D = deviceContext;


	using namespace AngelGUISystem::GUIGlobalTools;
	IDXGISurface *texture;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));

#pragma region Create device2D and context2D

	D2D1_FACTORY_OPTIONS factory2DOptions;
	ZeroMemory(&factory2DOptions, sizeof(D2D1_FACTORY_OPTIONS));

#ifdef _DEBUG
	//factory2DOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	//Create Direct2D factory
	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory2),
		&factory2DOptions,
		&m_direct2DFactory));

	HR(CoCreateInstance(CLSID_WICImagingFactory2,
		nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_imageFactory)));

	HR(DWriteCreateFactory
		(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory2),
			&m_directWriteFactory));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_HARDWARE,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96, 96);

	HR(m_direct2DFactory->
		CreateDxgiSurfaceRenderTarget(
		texture, &props, m_renderTarget2D.GetAddressOf()));

	m_renderTarget2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	using namespace Microsoft::WRL;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	HR(m_device3D->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf()));
	texture->Release();
	texture = nullptr;

	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	TCHAR Buffer[MAX_PATH];
	std::wstring cursorName = L"Cursor.png";
	GetCurrentDirectory(MAX_PATH, Buffer);
	std::wstring data = Buffer;
	std::wstring toWstr(cursorName.begin(), cursorName.end());
	data += L"\\AngelGUIData\\";
	data += toWstr;

	HR(m_imageFactory->CreateDecoderFromFilename(
		data.c_str(),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		));
	// Create the initial frame.
	HR(pDecoder->GetFrame(0, &pSource));

	// Convert the image format to 32bppPBGRA
	// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
	HR(AngelGUI::GetImageFactory()->
		CreateFormatConverter(&pConverter));

	HR(pConverter->Initialize(
		pSource,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeMedianCut
		));

	HR(m_renderTarget2D
		->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			m_mouseCursor.GetAddressOf()
			));

	return true;
}

bool AngelGUISystem::AngelGUI::CreateAdditionalRenderTarget(ID3D11Texture2D * swapChain)
{
	using namespace AngelGUISystem::GUIGlobalTools;
	IDXGISurface1 *pDxgiSurface = NULL;

	swapChain->QueryInterface(IID_PPV_ARGS(&pDxgiSurface));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_HARDWARE,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96, 96);

	HR(m_direct2DFactory->
	CreateDxgiSurfaceRenderTarget(
		pDxgiSurface, &props, m_AdditionalRenderTarget2D.GetAddressOf()));

	m_AdditionalRenderTarget2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	TCHAR Buffer[MAX_PATH];
	std::wstring cursorName = L"Cursor.png";
	GetCurrentDirectory(MAX_PATH, Buffer);
	std::wstring data = Buffer;
	std::wstring toWstr(cursorName.begin(), cursorName.end());
	data += L"\\AngelGUIData\\";
	data += toWstr;

	HR(m_imageFactory->CreateDecoderFromFilename(
		data.c_str(),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		));
	// Create the initial frame.
	HR(pDecoder->GetFrame(0, &pSource));

	// Convert the image format to 32bppPBGRA
	// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
	HR(AngelGUI::GetImageFactory()->
		CreateFormatConverter(&pConverter));

	HR(pConverter->Initialize(
		pSource,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeMedianCut
		));

	HR(m_AdditionalRenderTarget2D
		->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			m_mouseCursorAdditionalRenderTarget.GetAddressOf()
			));
	return true;
}
void AngelGUISystem::AngelGUI::PreResize()
{
	COM_RELEASE(m_renderTarget2D);
}

void AngelGUISystem::AngelGUI::Resize(IDXGISwapChain1 *swapChain)
{
	if (!m_hwnd)
		return;
	using namespace AngelGUISystem::GUIGlobalTools;
	IDXGISurface *texture;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
	DXGI_SURFACE_DESC desc;
	texture->GetDesc(&desc);


	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_HARDWARE,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96, 96);
	COM_RELEASE(m_renderTarget2D);
	HR(m_direct2DFactory->
		CreateDxgiSurfaceRenderTarget(
			texture, &props, m_renderTarget2D.GetAddressOf()));

	m_renderTarget2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	texture->Release();
	texture = nullptr;
	m_resized = true;

	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->Resize();
	}
}

bool AngelGUISystem::AngelGUI::ShutDown()
{
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		m_2dGuiItems[i]->Destroy();
		delete m_2dGuiItems[i];
	}

	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		m_3dGuiItems[i]->Destroy();
		delete m_3dGuiItems[i];
	}
	COM_RELEASE(m_dWriteFactory);
	COM_RELEASE(m_direct2DFactory);
	COM_RELEASE(m_imageFactory);
	COM_RELEASE(m_device2D);
	COM_RELEASE(m_deviceContext2D);
	COM_RELEASE(m_bitmap);
	COM_RELEASE(m_renderTarget2D);
	COM_RELEASE(m_shaderResourceView);
	return true;
}

void AngelGUISystem::AngelGUI::OnKeyPress(UINT32 keyCode)
{
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if(m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->OnKeyPress(keyCode);
	}

	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->OnKeyPress(keyCode);
	}
}

void AngelGUISystem::AngelGUI::OnKeyCharacter(UINT32 keyCode)
{
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->OnKeyCharacter(keyCode);
	}

	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->OnKeyCharacter(keyCode);
	}
}

void AngelGUISystem::AngelGUI::OnMousePress(UINT message, float x, float y)
{
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive() && m_2dGuiItems[i])
			m_2dGuiItems[i]->OnMousePress(message,x,y);
	}

	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->OnMousePress(message, x, y);
	}
}

void AngelGUISystem::AngelGUI::OnMouseMove(float x, float y)
{
	m_mousePosition.x = x;
	m_mousePosition.y = y;
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->OnMouseMove(x, y);
	}

	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->OnMouseMove(x, y);
	}
}

void AngelGUISystem::AngelGUI::OnMouseRelease(UINT message, float x, float y)
{
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->OnMouseRelease(message, x, y);
	}
	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->OnMouseRelease(message, x, y);
	}
}

void AngelGUISystem::AngelGUI::OnMouseScroll(float xScroll, float yScroll)
{
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->OnMouseScroll(xScroll, yScroll);
	}
	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->OnMouseScroll(xScroll, yScroll);
	}
}

void AngelGUISystem::AngelGUI::AddGUI2DItem(AngelGUISystem::GUI2D::IGUI2D * item)
{
	m_2dGuiItems.push_back(item);
}

void AngelGUISystem::AngelGUI::AddGUI3DItem(AngelGUISystem::GUI3D::IGUI3D * item)
{
	m_3dGuiItems.push_back(item);
}

void AngelGUISystem::AngelGUI::RenderGUI2D()
{
	m_renderTarget2D->BeginDraw();
	
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->Render();
	}
	m_renderTarget2D->SetTransform(D2D1::Matrix3x2F::Identity());
	if (m_renderMouse)
	{
		m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		m_renderTarget2D->DrawBitmap(m_mouseCursor.Get(),
			D2D1::RectF(m_mousePosition.x - 5, m_mousePosition.y, m_mousePosition.x + 20, m_mousePosition.y + 25));
	}
	m_renderTarget2D->EndDraw();
}

void AngelGUISystem::AngelGUI::RenderGUI2DUsingAdditionalRenderTarget()
{
	m_AdditionalRenderTarget2D->BeginDraw();
	m_AdditionalRenderTarget2D->Clear(D2D1::ColorF(0, 0, 0, 0));
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->Render();
	}
	
	m_AdditionalRenderTarget2D->SetTransform(D2D1::Matrix3x2F::Identity());
	if (m_renderMouse)
	{
		m_AdditionalRenderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		m_AdditionalRenderTarget2D->DrawBitmap(m_mouseCursorAdditionalRenderTarget.Get(),
			D2D1::RectF(m_mousePosition.x - 5, m_mousePosition.y, m_mousePosition.x + 20, m_mousePosition.y + 25));
	}
	m_AdditionalRenderTarget2D->EndDraw();
}

void AngelGUISystem::AngelGUI::RenderGUI3D()
{
	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->Render();
	}
}
void AngelGUISystem::AngelGUI::UpdateGUI2D(float dt)
{
	SetCursor(m_renderMouse ? NULL : LoadCursor(NULL, IDC_ARROW));
	for (unsigned int i = 0;i < m_2dGuiItems.size();i++)
	{
		if (m_2dGuiItems[i]->GetActive())
			m_2dGuiItems[i]->Update(dt);
	}
}

void AngelGUISystem::AngelGUI::UpdateGUI3D(float dt
	, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
	for (unsigned int i = 0;i < m_3dGuiItems.size();i++)
	{
		if (m_3dGuiItems[i]->GetActive())
			m_3dGuiItems[i]->Update(dt, view, projection);
	}
}

void AngelGUISystem::AngelGUI::Clear()
{
	m_2dGuiItems.clear();
	m_3dGuiItems.clear();
}