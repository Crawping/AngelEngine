
#include<tchar.h>
#include<d2d1helper.h>
#include"../Public/AG3DText.h"
#include"../Public/AG3DSprite.h"
#include"../../../Core/WICTextureLoader.h"


AngelGUISystem::GUI3D::AGText::AGText() :
	m_currentTextSize(0), m_heightOffset(0.0f)
	, m_editable(true),
	m_fitMode(SCALE_DOWN_TO_FIT), m_useForButton(false), m_Layer(nullptr)
	, m_renderScrollBar(false), m_scrollBarSelected(false), m_rectangleGeometrySrollBar(nullptr)
	, m_lastMouseX(0.0f), m_lastMouseY(0.0f), m_scrollBarY(0.0f), m_scrollBarSpeed(0.005f)
	, m_scrolledTextPosition(0.0f), m_canScroll(true), m_rectangleGeometryBackground(nullptr)
	,m_scrollBar(nullptr) , m_scrollBarHovered(false)
{
	m_scale = DirectX::XMFLOAT3(1, 1, 1);
	m_rotation = DirectX::XMFLOAT3(0.0, 0.0f, 0.0);
	m_textColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Backcolor = DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 0.8f);
	m_caretBlinkCounter = 0;
	m_caretBlinkSpeed = 40;
	m_caretBlink = false;
}

AngelGUISystem::GUI3D::AGText::AGText(const AGText& that)
{
	
}

AngelGUISystem::GUI3D::AGText::~AGText()
{
	COM_RELEASE(this->m_stateBlock);
	COM_RELEASE(this->m_textFormat);
	COM_RELEASE(this->m_textLayout)
}

AngelGUISystem::GUI3D::AGText & AngelGUISystem::GUI3D::AGText::operator=(const AGText& that)
{
	return *this;
}


void AngelGUISystem::GUI3D::AGText::Initialize()
{
	hwnd_ = NULL;

	caretPosition_ = 0;
	caretAnchor_ = 0;
	caretPositionOffset_ = 0;
	currentlySelecting_ = false;
	currentlyPanning_ = false;
	previousMouseX = 0;
	previousMouseY = 0;
	scaleX_ = 1;
	scaleY_ = 1;
	angle_ = 0;
	originX_ = 0;
	originY_ = 0;
	ID3D11Texture2D *texture;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Height = 598;
	texDesc.Width = 781;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	HR(AngelGUI::GetDevice3D()
		->CreateTexture2D(&texDesc, NULL, &texture));

	IDXGISurface1 *pDxgiSurface = NULL;

	texture->QueryInterface(IID_PPV_ARGS(&pDxgiSurface));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96
			);


	HR(AngelGUI::GetFactory()->CreateDxgiSurfaceRenderTarget(
		pDxgiSurface, props, m_renderTarget2D.GetAddressOf()));


	//HR(AngelGUISystem::AngelSubSystemResources::GraphicDeviceResources::Direct2DFactory->CreateHwndRenderTarget(
	//D2D1::RenderTargetProperties(),
	//D2D1::HwndRenderTargetProperties(hwnd_, d2dSize),
	//m_renderTarget2D.GetAddressOf()
	//));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = texDesc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	(AngelGUI::GetDevice3D()->CreateShaderResourceView
		(texture, &srvd, m_shaderResourceView.GetAddressOf()));

	m_renderTarget2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	//DPIScale::Initialize(m_direct2DFactory.Get());
	m_font = new Font();
	m_font->size = 40;

	AngelGUI::GetDirectWriteFactory()->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_font->size,
		L"",
		&m_textFormat
		);
	m_renderTarget2D->CreateSolidColorBrush(D2D1::ColorF(
		1.0f,
		1.0f,
		1.0f,
		0.8f),
		&m_brush);

	COM_RELEASE(m_BrushCursor);
	HR(m_renderTarget2D->CreateSolidColorBrush(D2D1::ColorF(
		1.0f,
		1.0f,
		1.0f,
		0.8f), m_BrushCursor.GetAddressOf()));

	COM_RELEASE(m_BrushCaretHighlight);
	HR(m_renderTarget2D->CreateSolidColorBrush(D2D1::ColorF(
		0.52f,
		0.80f,
		0.98f,
		0.8f), m_BrushCaretHighlight.GetAddressOf()));

	// Set the initial text.
	text_.assign(L"");
	

	layoutEditor_ = AngelGUI::GetDirectWriteFactory().Get();
	// Create an ideal layout for the text editor based on the text and format,
	// favoring document layout over pixel alignment.
	HR(layoutEditor_.GetFactory()->CreateTextLayout(
		text_.c_str(),
		static_cast<UINT32>(text_.size()),
		m_textFormat.Get(),
		m_rectangle.right,            // maximum width
		m_rectangle.bottom,
		&textLayout_
		));

	// Get size of text layout; needed for setting the view origin.
	float layoutWidth = textLayout_->GetMaxWidth();
	float layoutHeight = textLayout_->GetMaxHeight();
	originX_ = layoutWidth / 2;
	originY_ = layoutHeight / 2;
	m_matrix2DTransform = D2D1::Matrix3x2F::Identity();
	// Set the initial text layout and update caret properties accordingly.
	UpdateCaretFormatting();
	m_scrollBar = new AGSprite();
	m_scrollBar->Initialize();
	m_scrollBar->SetRectangle(0, 0, 30, 30);
	m_scrollBar->SetPosition(m_position);
	m_scrollBar->SetScale(m_scale);
	m_scrollBar->SetRotation(m_rotation);
	AngelGUI::AddGUI3DItem(this);

	m_cbWorldCopy.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());
	m_cbWorldPaste.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());
	m_cbWorldCut.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());
	m_cbWorldfont.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());

	m_intersectsCopyOption = false;
	m_intersectsPasteOption = false;
	m_intersectsCutOption = false;
	m_intersectsfontOption = false;
	m_showOptions = false;
	m_updateOptionsTransform = false;

	DirectX::XMStoreFloat4x4(&m_copyOptionWorld, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_pasteOptionWorld, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_cutOptionWorld, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_fontOptionWorld, DirectX::XMMatrixIdentity());

	m_mousePosition.x = 0;
	m_mousePosition.y = 0;

	m_copyTexture = LoadOptionIcon(L"copy.png");
	m_pasteTexture = LoadOptionIcon(L"paste.png");
	m_fontTexture = LoadOptionIcon(L"font.png");
	m_cutTexture = LoadOptionIcon(L"cut.png");

	m_copyOptionPosition = DirectX::XMFLOAT3(0, 0, 0);
	m_cutOptionPosition = DirectX::XMFLOAT3(0, 0, 0);
	m_pasteOptionPosition = DirectX::XMFLOAT3(0, 0, 0);
	m_fontOptionPosition = DirectX::XMFLOAT3(0, 0, 0);

	m_isMouseSelectedOptions = false;

	m_cbColorOptionsPaste.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());
	m_cbColorOptionsCopy.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());
	m_cbColorOptionsCut.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());
	m_cbColorOptionsFont.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());

}


LRESULT CALLBACK AngelGUISystem::GUI3D::AGText::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Relays messages for the text editor to the internal class.

	AngelGUISystem::GUI3D::AGText* window = reinterpret_cast<AngelGUISystem::GUI3D::AGText*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch (message)
	{
	case WM_NCCREATE:
	{
		// Associate the data structure with this window handle.
		CREATESTRUCT* pcs = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<AngelGUISystem::GUI3D::AGText*>(pcs->lpCreateParams);
		window->hwnd_ = hwnd;
		//window->AddRef(); // implicit reference via HWND
		SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToUlong(window));

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

void AngelGUISystem::GUI3D::AGText::Render()
{
	using namespace AngelGUISystem::GUIGlobalTools;

	if (true)
	{
		m_scrollBar->SetRectangle(0,0,50,m_rectangle.bottom);
		m_scrollBar->SetPosition(DirectX::XMFLOAT3(
			m_position.x
			+
			(m_rectangle.right/500)*m_scale.x
			+
			0.105f,m_position.y-m_scrollBarY,m_position.z));
	
		m_scrollBar->SetScale(m_scale);
		m_scrollBar->SetRotation(m_rotation);
		m_recreate = false;
	}

	if(m_scrollBarHovered)
		m_scrollBar->SetCurrentColor(0.65f, 0.65f, 0.65f, 1.0f);
	if(m_scrollBarSelected)
		m_scrollBar->SetCurrentColor(0.85f, 0.85f, 0.85f, 1.0f);
	else if(!m_scrollBarSelected && !m_scrollBarHovered)
		m_scrollBar->SetCurrentColor(0.5f, 0.5f, 0.5f, 1.0f);

	if (m_renderScrollBar)
		m_scrollBar->SetActive(true);
	else
		m_scrollBar->SetActive(false);
	// Draws the background, page, selection, and text.
	m_renderTarget2D->BeginDraw();
	D2D1_COLOR_F clearColor;

	clearColor.r = m_currentColor.x;
	clearColor.g = m_currentColor.y;
	clearColor.b = m_currentColor.z;
	clearColor.a = m_currentColor.w;
	if (m_editable && m_selected)
	{
		clearColor.r = m_onHoverColor.x;
		clearColor.g = m_onHoverColor.y;
		clearColor.b = m_onHoverColor.z;
		clearColor.a = m_onHoverColor.w;
	}
	//m_renderTarget2D->BeginDraw();
	m_renderTarget2D->Clear(clearColor);
	// Calculate actual location in render target based on the
	// current page transform and location of edit control.
	D2D1::Matrix3x2F pageTransform;
	GetViewMatrix(&Cast(pageTransform));

	// Scale/Rotate canvas as needed
	DWRITE_MATRIX previousTransform;
	m_renderTarget2D->GetTransform(&Cast(previousTransform));
	m_renderTarget2D->SetTransform((pageTransform));

	DWRITE_TEXT_METRICS met;
	textLayout_->GetMetrics(&met);
	if (m_fitMode == SCALE_DOWN_TO_FIT && !m_useForButton)
	{
		while (met.height > 600.0f)
			/*|| met.width + 5 > (((m_rectangle.right)*m_scale.x) - ((m_rectangle.left)*m_scale.x))*/

		{
			textLayout_->GetMetrics(&met);
			ScaleDownFont(0.05f);
		}
	}

	if (m_useForButton)
	{
		while (met.height > ((m_rectangle.bottom) - (m_rectangle.top))
			|| met.width + 5 > (((m_rectangle.right-30)) - ((m_rectangle.left))))

		{
			textLayout_->GetMetrics(&met);
			ScaleDownFont(0.05f);
		}
	}

	if (m_useForButton)
	{
		m_matrix2DTransform
			= D2D1::Matrix3x2F::Translation(
				m_rectangle.right/2-met.width/2,
				m_rectangle.bottom / 2 - met.height / 2);
	}

	if (m_fitMode == FIT_MODE::SCROLL_BAR)
	{
		if (met.height > ((m_rectangle.bottom*m_scale.y) - (m_rectangle.top*m_scale.y)))
			m_renderScrollBar = true;
		else
			m_renderScrollBar = false;
	}

	if (m_renderScrollBar)
	{
		float precentShown = (m_rectangle.bottom / met.height);
		m_scrollBarRectangle.left = 0;
		m_scrollBarRectangle.right = 50;
		m_scrollBarRectangle.top = 0;
		m_scrollBarRectangle.bottom = m_rectangle.bottom - (m_rectangle.bottom*(1 - precentShown));
		m_scrollBar->SetRectangle(m_scrollBarRectangle);

	/*	if (m_scrollBarY < 0.0f)
			m_scrollBarY = 0.0f;
		if (m_scrollBarY + m_scrollBarRectangle.bottom > m_rectangle.bottom)
		{
			m_scrollBarY = m_rectangle.bottom - m_scrollBarRectangle.bottom;
		}
		{
			m_scrollBarTransformationMatrix =
				D2D1::Matrix3x2F::Translation(m_position.x + m_rectangle.right, m_position.y +
					m_scrollBarY);
		}*/

		m_scrolledTextPosition = m_scrollBarY;
		float unitInBar = met.height / m_rectangle.bottom;
		float unitInText = 1 / unitInBar;
		m_scrolledTextPosition = m_scrollBarY*100*unitInBar;
		m_matrix2DTransform = D2D1::Matrix3x2F::Translation(0, 
			-m_scrolledTextPosition);
		m_renderTarget2D->SetTransform((pageTransform));
	}

	// Determine actual number of hit-test ranges
	DWRITE_TEXT_RANGE caretRange = GetSelectionRange();
	UINT32 actualHitTestCount = 0;

	if (caretRange.length > 0 && m_editable)
	{
		textLayout_->HitTestTextRange(
			caretRange.startPosition,
			caretRange.length,
			0, // x
			0, // y
			NULL,
			0, // metrics count
			&actualHitTestCount
			);
	}

	// Allocate enough room to return all hit-test metrics.
	std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTestCount);

	if (caretRange.length > 0 && m_editable)
	{
		textLayout_->HitTestTextRange(
			caretRange.startPosition,
			caretRange.length,
			0, // x
			0, // y
			&hitTestMetrics[0],
			static_cast<UINT32>(hitTestMetrics.size()),
			&actualHitTestCount
			);
	}

	// Draw the selection ranges behind the text.
	if (actualHitTestCount > 0 && m_editable)
	{
		// Note that an ideal layout will return fractional values,
		// so you may see slivers between the selection ranges due
		// to the per-primitive antialiasing of the edges unless
		// it is disabled (better for performance anyway).
		m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		//target.SetAntialiasing(false);

		for (size_t i = 0; i < actualHitTestCount; ++i)
		{
			const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
			D2D1_RECT_F highlightRect = {
				htm.left,
				htm.top,
				(htm.left + htm.width),
				(htm.top + htm.height)
			};

			m_renderTarget2D->FillRectangle(highlightRect, m_BrushCaretHighlight.Get());
		}

		m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}

	// Draw our caret onto the render target.
	if (m_editable && m_selected)
	{
		D2D1_RECT_F caretRect;
		GetCaretRect(caretRect);
		m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		m_renderTarget2D->FillRectangle(caretRect, m_BrushCursor.Get());
		m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_ALIASED);
	}
	m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	// Draw text
	m_renderTarget2D->DrawTextLayout(D2D1::Point2F(0.0f, .0f), textLayout_, m_brush.Get());
	//target.DrawTextLayout(textLayout_, pageRect);

	// Draw the selection ranges in front of images.
	// This shades otherwise opaque images so they are visibly selected,
	// checking the isText field of the hit-test metrics.
	if (actualHitTestCount > 0 && m_editable)
	{
		// Note that an ideal layout will return fractional values,
		// so you may see slivers between the selection ranges due
		// to the per-primitive antialiasing of the edges unless
		// it is disabled (better for performance anyway).
		m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		for (size_t i = 0; i < actualHitTestCount; ++i)
		{
			const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
			if (htm.isText)
				continue; // Only draw selection if not text.

			D2D1_RECT_F highlightRect = {
				htm.left,
				htm.top,
				(htm.left + htm.width),
				(htm.top + htm.height)
			};

			m_renderTarget2D->FillRectangle(highlightRect, m_BrushCaretHighlight.Get());
			//target.FillRectangle(highlightRect, *imageSelectionEffect_);
		}

		m_renderTarget2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_ALIASED);
	}

	// Restore transform
	m_renderTarget2D->SetTransform(&Cast(previousTransform));
	m_renderTarget2D->EndDraw();

	RenderOptions();
	//target.SetTransform(previousTransform);
	__super::Render();
}

void AngelGUISystem::GUI3D::AGText::GetViewMatrix(OUT DWRITE_MATRIX* matrix)
{
	// Generates a view matrix from the current origin, angle, and scale.
	using namespace AngelGUISystem::GUIGlobalTools;
	// Need the editor size for centering.
	RECT rect;
	GetClientRect(hwnd_, &rect);

	// Translate the origin to 0,0
	DWRITE_MATRIX translationMatrix = {
		1, 0,
		0, 1,
		-originX_, -originY_
	};

	// Scale and rotate
	double radians = DirectX::XMConvertToRadians(fmod(angle_, 360.0f));
	double cosValue = cos(radians);
	double sinValue = sin(radians);

	// If rotation is a quarter multiple, ensure sin and cos are exactly one of {-1,0,1}
	if (fmod(angle_, 90.0f) == 0)
	{
		cosValue = floor(cosValue + .5);
		sinValue = floor(sinValue + .5);
	}

	DWRITE_MATRIX rotationMatrix = {
		float(cosValue * scaleX_), float(sinValue * scaleX_),
		float(-sinValue * scaleY_), float(cosValue * scaleY_),
		0, 0
	};

	// Set the origin in the center of the window
	float centeringFactor = .5f;
	DWRITE_MATRIX centerMatrix = {
		1, 0,
		0, 1,
		floor(float(rect.right * centeringFactor)), floor(float(rect.bottom * centeringFactor))
	};

	D2D1::Matrix3x2F resultA, resultB;

	resultB.SetProduct(Cast(translationMatrix), Cast(rotationMatrix));
	resultA.SetProduct(resultB, Cast(centerMatrix));

	// For better pixel alignment (less blurry text)
	resultA._31 = floor(resultA._31);
	resultA._32 = floor(resultA._32);

	*matrix = *reinterpret_cast<DWRITE_MATRIX*>(&resultA);
	*matrix = AngelGUISystem::GUIGlobalTools::Cast(m_matrix2DTransform);
}


void AngelGUISystem::GUI3D::AGText::GetInverseViewMatrix(OUT DWRITE_MATRIX* matrix)
{
	// Inverts the view matrix for hit-testing and scrolling.

	DWRITE_MATRIX viewMatrix;
	GetViewMatrix(&viewMatrix);
	ComputeInverseMatrix(viewMatrix, *matrix);
}

DWRITE_TEXT_RANGE AngelGUISystem::GUI3D::AGText::GetSelectionRange()
{
	// Returns a valid range of the current selection,
	// regardless of whether the caret or anchor is first.

	UINT32 caretBegin = caretAnchor_;
	UINT32 caretEnd = caretPosition_ + caretPositionOffset_;
	if (caretBegin > caretEnd)
		std::swap(caretBegin, caretEnd);

	// Limit to actual text length.
	UINT32 textLength = static_cast<UINT32>(text_.size());
	caretBegin = DirectX::XMMin<UINT32>(caretBegin, textLength);
	caretEnd = DirectX::XMMin<UINT32>(caretEnd, textLength);

	DWRITE_TEXT_RANGE textRange = { caretBegin, caretEnd - caretBegin };
	return textRange;
}

UINT32 AngelGUISystem::GUI3D::AGText::GetCaretPosition()
{
	return caretPosition_ + caretPositionOffset_;
}

void AngelGUISystem::GUI3D::AGText::AlignCaretToNearestCluster(bool isTrailingHit, bool skipZeroWidth)
{
	// Uses hit-testing to align the current caret position to a whole cluster,
	// rather than residing in the middle of a base character + diacritic,
	// surrogate pair, or character + UVS.

	DWRITE_HIT_TEST_METRICS hitTestMetrics;
	float caretX, caretY;

	// Align the caret to the nearest whole cluster.
	textLayout_->HitTestTextPosition(
		caretPosition_,
		false,
		&caretX,
		&caretY,
		&hitTestMetrics
		);

	// The caret position itself is always the leading edge.
	// An additional offset indicates a trailing edge when non-zero.
	// This offset comes from the number of code-units in the
	// selected cluster or surrogate pair.
	caretPosition_ = hitTestMetrics.textPosition;
	caretPositionOffset_ = (isTrailingHit) ? hitTestMetrics.length : 0;

	// For invisible, zero-width characters (like line breaks
	// and formatting characters), force leading edge of the
	// next position.
	if (skipZeroWidth && hitTestMetrics.width == 0)
	{
		caretPosition_ += caretPositionOffset_;
		caretPositionOffset_ = 0;
	}
}

void AngelGUISystem::GUI3D::AGText::UpdateSystemCaret(const D2D1_RECT_F& rect)
{
	// Moves the system caret to a new position.
	using namespace AngelGUISystem::GUIGlobalTools;
	// Although we don't actually use the system caret (drawing our own
	// instead), this is important for accessibility, so the magnifier
	// can follow text we type. The reason we draw our own directly
	// is because intermixing DirectX and GDI content (the caret) reduces
	// performance.

	// Gets the current caret position (in untransformed space).

	if (GetFocus() != hwnd_) // Only update if we have focus.
		return;

	D2D1::Matrix3x2F pageTransform;
	GetViewMatrix(&Cast(pageTransform));

	// Transform caret top/left and size according to current scale and origin.
	D2D1_POINT_2F caretPoint = pageTransform.TransformPoint(D2D1::Point2F(rect.left, rect.top));

	float width = (rect.right - rect.left);
	float height = (rect.bottom - rect.top);
	float transformedWidth = width * pageTransform._11 + height * pageTransform._21;
	float transformedHeight = width * pageTransform._12 + height * pageTransform._22;

	// Update the caret's location, rounding to nearest integer so that
	// it lines up with the text selection.

	int intX = RoundToInt(caretPoint.x);
	int intY = RoundToInt(caretPoint.y);
	int intWidth = RoundToInt(transformedWidth);
	int intHeight = RoundToInt(caretPoint.y + transformedHeight) - intY;

	CreateCaret(hwnd_, NULL, intWidth, intHeight);
	SetCaretPos(intX, intY);

	// Don't actually call ShowCaret. It's enough to just set its position.
}

void AngelGUISystem::GUI3D::AGText::GetLineFromPosition(
	const DWRITE_LINE_METRICS* lineMetrics, // [lineCount]
	UINT32 lineCount,
	UINT32 textPosition,
	OUT UINT32* lineOut,
	OUT UINT32* linePositionOut
	)
{
	// Given the line metrics, determines the current line and starting text
	// position of that line by summing up the lengths. When the starting
	// line position is beyond the given text position, we have our line.
	using namespace AngelGUISystem::GUIGlobalTools;
	UINT32 line = 0;
	UINT32 linePosition = 0;
	UINT32 nextLinePosition = 0;
	for (; line < lineCount; ++line)
	{
		linePosition = nextLinePosition;
		nextLinePosition = linePosition + lineMetrics[line].length;
		if (nextLinePosition > textPosition)
		{
			// The next line is beyond the desired text position,
			// so it must be in the current line.
			break;
		}
	}
	*linePositionOut = linePosition;
	*lineOut = DirectX::XMMin<UINT32>(line, lineCount - 1);
	return;
}

void AngelGUISystem::GUI3D::AGText::DeleteSelection()
{
	// Deletes selection.

	DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
	if (selectionRange.length <= 0)
		return;

	layoutEditor_.RemoveTextAt(textLayout_, text_, selectionRange.startPosition, selectionRange.length);

	SetSelection(SetSelectionModeAbsoluteLeading, selectionRange.startPosition, false);
	RefreshView();
}

void AngelGUISystem::GUI3D::AGText::OnMousePress(UINT message, float x, float y)
{
	if (!m_editable)
		return;
	using namespace AngelGUISystem::GUIGlobalTools;
	MirrorXCoordinate(m_mousePosition.x);

	if (message == WM_LBUTTONDOWN)
	{
		if (m_intersectsCutOption && m_showOptions)
		{
			CopyToClipboard();
			DeleteSelection();
		}

		if (m_intersectsCopyOption&& m_showOptions)
		{
			CopyToClipboard();
		}

		if (m_intersectsPasteOption&& m_showOptions)
			PasteFromClipboard();

		float dist = 0.0f;
		//if (!m_useForButton)
			if (TestIntersects(&m_mousePosition, nullptr, &dist, x, y))
			{
				m_selected = true;
				m_currentColor = m_onHoverColor;
				if(!m_isMouseSelectedOptions)
					m_showOptions = false;
			}
			else
			{
				m_currentColor = m_defaultColor;
				m_selected = false;
			}

		if (m_renderScrollBar)
		{
			float dist = 0;
			if (m_scrollBar->TestIntersects(nullptr, nullptr, &dist, x, y))
			{
				m_scrollBarSelected = true;
			}
			else
			{
				m_scrollBarSelected = false;
			}
		}
		// Start dragging selection.
		currentlySelecting_ = true;

		bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
		SetSelectionFromPoint(m_mousePosition.x, m_mousePosition.y, heldShift);
	}
	else if (message == WM_RBUTTONDOWN)
	{
		// Start dragging selection.
		if (m_hovered)
		{
			m_updateOptionsTransform = true;
			m_showOptions = true;
		}
	}
	else if (message == WM_MBUTTONDOWN)
	{
		previousMouseX = m_mousePosition.x;
		previousMouseY = m_mousePosition.y;
		currentlyPanning_ = true;
	}
}

void AngelGUISystem::GUI3D::AGText::OnMouseRelease(UINT message, float x, float y)
{
	MirrorXCoordinate(m_mousePosition.x);

	if (message == WM_LBUTTONUP)
	{
		m_scrollBarSelected = false;
		currentlySelecting_ = false;
	}
	else if (message == WM_MBUTTONUP)
	{
		currentlyPanning_ = false;
	}
}


void AngelGUISystem::GUI3D::AGText::OnMouseMove(float x, float y)
{
	m_mousePositionScreen.x = x;
	m_mousePositionScreen.y = y;

	if (!m_editable)
		return;
	float dist = 0;
	//if (!m_useForButton)
	if (TestIntersects(&m_mousePosition, &m_pointInLocalSpace, &dist, x, y))
	{
		m_hovered = true;
		m_currentColor = m_onHoverColor;
	}
	else
	{
		m_currentColor = m_defaultColor;
		m_hovered = false;
	}
	if (m_renderScrollBar)
	{
		float dist = 0;
		if (m_scrollBar->TestIntersects(nullptr, nullptr, &dist, x, y))
		{
			m_scrollBarHovered = true;
		}
		else
		{
			m_scrollBarHovered = false;
		}
	}

	if (m_scrollBarSelected)
	{
		if (m_lastMouseY != y)
		{
			if (m_lastMouseY - y < 0.0f)
			{
				if (m_canScroll)
					m_scrollBarY += m_scrollBarSpeed;
			}
			else if (m_lastMouseY - y > 0.0f)
			{
				if (m_canScroll)
					m_scrollBarY -= m_scrollBarSpeed;
			}
		}
	}
	MirrorXCoordinate(m_mousePosition.x);

	if (currentlySelecting_)
	{
		// Drag current selection.
		SetSelectionFromPoint(m_mousePosition.x, m_mousePosition.y, true);
	}
	else if (currentlyPanning_)
	{
		DWRITE_MATRIX matrix;
		GetInverseViewMatrix(&matrix);

		float xDif = m_mousePosition.x - previousMouseX;
		float yDif = m_mousePosition.y - previousMouseY;
		previousMouseX = m_mousePosition.x;
		previousMouseY = m_mousePosition.y;

		originX_ -= (xDif * matrix.m11 + yDif * matrix.m21);
		originY_ -= (xDif * matrix.m12 + yDif * matrix.m22);
		ConstrainViewOrigin();

		RefreshView();
	}

	m_lastMouseX = x;
	m_lastMouseY = y;
}


void AngelGUISystem::GUI3D::AGText::OnMouseScroll(float xScroll, float yScroll)
{
	// Pans or scales the editor.

	bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
	bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

	if (heldControl)
	{
		// Scale
		float scaleFactor = (yScroll > 0) ? 1.0625f : 1 / 1.0625f;
		SetScale(scaleFactor, scaleFactor, true);
	}
	else
	{
		// Pan
		DWRITE_MATRIX matrix;
		GetInverseViewMatrix(&matrix);

		yScroll *= 10;
		xScroll *= 10; // for mice that support horizontal panning
		if (heldShift)
			std::swap(xScroll, yScroll);

		originX_ -= (xScroll * matrix.m11 + yScroll * matrix.m21);
		originY_ -= (xScroll * matrix.m12 + yScroll * matrix.m22);
		ConstrainViewOrigin();

		RefreshView();
	}
}

void AngelGUISystem::GUI3D::AGText::ConstrainViewOrigin()
{
	// Keep the page on-screen by not allowing the origin
	// to go outside the page bounds.

	D2D1_POINT_2F pageSize = GetPageSize(textLayout_);

	if (originX_ > pageSize.x)
		originX_ = pageSize.x;
	if (originX_ < 0)
		originX_ = 0;

	if (originY_ > pageSize.y)
		originY_ = pageSize.y;
	if (originY_ < 0)
		originY_ = 0;
}

void AngelGUISystem::GUI3D::AGText::OnMouseExit()
{
	currentlySelecting_ = false;
	currentlyPanning_ = false;
}

bool AngelGUISystem::GUI3D::AGText::SetSelectionFromPoint(float x, float y, bool extendSelection)
{
	// Returns the text position corresponding to the mouse x,y.
	// If hitting the trailing side of a cluster, return the
	// leading edge of the following text position.
	OutputDebugStringA("\nx : ");
	OutputDebugStringA(std::to_string(x).c_str());
	OutputDebugStringA("\ny : ");
	OutputDebugStringA(std::to_string(y).c_str());
	BOOL isTrailingHit;
	BOOL isInside;
	DWRITE_HIT_TEST_METRICS caretMetrics;

	// Remap display coordinates to actual.
	DWRITE_MATRIX matrix;
	GetInverseViewMatrix(&matrix);

	float transformedX = (x * matrix.m11 + y * matrix.m21 + matrix.dx);
	float transformedY = (x * matrix.m12 + y * matrix.m22 + matrix.dy);

	textLayout_->HitTestPoint(
		transformedX,
		transformedY,
		&isTrailingHit,
		&isInside,
		&caretMetrics
		);

	// Update current selection according to click or mouse drag.
	SetSelection(
		isTrailingHit ? SetSelectionModeAbsoluteTrailing : SetSelectionModeAbsoluteLeading,
		caretMetrics.textPosition,
		extendSelection
		);

	return true;
}

void AngelGUISystem::GUI3D::AGText::MirrorXCoordinate(IN OUT float& x)
{
	// On RTL builds, coordinates may need to be restored to or converted
	// from Cartesian coordinates, where x increases positively to the right.
	if (GetWindowLong(hwnd_, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
	{
		RECT rect;
		GetClientRect(hwnd_, &rect);
		x = float(rect.right) - x - 1;
	}
}

void AngelGUISystem::GUI3D::AGText::OnKeyPress(UINT32 keyCode)
{
	// Handles caret navigation and special presses that
	// do not generate characters.
	if (!m_editable)
		return;
	if (!m_selected)
		return;
	bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
	bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
	bool lShift = (GetKeyState(VK_LSHIFT) & 0x80) != 0;
	bool RShift = (GetKeyState(VK_RSHIFT) & 0x80) != 0;
	bool RCtrl = (GetKeyState(VK_RCONTROL) & 0x80) != 0;
	bool LCtrl = (GetKeyState(VK_LCONTROL) & 0x80) != 0;

	if (RCtrl && lShift)
		textLayout_->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
	if (LCtrl && RShift)
		textLayout_->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);

	UINT32 absolutePosition = caretPosition_ + caretPositionOffset_;

	using namespace AngelGUISystem::GUIGlobalTools;

	switch (keyCode)
	{
	case VK_RETURN:
		// Insert CR/LF pair
		DeleteSelection();
		layoutEditor_.InsertTextAt(textLayout_, text_, absolutePosition, L"\r\n", 2, &caretFormat_);
		SetSelection(SetSelectionModeAbsoluteLeading, absolutePosition + 2, false, false);
		RefreshView();
		break;

	case VK_BACK:
		// Erase back one character (less than a character though).
		// Since layout's hit-testing always returns a whole cluster,
		// we do the surrogate pair detection here directly. Otherwise
		// there would be no way to delete just the diacritic following
		// a base character.

		if (absolutePosition != caretAnchor_)
		{
			// delete the selected text
			DeleteSelection();
		}
		else if (absolutePosition > 0)
		{
			UINT32 count = 1;
			// Need special case for surrogate pairs and CR/LF pair.
			if (absolutePosition >= 2
				&& absolutePosition <= text_.size())
			{
				wchar_t charBackOne = text_[absolutePosition - 1];
				wchar_t charBackTwo = text_[absolutePosition - 2];
				if ((IsLowSurrogate(charBackOne) && IsHighSurrogate(charBackTwo))
					|| (charBackOne == '\n' && charBackTwo == '\r'))
				{
					count = 2;
				}
			}
			SetSelection(SetSelectionModeLeftChar, count, false);
			layoutEditor_.RemoveTextAt(textLayout_, text_, caretPosition_, count);
			RefreshView();
		}
		break;

	case VK_DELETE:
		// Delete following cluster.

		if (absolutePosition != caretAnchor_)
		{
			// Delete all the selected text.
			DeleteSelection();
		}
		else
		{
			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			float caretX, caretY;

			// Get the size of the following cluster.
			textLayout_->HitTestTextPosition(
				absolutePosition,
				false,
				&caretX,
				&caretY,
				&hitTestMetrics
				);

			layoutEditor_.RemoveTextAt(textLayout_, text_, hitTestMetrics.textPosition, hitTestMetrics.length);

			SetSelection(SetSelectionModeAbsoluteLeading, hitTestMetrics.textPosition, false);
			RefreshView();
		}
		break;

	case VK_TAB:
		break; // want tabs

	case VK_LEFT: // seek left one cluster
		SetSelection(heldControl ? SetSelectionModeLeftWord : SetSelectionModeLeft, 1, heldShift);
		break;

	case VK_RIGHT: // seek right one cluster
		SetSelection(heldControl ? SetSelectionModeRightWord : SetSelectionModeRight, 1, heldShift);
		break;

	case VK_UP: // up a line
		SetSelection(SetSelectionModeUp, 1, heldShift);
		break;

	case VK_DOWN: // down a line
		SetSelection(SetSelectionModeDown, 1, heldShift);
		break;

	case VK_HOME: // beginning of line
		SetSelection(heldControl ? SetSelectionModeFirst : SetSelectionModeHome, 0, heldShift);
		break;

	case VK_END: // end of line
		SetSelection(heldControl ? SetSelectionModeLast : SetSelectionModeEnd, 0, heldShift);
		break;

	case 'C':
		if (heldControl)
			CopyToClipboard();
		break;

	case VK_INSERT:
		if (heldControl)
			CopyToClipboard();
		else if (heldShift)
			PasteFromClipboard();
		break;

	case 'V':
		if (heldControl)
			PasteFromClipboard();
		break;

	case 'X':
		if (heldControl)
		{
			CopyToClipboard();
			DeleteSelection();
		}
		break;

	case 'A':
		if (heldControl)
			SetSelection(SetSelectionModeAll, 0, true);
		break;
	}
}


void AngelGUISystem::GUI3D::AGText::OnKeyCharacter(UINT32 charCode)
{
	// Inserts text characters.
	if (!m_editable)
		return;
	if (!m_selected)
		return;
	// Allow normal characters and tabs
	if (charCode >= 0x20 || charCode == 9)
	{
		// Replace any existing selection.
		DeleteSelection();

		// Convert the UTF32 character code from the Window message to UTF16,
		// yielding 1-2 code-units. Then advance the caret position by how
		// many code-units were inserted.

		UINT32 charsLength = 1;
		wchar_t chars[2] = { static_cast<wchar_t>(charCode), 0 };

		// If above the basic multi-lingual plane, split into
		// leading and trailing surrogates.
		if (charCode > 0xFFFF)
		{
			// From http://unicode.org/faq/utf_bom.html#35
			chars[0] = wchar_t(0xD800 + (charCode >> 10) - (0x10000 >> 10));
			chars[1] = wchar_t(0xDC00 + (charCode & 0x3FF));
			charsLength++;
		}
		layoutEditor_.InsertTextAt(textLayout_, text_, caretPosition_ + caretPositionOffset_, chars, charsLength, &caretFormat_);
		SetSelection(SetSelectionModeRight, charsLength, false, false);

		RefreshView();
	}
}

void AngelGUISystem::GUI3D::AGText::CopyToClipboard()
{
	// Copies selected text to clipboard.

	DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
	if (selectionRange.length <= 0)
		return;

	// Open and empty existing contents.
	if (OpenClipboard(hwnd_))
	{
		if (EmptyClipboard())
		{
			// Allocate room for the text
			size_t byteSize = sizeof(wchar_t) * (selectionRange.length + 1);
			HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, byteSize);

			if (hClipboardData != NULL)
			{
				void* memory = GlobalLock(hClipboardData);  // [byteSize] in bytes

				if (memory != NULL)
				{
					// Copy text to memory block.
					const wchar_t* text = text_.c_str();
					memcpy(memory, &text[selectionRange.startPosition], byteSize);
					GlobalUnlock(hClipboardData);

					if (SetClipboardData(CF_UNICODETEXT, hClipboardData) != NULL)
					{
						hClipboardData = NULL; // system now owns the clipboard, so don't touch it.
					}
				}
				GlobalFree(hClipboardData); // free if failed
			}
		}
		CloseClipboard();
	}
}

void AngelGUISystem::GUI3D::AGText::UpdateScrollInfo()
{
	// Updates scroll bars.
	using namespace AngelGUISystem::GUIGlobalTools;
	if (textLayout_ == NULL)
		return;

	// Determine scroll bar's step size in pixels by multiplying client rect by current view.
	RECT clientRect;
	GetClientRect(hwnd_, &clientRect);

	D2D1::Matrix3x2F pageTransform;
	GetInverseViewMatrix(&Cast(pageTransform));

	// Transform vector of viewport size
	D2D1_POINT_2F clientSize = { float(clientRect.right), float(clientRect.bottom) };
	D2D1_POINT_2F scaledSize = { clientSize.x * pageTransform._11 + clientSize.y * pageTransform._21,
		clientSize.x * pageTransform._12 + clientSize.y * pageTransform._22 };

	float x = originX_;
	float y = originY_;
	D2D1_POINT_2F pageSize = GetPageSize(textLayout_);

	SCROLLINFO scrollInfo = { sizeof(scrollInfo) };
	scrollInfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

	if (IsLandscapeAngle(angle_))
	{
		std::swap(x, y);
		std::swap(pageSize.x, pageSize.y);
		std::swap(scaledSize.x, scaledSize.y);
	}

	// Set vertical scroll bar.
	scrollInfo.nPage = int(abs(scaledSize.y));
	scrollInfo.nPos = int(scaledSize.y >= 0 ? y : pageSize.y - y);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = int(pageSize.y) + scrollInfo.nPage;
	SetScrollInfo(hwnd_, SB_VERT, &scrollInfo, TRUE);
	scrollInfo.nPos = 0;
	scrollInfo.nMax = 0;
	GetScrollInfo(hwnd_, SB_VERT, &scrollInfo);

	// Set horizontal scroll bar.
	scrollInfo.nPage = int(abs(scaledSize.x));
	scrollInfo.nPos = int(scaledSize.x >= 0 ? x : pageSize.x - x);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = int(pageSize.x) + scrollInfo.nPage;
	SetScrollInfo(hwnd_, SB_HORZ, &scrollInfo, TRUE);
}

void AngelGUISystem::GUI3D::AGText::OnDestroy()
{
	using namespace AngelGUISystem::GUIGlobalTools;
	SafeRelease(m_renderTarget2D.GetAddressOf());

}


void AngelGUISystem::GUI3D::AGText::PasteFromClipboard()
{
	// Pastes text from clipboard at current caret position.

	DeleteSelection();

	UINT32 characterCount = 0;

	// Copy Unicode text from clipboard.

	if (OpenClipboard(hwnd_))
	{
		HGLOBAL hClipboardData = GetClipboardData(CF_UNICODETEXT);

		if (hClipboardData != NULL)
		{
			// Get text and size of text.
			size_t byteSize = GlobalSize(hClipboardData);
			void* memory = GlobalLock(hClipboardData); // [byteSize] in bytes
			const wchar_t* text = reinterpret_cast<const wchar_t*>(memory);
			characterCount = static_cast<UINT32>(wcsnlen(text, byteSize / sizeof(wchar_t)));

			if (memory != NULL)
			{
				// Insert the text at the current position.
				layoutEditor_.InsertTextAt(
					textLayout_,
					text_,
					caretPosition_ + caretPositionOffset_,
					text,
					characterCount
					);
				GlobalUnlock(hClipboardData);
			}
		}
		CloseClipboard();
	}

	SetSelection(SetSelectionModeRightChar, characterCount, true);
	RefreshView();
}

void AngelGUISystem::GUI3D::AGText::RefreshView()
{
	// Redraws the text and scrollbars.

	UpdateScrollInfo();
	PostRedraw();
}


void AngelGUISystem::GUI3D::AGText::GetLineMetrics(
	OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics
	)
{
	// Retrieves the line metrics, used for caret navigation, up/down and home/end.

	DWRITE_TEXT_METRICS textMetrics;
	textLayout_->GetMetrics(&textMetrics);

	lineMetrics.resize(textMetrics.lineCount);
	textLayout_->GetLineMetrics(&lineMetrics.front(), textMetrics.lineCount, &textMetrics.lineCount);
}


void AngelGUISystem::GUI3D::AGText::UpdateCaretFormatting()
{
	UINT32 currentPos = caretPosition_ + caretPositionOffset_;

	if (currentPos > 0)
	{
		--currentPos; // Always adopt the trailing properties.
	}

	// Get the family name
	caretFormat_.fontFamilyName[0] = '\0';
	textLayout_->GetFontFamilyName(currentPos, &caretFormat_.fontFamilyName[0], ARRAYSIZE(caretFormat_.fontFamilyName));

	// Get the locale
	caretFormat_.localeName[0] = '\0';
	textLayout_->GetLocaleName(currentPos, &caretFormat_.localeName[0], ARRAYSIZE(caretFormat_.localeName));

	// Get the remaining attributes...
	textLayout_->GetFontWeight(currentPos, &caretFormat_.fontWeight);
	textLayout_->GetFontStyle(currentPos, &caretFormat_.fontStyle);
	textLayout_->GetFontStretch(currentPos, &caretFormat_.fontStretch);
	textLayout_->GetFontSize(currentPos, &caretFormat_.fontSize);
	textLayout_->GetUnderline(currentPos, &caretFormat_.hasUnderline);
	textLayout_->GetStrikethrough(currentPos, &caretFormat_.hasStrikethrough);

	// Get the current color.
	IUnknown* drawingEffect = NULL;
	textLayout_->GetDrawingEffect(currentPos, &drawingEffect);
	caretFormat_.color = 0;
	if (drawingEffect != NULL)
	{
		DrawingEffect& effect = *reinterpret_cast<DrawingEffect*>(drawingEffect);
		caretFormat_.color = effect.GetColor();
	}
	using namespace AngelGUISystem::GUIGlobalTools;
	SafeRelease(&drawingEffect);
}

bool AngelGUISystem::GUI3D::AGText::SetSelection(SetSelectionMode moveMode, UINT32 advance, bool extendSelection, bool updateCaretFormat)
{
	// Moves the caret relatively or absolutely, optionally extending the
	// selection range (for example, when shift is held).

	UINT32 line = UINT32_MAX; // current line number, needed by a few modes
	UINT32 absolutePosition = caretPosition_ + caretPositionOffset_;
	UINT32 oldAbsolutePosition = absolutePosition;
	UINT32 oldCaretAnchor = caretAnchor_;

	switch (moveMode)
	{
	case SetSelectionModeLeft:
		caretPosition_ += caretPositionOffset_;
		if (caretPosition_ > 0)
		{
			--caretPosition_;
			AlignCaretToNearestCluster(false, true);

			// special check for CR/LF pair
			absolutePosition = caretPosition_ + caretPositionOffset_;
			if (absolutePosition >= 1
				&& absolutePosition < text_.size()
				&& text_[absolutePosition - 1] == '\r'
				&&  text_[absolutePosition] == '\n')
			{
				caretPosition_ = absolutePosition - 1;
				AlignCaretToNearestCluster(false, true);
			}
		}
		break;

	case SetSelectionModeRight:
		caretPosition_ = absolutePosition;
		AlignCaretToNearestCluster(true, true);

		// special check for CR/LF pair
		absolutePosition = caretPosition_ + caretPositionOffset_;
		if (absolutePosition >= 1
			&& absolutePosition < text_.size()
			&& text_[absolutePosition - 1] == '\r'
			&&  text_[absolutePosition] == '\n')
		{
			caretPosition_ = absolutePosition + 1;
			AlignCaretToNearestCluster(false, true);
		}
		break;

	case SetSelectionModeLeftChar:
		caretPosition_ = absolutePosition;
		caretPosition_ -= DirectX::XMMin<UINT32>(advance, absolutePosition);
		caretPositionOffset_ = 0;
		break;

	case SetSelectionModeRightChar:
		caretPosition_ = absolutePosition + advance;
		caretPositionOffset_ = 0;
		{
			// Use hit-testing to limit text position.
			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			float caretX, caretY;

			textLayout_->HitTestTextPosition(
				caretPosition_,
				false,
				&caretX,
				&caretY,
				&hitTestMetrics
				);
			caretPosition_ = DirectX::XMMin<UINT32>(caretPosition_, hitTestMetrics.textPosition + hitTestMetrics.length);
		}
		break;

	case SetSelectionModeUp:
	case SetSelectionModeDown:
	{
		// Retrieve the line metrics to figure out what line we are on.
		std::vector<DWRITE_LINE_METRICS> lineMetrics;
		GetLineMetrics(lineMetrics);

		UINT32 linePosition;
		GetLineFromPosition(
			&lineMetrics.front(),
			static_cast<UINT32>(lineMetrics.size()),
			caretPosition_,
			&line,
			&linePosition
			);

		// Move up a line or down
		if (moveMode == SetSelectionModeUp)
		{
			if (line <= 0)
				break; // already top line
			line--;
			linePosition -= lineMetrics[line].length;
		}
		else
		{
			linePosition += lineMetrics[line].length;
			line++;
			if (line >= lineMetrics.size())
				break; // already bottom line
		}

		// To move up or down, we need three hit-testing calls to determine:
		// 1. The x of where we currently are.
		// 2. The y of the new line.
		// 3. New text position from the determined x and y.
		// This is because the characters are variable size.

		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		float caretX, caretY, dummyX;

		// Get x of current text position
		textLayout_->HitTestTextPosition(
			caretPosition_,
			caretPositionOffset_ > 0, // trailing if nonzero, else leading edge
			&caretX,
			&caretY,
			&hitTestMetrics
			);

		// Get y of new position
		textLayout_->HitTestTextPosition(
			linePosition,
			false, // leading edge
			&dummyX,
			&caretY,
			&hitTestMetrics
			);

		// Now get text position of new x,y.
		BOOL isInside, isTrailingHit;
		textLayout_->HitTestPoint(
			caretX,
			caretY,
			&isTrailingHit,
			&isInside,
			&hitTestMetrics
			);

		caretPosition_ = hitTestMetrics.textPosition;
		caretPositionOffset_ = isTrailingHit ? (hitTestMetrics.length > 0) : 0;
	}
	break;

	case SetSelectionModeLeftWord:
	case SetSelectionModeRightWord:
	{
		// To navigate by whole words, we look for the canWrapLineAfter
		// flag in the cluster metrics.

		// First need to know how many clusters there are.
		std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics;
		UINT32 clusterCount;
		textLayout_->GetClusterMetrics(NULL, 0, &clusterCount);

		if (clusterCount == 0)
			break;

		// Now we actually read them.
		clusterMetrics.resize(clusterCount);
		textLayout_->GetClusterMetrics(&clusterMetrics.front(), clusterCount, &clusterCount);

		caretPosition_ = absolutePosition;

		UINT32 clusterPosition = 0;
		UINT32 oldCaretPosition = caretPosition_;

		if (moveMode == SetSelectionModeLeftWord)
		{
			// Read through the clusters, keeping track of the farthest valid
			// stopping point just before the old position.
			caretPosition_ = 0;
			caretPositionOffset_ = 0; // leading edge
			for (UINT32 cluster = 0; cluster < clusterCount; ++cluster)
			{
				clusterPosition += clusterMetrics[cluster].length;
				if (clusterMetrics[cluster].canWrapLineAfter)
				{
					if (clusterPosition >= oldCaretPosition)
						break;

					// Update in case we pass this point next loop.
					caretPosition_ = clusterPosition;
				}
			}
		}
		else // SetSelectionModeRightWord
		{
			// Read through the clusters, looking for the first stopping point
			// after the old position.
			for (UINT32 cluster = 0; cluster < clusterCount; ++cluster)
			{
				UINT32 clusterLength = clusterMetrics[cluster].length;
				caretPosition_ = clusterPosition;
				caretPositionOffset_ = clusterLength; // trailing edge
				if (clusterPosition >= oldCaretPosition && clusterMetrics[cluster].canWrapLineAfter)
					break; // first stopping point after old position.

				clusterPosition += clusterLength;
			}
		}
	}
	break;

	case SetSelectionModeHome:
	case SetSelectionModeEnd:
	{
		// Retrieve the line metrics to know first and last position
		// on the current line.
		std::vector<DWRITE_LINE_METRICS> lineMetrics;
		GetLineMetrics(lineMetrics);

		GetLineFromPosition(
			&lineMetrics.front(),
			static_cast<UINT32>(lineMetrics.size()),
			caretPosition_,
			&line,
			&caretPosition_
			);

		caretPositionOffset_ = 0;
		if (moveMode == SetSelectionModeEnd)
		{
			// Place the caret at the last character on the line,
			// excluding line breaks. In the case of wrapped lines,
			// newlineLength will be 0.
			UINT32 lineLength = lineMetrics[line].length - lineMetrics[line].newlineLength;
			caretPositionOffset_ = DirectX::XMMin<UINT32>(lineLength, 1u);
			caretPosition_ += lineLength - caretPositionOffset_;
			AlignCaretToNearestCluster(true);
		}
	}
	break;

	case SetSelectionModeFirst:
		caretPosition_ = 0;
		caretPositionOffset_ = 0;
		break;

	case SetSelectionModeAll:
		caretAnchor_ = 0;
		extendSelection = true;
		__fallthrough;

	case SetSelectionModeLast:
		caretPosition_ = UINT32_MAX;
		caretPositionOffset_ = 0;
		AlignCaretToNearestCluster(true);
		break;

	case SetSelectionModeAbsoluteLeading:
		caretPosition_ = advance;
		caretPositionOffset_ = 0;
		break;

	case SetSelectionModeAbsoluteTrailing:
		caretPosition_ = advance;
		AlignCaretToNearestCluster(true);
		break;
	}

	absolutePosition = caretPosition_ + caretPositionOffset_;

	if (!extendSelection)
		caretAnchor_ = absolutePosition;

	bool caretMoved = (absolutePosition != oldAbsolutePosition)
		|| (caretAnchor_ != oldCaretAnchor);

	if (caretMoved)
	{
		// update the caret formatting attributes
		if (updateCaretFormat)
			UpdateCaretFormatting();

		PostRedraw();

		D2D1_RECT_F rect;
		GetCaretRect(rect);
		UpdateSystemCaret(rect);
	}

	return caretMoved;
}

void AngelGUISystem::GUI3D::AGText::Update(float dt,
	DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
	using namespace DirectX;
	if (m_editable)
	{
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
		DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationY(m_rotation.y);
		//---------------------- Copy
		
		XMVECTOR mouseInWorld = XMVectorSet(0, 0, 0, 0);
		XMMATRIX translation = XMMatrixIdentity();
		if (m_updateOptionsTransform)
		{
			mouseInWorld = XMVector3TransformCoord(XMVectorSet(m_pointInLocalSpace.x + 0.1f,
				m_pointInLocalSpace.y - 0.1f, 0.0f, 1.0f), DirectX::XMLoadFloat4x4(&m_worldMatrix));
			m_copyOptionPosition = DirectX::XMFLOAT3(mouseInWorld.m128_f32[0],
				mouseInWorld.m128_f32[1], mouseInWorld.m128_f32[2] + 0.1);
			mouseInWorld = XMVector3TransformCoord(XMVectorSet(m_pointInLocalSpace.x + 0.1f,
				m_pointInLocalSpace.y - 0.3f, 0.0f, 1.0f), DirectX::XMLoadFloat4x4(&m_worldMatrix));
			m_pasteOptionPosition = DirectX::XMFLOAT3(mouseInWorld.m128_f32[0],
				mouseInWorld.m128_f32[1], mouseInWorld.m128_f32[2] + 0.1);
			mouseInWorld = XMVector3TransformCoord(XMVectorSet(m_pointInLocalSpace.x + 0.1f,
				m_pointInLocalSpace.y - 0.5f, 0.0f, 1.0f), DirectX::XMLoadFloat4x4(&m_worldMatrix));
			m_cutOptionPosition = DirectX::XMFLOAT3(mouseInWorld.m128_f32[0],
				mouseInWorld.m128_f32[1], mouseInWorld.m128_f32[2] + 0.1);
			mouseInWorld = XMVector3TransformCoord(XMVectorSet(m_pointInLocalSpace.x + 0.1f,
				m_pointInLocalSpace.y - 0.7f, 0.0f, 1.0f), DirectX::XMLoadFloat4x4(&m_worldMatrix));
			m_fontOptionPosition = DirectX::XMFLOAT3(mouseInWorld.m128_f32[0],
				mouseInWorld.m128_f32[1], mouseInWorld.m128_f32[2] + 0.1);

		}
		translation = XMMatrixTranslation(m_copyOptionPosition.x,
			m_copyOptionPosition.y, m_copyOptionPosition.z + 0.1);
		XMMATRIX scale = XMMatrixScaling(m_scale.x/10,
			m_scale.y / 10
			, m_scale.z / 10);

		DirectX::XMStoreFloat4x4(&m_copyOptionWorld, scale*rotation*translation);
		//----------------------------------------------------------------------------

		//--------------------- Paste
		translation = XMMatrixTranslation(m_pasteOptionPosition.x,
			m_pasteOptionPosition.y, m_pasteOptionPosition.z + 0.1);

		DirectX::XMStoreFloat4x4(&m_pasteOptionWorld, scale*rotation*translation);
		//-----------------------------------------------------------------------------

		//--------------------- Cut
		translation = XMMatrixTranslation(m_cutOptionPosition.x,
			m_cutOptionPosition.y, m_cutOptionPosition.z + 0.1);

		DirectX::XMStoreFloat4x4(&m_cutOptionWorld, scale*rotation*translation);
		//-----------------------------------------------------------------------------

		//--------------------- Font
		translation = XMMatrixTranslation(m_fontOptionPosition.x,
			m_fontOptionPosition.y, m_fontOptionPosition.z + 0.1);

		DirectX::XMStoreFloat4x4(&m_fontOptionWorld, scale*rotation*translation);
		//-----------------------------------------------------------------------------

		m_updateOptionsTransform = false;
	}
	float dist = 0;
	XMMATRIX invWorld = XMMatrixIdentity();
	invWorld = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&m_copyOptionWorld))
		, DirectX::XMLoadFloat4x4(&m_copyOptionWorld));

	m_intersectsCopyOption =
		TestIntersects(
			invWorld, &dist, m_mousePositionScreen.x, m_mousePositionScreen.y);

	invWorld = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&m_pasteOptionWorld))
		, DirectX::XMLoadFloat4x4(&m_pasteOptionWorld));

	m_intersectsPasteOption =
		TestIntersects(
			invWorld, &dist, m_mousePositionScreen.x, m_mousePositionScreen.y);

	invWorld = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&m_cutOptionWorld))
		, DirectX::XMLoadFloat4x4(&m_cutOptionWorld));

	m_intersectsCutOption =
		TestIntersects(
			invWorld, &dist, m_mousePositionScreen.x, m_mousePositionScreen.y);

	invWorld = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&m_fontOptionWorld))
		, DirectX::XMLoadFloat4x4(&m_fontOptionWorld));

	m_intersectsfontOption =
		TestIntersects(
			invWorld, &dist, m_mousePositionScreen.x, m_mousePositionScreen.y);


	if (m_intersectsCopyOption)
	{
		m_cbColorOptionsCopy.Const.color = DirectX::XMFLOAT4(
			0.52f,
			0.80f,
			0.98f,
			1.0f);
		m_isMouseSelectedOptions = true;
	}
	else
	{
		m_cbColorOptionsCopy.Const.color = DirectX::XMFLOAT4(1, 1, 1, 1);
		m_isMouseSelectedOptions = false;
	}

	if (m_intersectsPasteOption)
	{
		m_cbColorOptionsPaste.Const.color = DirectX::XMFLOAT4(
			0.52f,
			0.80f,
			0.98f,
			1.0f);
		m_isMouseSelectedOptions = true;
	}
	else
	{
		m_cbColorOptionsPaste.Const.color = DirectX::XMFLOAT4(1, 1, 1, 1);
		m_isMouseSelectedOptions = false;
	}

	if (m_intersectsCutOption)
	{
		m_cbColorOptionsCut.Const.color = DirectX::XMFLOAT4(
			0.52f,
			0.80f,
			0.98f,
			1.0f);
		m_isMouseSelectedOptions = true;
	}
	else
	{
		m_cbColorOptionsCut.Const.color = DirectX::XMFLOAT4(1, 1, 1, 1);
		m_isMouseSelectedOptions = false;
	}

	if (m_intersectsfontOption)
	{
		m_cbColorOptionsFont.Const.color = DirectX::XMFLOAT4(
			0.52f,
			0.80f,
			0.98f,
			1.0f);
		m_isMouseSelectedOptions = true;
	}
	else
	{
		m_cbColorOptionsFont.Const.color = DirectX::XMFLOAT4(1, 1, 1, 1);
		m_isMouseSelectedOptions = false;
	}

	XMStoreFloat4x4(&this->m_cbWorldCopy.Const.WVP,
		XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_copyOptionWorld)
			*view
			*projection));

	XMStoreFloat4x4(&this->m_cbWorldPaste.Const.WVP,
		XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_pasteOptionWorld)
			*view
			*projection));

	XMStoreFloat4x4(&this->m_cbWorldCut.Const.WVP,
		XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_cutOptionWorld)
			*view
			*projection));

	XMStoreFloat4x4(&this->m_cbWorldfont.Const.WVP,
		XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_fontOptionWorld)
			*view
			*projection));

	__super::Update(dt, view, projection);
	
}

void AngelGUISystem::GUI3D::AGText::SetText(const wchar_t *_newText)
{
	text_.clear();
	text_.assign(_newText);
	HR(AngelGUI::GetDirectWriteFactory()->CreateTextLayout(
		text_.c_str(),
		static_cast<UINT32>(text_.size()),
		m_textFormat.Get(),
		m_rectangle.right,            // maximum width
		m_rectangle.bottom,            // maximum height
		&textLayout_
		));

}

void AngelGUISystem::GUI3D::AGText::SetText(std::string & txt)
{
	wchar_t * wtxt = new wchar_t[txt.size() + 1];
	for (unsigned int i = 0;i < txt.size();i++)
		wtxt[i] = txt[i];
	wtxt[txt.size()] = L'\0';
	SetText(wtxt);
	delete wtxt;
}

int AngelGUISystem::GUI3D::AGText::GetWcharSize(wchar_t text[])
{
	wchar_t * start = &text[0];
	int ret = 0;
	while (std::wcscmp(start, L"") != 0)
	{
		ret++;
		start++;
	}
	return ret;
}

void AngelGUISystem::GUI3D::AGText::SetActive(bool b)
{
	m_active = b;
}

void AngelGUISystem::GUI3D::AGText::SetActive(bool &b)
{
	if (b == true)
		m_active = true;
	if (b == false)
		m_active = false;
}

void AngelGUISystem::GUI3D::AGText::Destroy()
{
	COM_RELEASE(m_brush);
	COM_RELEASE(m_BrushCursor);
	COM_RELEASE(m_BrushCaretHighlight);
}

void AngelGUISystem::GUI3D::AGText::ScaleDownFont(float amount)
{
	m_font->size -= amount;
	ReCreateFont();
}

void AngelGUISystem::GUI3D::AGText::ReCreateFont()
{
	COM_RELEASE(m_textFormat);
	AngelGUI::GetDirectWriteFactory()->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_font->size,
		L"",
		&m_textFormat
		);
	textLayout_->Release();
	textLayout_ = nullptr;
	HR(AngelGUI::GetDirectWriteFactory()->CreateTextLayout(
		text_.c_str(),
		static_cast<UINT32>(text_.size()),
		m_textFormat.Get(),
		m_rectangle.right,            // maximum width
		m_rectangle.bottom,            // maximum height
		&textLayout_
		));
}


void AngelGUISystem::GUI3D::AGText::SetEditable(bool b)
{
	m_editable = b;
}

void AngelGUISystem::GUI3D::AGText::SetEditable(bool &b)
{
	if (b == true)
		m_editable = true;
	else
		m_editable = false;
}

void AngelGUISystem::GUI3D::AGText::SetTextColor(float &r, float  &g, float  &ba, float  &a)
{
	m_brush->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI3D::AGText::SetTextColor(float r, float  g, float  ba, float  a)
{
	m_brush->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI3D::AGText::SetTextColor(DirectX::XMFLOAT4 color)
{
	m_defaultColor = color;
	m_brush->SetColor(D2D1::ColorF(color.x, color.y, color.z, color.w));
}

void AngelGUISystem::GUI3D::AGText::SetCaretColor(float &r, float  &g, float  &ba, float  &a)
{
	m_BrushCursor->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI3D::AGText::RenderOptions()
{
	if (!m_selected)
		return;
	if (m_showOptions && m_editable)
	{
		//Render Options
		//--------------------------------- Copy
		this->m_cbWorldCopy.Update();
		this->m_shader->SetConstantBuffer(0, 1, m_cbWorldCopy.getBuffer());
		this->m_cbColorOptionsCopy.Update();
		this->m_shader->SetConstantBuffer(1, 1, m_cbColorOptionsCopy.getBuffer());
		this->m_shader->BindSRV(0, 1, this->m_copyTexture.Get(),
			AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);

		this->m_shader->SetSampler(0, 1, AngelGUI::GetSamplerState().GetAddressOf());
		this->m_stride = sizeof(VertexType);
		this->m_offset = 0;
		this->m_shader->Apply();
		{
			AngelGUI::GetDeviceContext3D()
				->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &this->m_stride, &this->m_offset);
			AngelGUI::GetDeviceContext3D()
				->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			AngelGUI::GetDeviceContext3D()
				->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			AngelGUI::GetDeviceContext3D()
				->DrawIndexed(6, 0, 0);
		}
		m_shader->UnBindSRV(0, 1, AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
		m_shader->Disable();
		//--------------------------- 

		////-------------------- Paste
		this->m_cbWorldPaste.Update();
		this->m_shader->SetConstantBuffer(0, 1, m_cbWorldPaste.getBuffer());
		this->m_cbColorOptionsPaste.Update();
		this->m_shader->SetConstantBuffer(1, 1, m_cbColorOptionsPaste.getBuffer());
		this->m_shader->BindSRV(0, 1, this->m_pasteTexture.Get(), 
			AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);

		this->m_shader->SetSampler(0, 1, AngelGUI::GetSamplerState().GetAddressOf());
		this->m_stride = sizeof(VertexType);
		this->m_offset = 0;
		this->m_shader->Apply();
		{
			AngelGUI::GetDeviceContext3D()
				->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &this->m_stride, &this->m_offset);
			AngelGUI::GetDeviceContext3D()
				->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			AngelGUI::GetDeviceContext3D()
				->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			AngelGUI::GetDeviceContext3D()
				->DrawIndexed(6, 0, 0);
		}
		m_shader->UnBindSRV(0, 1, AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
		m_shader->Disable();
		//----------------------------

		//-------------------- Cut
		this->m_cbWorldCut.Update();
		this->m_shader->SetConstantBuffer(0, 1, m_cbWorldCut.getBuffer());
		this->m_cbColorOptionsCut.Update();
		this->m_shader->SetConstantBuffer(1, 1, m_cbColorOptionsCut.getBuffer());
		this->m_shader->BindSRV(0, 1, this->m_cutTexture.Get(),
			AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
		this->m_shader->SetSampler(0, 1, AngelGUI::GetSamplerState().GetAddressOf());
		this->m_stride = sizeof(VertexType);
		this->m_offset = 0;
		this->m_shader->Apply();
		{
			AngelGUI::GetDeviceContext3D()
				->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &this->m_stride, &this->m_offset);
			AngelGUI::GetDeviceContext3D()
				->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			AngelGUI::GetDeviceContext3D()
				->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			AngelGUI::GetDeviceContext3D()
				->DrawIndexed(6, 0, 0);
		}
		m_shader->UnBindSRV(0, 1, AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
		m_shader->Disable();
		//----------------------------

		//-------------------- Font
		/*this->m_cbWorldfont.Update();
		this->m_shader->SetConstantBuffer(0, 1, m_cbWorldfont.getBuffer());
		this->m_cbColorOptionsFont.Update();
		this->m_shader->SetConstantBuffer(1, 1, m_cbColorOptionsFont.getBuffer());
		
		this->m_shader->BindSRV(0, 1, this->m_fontTexture.Get(), 
			AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
		this->m_shader->SetSampler(0, 1, AngelGUI::GetSamplerState().GetAddressOf());
		this->m_stride = sizeof(VertexType);
		this->m_offset = 0;
		this->m_shader->Apply();
		{
			AngelGUI::GetDeviceContext3D()
				->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &this->m_stride, &this->m_offset);
			AngelGUI::GetDeviceContext3D()
				->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			AngelGUI::GetDeviceContext3D()
				->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			AngelGUI::GetDeviceContext3D()
				->DrawIndexed(6, 0, 0);
		}
		m_shader->UnBindSRV(0, 1, AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
		m_shader->Disable();*/
		////----------------------------
	}
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> AngelGUISystem::GUI3D::AGText::LoadOptionIcon(std::wstring name)
{
	TCHAR Buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, Buffer);
	std::wstring data = Buffer;
	std::wstring toWstr(name.begin(), name.end());
	data += L"\\AngelGUIData\\";
	data += toWstr;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ret;

	HR(DirectX::CreateWICTextureFromFile(
		AngelGUI::GetDevice3D().Get()
		, data.c_str(), nullptr, ret.GetAddressOf()));
	return ret;

}