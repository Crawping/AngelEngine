#include"../Public/AGSprite.h"
#include"../../../Core/AngelGUI.h"

AngelGUISystem::GUI2D::AGSprite::AGSprite() : IGUI2D(), m_useBitmap(false), m_bitmapPath(nullptr)
, m_rounded(true), m_radiusX(2), m_radiusY(2)
{

}

void AngelGUISystem::GUI2D::AGSprite::Initialize()
{
	AngelGUISystem::AngelGUI::GetRenderTarget2D()
		->CreateSolidColorBrush(D2D1::ColorF(
			m_defaultColor.x,
			m_defaultColor.y,
			m_defaultColor.z,
			m_defaultColor.w),
			&m_brush);

	D2D1_BITMAP_PROPERTIES Bitprops =
		D2D1::BitmapProperties(
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED));

	D2D1_SIZE_U s;
	s.height = 600;
	s.width = 800;

	HR(AngelGUI::GetRenderTarget2D()->CreateBitmap(s, Bitprops, m_bitmapImage.GetAddressOf()));


	AngelGUI::AddGUI2DItem(this);
}

void AngelGUISystem::GUI2D::AGSprite::Render()
{
	if (m_recreate)
	{
		if (m_rounded)
		{
			COM_RELEASE(m_rectangleRoundedGeometry);
			D2D1_ROUNDED_RECT rect;
			rect.rect = m_rectangle;
			rect.radiusX = m_radiusX;
			rect.radiusY = m_radiusY;
			AngelGUI::GetFactory()->CreateRoundedRectangleGeometry
				(rect, m_rectangleRoundedGeometry.GetAddressOf());
		}
		else
		{
			COM_RELEASE(m_rectangleGeometry);
			AngelGUI::GetFactory()->CreateRectangleGeometry
				(m_rectangle, m_rectangleGeometry.GetAddressOf());
		}


		m_recreate = false;
	}
	AngelGUISystem::AngelGUI::GetRenderTarget2D()
		->SetTransform(D2D1::Matrix3x2F::Scale(m_scale.x, m_scale.y)
			*
			D2D1::Matrix3x2F::Rotation(m_rotationAngle, D2D1::Point2F(
				((m_rectangle.right - m_rectangle.left)*m_scale.x) / 2
				, ((m_rectangle.bottom - m_rectangle.top)*m_scale.x) / 2))
			*
			D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));

	AngelGUISystem::AngelGUI::GetRenderTarget2D()->GetTransform(&m_transformMatrix);


	if (!m_useBitmap)
	{
		if (m_rounded)
			AngelGUISystem::AngelGUI::GetRenderTarget2D()->
			FillGeometry(m_rectangleRoundedGeometry.Get(), m_brush.Get());
		else
			AngelGUISystem::AngelGUI::GetRenderTarget2D()->
			FillGeometry(m_rectangleGeometry.Get(), m_brush.Get());
	}

	else
	{
		AngelGUISystem::AngelGUI::GetRenderTarget2D()->
			DrawBitmap(m_bitmapImage.Get(),
				m_rectangle, 1.0,
				D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
				);

		AngelGUISystem::AngelGUI::GetRenderTarget2D()->
			FillGeometry(m_rectangleRoundedGeometry.Get(), m_brush.Get());

	}
}
void AngelGUISystem::GUI2D::AGSprite::Update(float dt)
{

}

AngelGUISystem::GUI2D::AGSprite::~AGSprite()
{

}

void AngelGUISystem::GUI2D::AGSprite::LoadBitmapImage(const wchar_t * name)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HR(AngelGUI::GetImageFactory()->CreateDecoderFromFilename(
		name,
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

	HR(AngelGUI::GetRenderTarget2D()
		->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			m_bitmapImage.GetAddressOf()
			));

	m_useBitmap = true;
}

void AngelGUISystem::GUI2D::AGSprite::Destroy()
{
	COM_RELEASE(m_rectangleGeometry);
	COM_RELEASE(m_rectangleRoundedGeometry);
	COM_RELEASE(m_brush);
	COM_RELEASE(m_brush);
	COM_RELEASE(m_bitmapImage);
}

void AngelGUISystem::GUI2D::AGSprite::SetActive(bool b)
{
	m_active = b;
}

void AngelGUISystem::GUI2D::AGSprite::SetActive(bool &b)
{
	if (b == true)
		m_active = true;
	else
		m_active = false;
}

void AngelGUISystem::GUI2D::AGSprite::Resize()
{
	if (m_rounded)
	{
		COM_RELEASE(m_rectangleRoundedGeometry);
		D2D1_ROUNDED_RECT rect;
		rect.rect = m_rectangle;
		rect.radiusX = m_radiusX;
		rect.radiusY = m_radiusY;
		AngelGUI::GetFactory()->CreateRoundedRectangleGeometry
			(rect, m_rectangleRoundedGeometry.GetAddressOf());
	}
	else
	{
		COM_RELEASE(m_rectangleGeometry);
		AngelGUI::GetFactory()->CreateRectangleGeometry
			(m_rectangle, m_rectangleGeometry.GetAddressOf());
	}
}

void AngelGUISystem::GUI2D::AGSprite::SetFromResource(ID3D11Texture2D * resource)
{
	
	DXGI_MAPPED_RECT bitmap2Dmap;
	IDXGISurface *tmp;
	resource->QueryInterface(__uuidof(IDXGISurface), (LPVOID*)&tmp);
	tmp->Map(&bitmap2Dmap, DXGI_MAP_READ);

	D2D1_RECT_U r;
	r.left = 0;
	r.right = 800;
	r.bottom = 600;
	r.top = 0;

	D2D1_PIXEL_FORMAT desc2d;
	D2D1_BITMAP_PROPERTIES bmpprops;

	desc2d.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc2d.alphaMode = D2D1_ALPHA_MODE_IGNORE; // Adapt to your needs.      

	bmpprops.dpiX = 96.0f;
	bmpprops.dpiY = 96.0f;
	bmpprops.pixelFormat = desc2d;


	D2D1_SIZE_U size = D2D1::SizeU(
		800,
		600
		); // Adapt to

	HR(AngelGUI::GetRenderTarget2D()->CreateBitmap(size, (void*)bitmap2Dmap.pBits, 
		bitmap2Dmap.Pitch, bmpprops, &m_bitmapImage));
	m_useBitmap = true;
	tmp->Unmap();

	
	/*HR(AngelGUISystem::AngelGUI::GetRenderTarget2D()->CreateSharedBitmap(
		__uuidof(IDXGISurface), (LPVOID*)&tmp, &Bitprops, &m_bitmapData));*/
}