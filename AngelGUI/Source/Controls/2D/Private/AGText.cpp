#include"../Public/AGText.h"
#include"../Public/AGSprite.h"
#include<tchar.h>
#include<d2d1helper.h>
#include"../../../Core/DrawingEffect.h"


AngelGUISystem::GUI2D::AGText::AGText() : 
	  m_currentTextSize(0), m_heightOffset(0.0f)
	, m_editable(true),
	  m_fitMode(SCROLL_BAR), m_useForButton(false), m_Layer(nullptr)
	, m_renderScrollBar(false), m_scrollBarSelected(false) , m_rectangleGeometrySrollBar(nullptr)
	, m_lastMouseX(0.0f),m_lastMouseY(0.0f), m_scrollBarY(0.0f), m_scrollBarSpeed(0.5f)
	, m_scrolledTextPosition(0.0f) , m_canScroll(true),m_rectangleGeometryBackground(nullptr)
{
	
	m_textColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Backcolor = DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 0.8f);
	m_caretBlinkCounter = 0;
	m_caretBlinkSpeed = 40;
	m_caretBlink = false;
	m_scrollBarTransformationMatrix = D2D1::Matrix3x2F::Identity();
	m_scrollBarHoverColor = DirectX::XMFLOAT4(0.0f, 0.75f, 0.0f, 0.8f);
	m_scrollBarSelectColor = DirectX::XMFLOAT4(0.0f, 0.9f, 0.0f, 0.8f);
	m_scrollBarCurrentColor =  DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 0.8f);
	m_scrollBarDefaultColor = DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 0.8f);

}

AngelGUISystem::GUI2D::AGText::AGText(const AngelGUISystem::GUI2D::AGText& that)
{
	//this->m_Brush = that.m_Brush;
	//
	//this->m_font = that.m_font;
	//this->m_maxTextHeigth = that.m_maxTextHeigth;
	//this->m_maxTextWidth = that.m_maxTextWidth;
	//this->m_next = that.m_next;
	//this->m_prev = that.m_prev;
	//this->m_stateBlock = that.m_stateBlock;
	////this->m_text[0] = that.m_text;
	//this->m_textFormat = that.m_textFormat;
	//this->m_textLayout = that.m_textLayout;
}

AngelGUISystem::GUI2D::AGText::~AGText()
{

}

AngelGUISystem::GUI2D::AGText & AngelGUISystem::GUI2D::AGText::operator=(const AngelGUISystem::GUI2D::AGText& that)
{
	
	//this->m_Brush = that.m_Brush;
	//
	//this->m_font = that.m_font;
	//this->m_maxTextHeigth = that.m_maxTextHeigth;
	//this->m_maxTextWidth = that.m_maxTextWidth;
	//this->m_next = that.m_next;
	//this->m_prev = that.m_prev;

	////this->m_text = that.m_text;
	//this->m_textFormat = that.m_textFormat;
	//this->m_textLayout = that.m_textLayout;
	return *this;
}


void AngelGUISystem::GUI2D::AGText::Initialize()
{
	hwnd_ = AngelGUI::GetHWND();
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

	/*RECT rc;
	GetClientRect(hwnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(
	rc.right - rc.left,
	rc.bottom - rc.top
	);*/

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_IBEAM);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("DirectWriteEdit");
	wcex.hIconSm = NULL;

	RegisterClassEx(&wcex);

	hwnd_ = CreateWindowEx(
		WS_EX_STATICEDGE,
		L"DirectWriteEdit",
		L"",
		WS_CHILDWINDOW | WS_VSCROLL | STATE_SYSTEM_INVISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		AngelGUI::GetHWND(),
		NULL,
		HINST_THISCOMPONENT,
		this
		);
	if (hwnd_ == NULL)
		HRESULT_FROM_WIN32(GetLastError());

	RECT rect = {};
	GetClientRect(hwnd_, &rect);
	D2D1_SIZE_U d2dSize = D2D1::SizeU(rect.right, rect.bottom);

	m_font = new Font();
	m_font->size = 20;


	AngelGUI::GetRenderTarget2D()->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	//DPIScale::Initialize(m_direct2DFactory.Get());

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

	COM_RELEASE(m_brush);
	HR(AngelGUI::GetRenderTarget2D()->CreateSolidColorBrush(D2D1::ColorF(
		m_defaultColor.x,
		m_defaultColor.y,
		m_defaultColor.z,
		m_defaultColor.w),
		&m_brush));

	COM_RELEASE(m_BrushCursor);
	HR(AngelGUI::GetRenderTarget2D()->CreateSolidColorBrush(D2D1::ColorF(
		0.0f,
		0.0f,
		0.0f,
		1.0f), m_BrushCursor.GetAddressOf()));

	COM_RELEASE(m_BrushCaretHighlight);
	HR(AngelGUI::GetRenderTarget2D()->CreateSolidColorBrush(D2D1::ColorF(
		0.52f,
		0.80f,
		0.98f,
		1.0f), m_BrushCaretHighlight.GetAddressOf()));

	COM_RELEASE(m_BrushBackground);
	HR(AngelGUI::GetRenderTarget2D()->CreateSolidColorBrush(D2D1::ColorF(
		0.92f,
		0.90f,
		0.98f,
		1.0f), m_BrushBackground.GetAddressOf()));

	COM_RELEASE(m_BrushScrollBar);
	HR(AngelGUI::GetRenderTarget2D()->CreateSolidColorBrush(D2D1::ColorF(
		m_scrollBarDefaultColor.x,
		m_scrollBarDefaultColor.y,
		m_scrollBarDefaultColor.z,
		m_scrollBarDefaultColor.w), m_BrushScrollBar.GetAddressOf()));


	// Set the initial text.

	text_.assign(L"");
	
	layoutEditor_ = AngelGUI::GetDirectWriteFactory().Get();
	// Create an ideal layout for the text editor based on the text and format,
	// favoring document layout over pixel alignment.
	
	HR(AngelGUI::GetDirectWriteFactory()->CreateTextLayout(
		text_.c_str(),
		static_cast<UINT32>(text_.size()),
		m_textFormat.Get(),
		m_rectangle.right,            // maximum width
		m_rectangle.bottom,            // maximum height
		&textLayout_
		));

	// Get size of text layout; needed for setting the view origin.
	float layoutWidth = textLayout_->GetMaxWidth();
	float layoutHeight = textLayout_->GetMaxHeight();
	originX_ = 0;
	originY_ = 0;

	// Set the initial text layout and update caret properties accordingly.
	UpdateCaretFormatting();
	AngelGUI::AddGUI2DItem(this);
}

LRESULT CALLBACK AngelGUISystem::GUI2D::AGText::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Relays messages for the text editor to the internal class.

	AngelGUISystem::GUI2D::AGText* window = reinterpret_cast<AngelGUISystem::GUI2D::AGText*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch (message)
	{
	case WM_NCCREATE:
	{
		// Associate the data structure with this window handle.
		CREATESTRUCT* pcs = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<AngelGUISystem::GUI2D::AGText*>(pcs->lpCreateParams);
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
void AngelGUISystem::GUI2D::AGText::Render()
{
	if (m_recreate)
	{
	
		HR(AngelGUI::GetDirectWriteFactory()->CreateTextLayout(
			text_.c_str(),
			static_cast<UINT32>(text_.size()),
			m_textFormat.Get(),
			m_rectangle.right,            // maximum width
			m_rectangle.bottom,            // maximum height
			&textLayout_
			));

		COM_RELEASE(m_rectangleGeometryBackground);
		AngelGUI::GetFactory()->CreateRoundedRectangleGeometry(
			D2D1::RoundedRect(m_rectangle,5,5), m_rectangleGeometryBackground.GetAddressOf());

	}

	if (m_scrollBarSelected)
		SetScrollBarCurrentColor(m_scrollBarSelectColor);
	
	DWRITE_TEXT_METRICS met;
	textLayout_->GetMetrics(&met);
	if (m_fitMode == SCALE_DOWN_TO_FIT && !m_useForButton)
	{
		while (met.height > ((m_rectangle.bottom*m_scale.y) - (m_rectangle.top*m_scale.y))
			/*|| met.width + 5 > (((m_rectangle.right)*m_scale.x) - ((m_rectangle.left)*m_scale.x))*/)

		{
			textLayout_->GetMetrics(&met);
			ScaleDownFont(0.05f);
		}
	}

	if (m_useForButton)
	{
		while (met.height > ((m_rectangle.bottom*m_scale.y) - (m_rectangle.top*m_scale.y))
			|| met.width + 5 > (((m_rectangle.right)*m_scale.x) - ((m_rectangle.left)*m_scale.x)))

		{
			textLayout_->GetMetrics(&met);
			ScaleDownFont(0.05f);
		}
	}

	if (m_fitMode == FIT_MODE::SCROLL_BAR)
	{
		if (met.height > ((m_rectangle.bottom*m_scale.y) - (m_rectangle.top*m_scale.y)))
			m_renderScrollBar = true;
		else
			m_renderScrollBar = false;
	}

	m_transformMatrix =
		D2D1::Matrix3x2F::Translation(m_position.x, m_position.y);

	if (m_useForButton)
	{
		//use for centering inside a button.
		m_transformMatrix =
			D2D1::Matrix3x2F::Translation(m_position.x-(met.width/2), m_position.y - (met.height / 2));
	}

	D2D1::Matrix3x2F pageTransform;
	GetViewMatrix(&AngelGUISystem::GUIGlobalTools::Cast(pageTransform));

	if (m_alignmentDirection == RIGHT_TO_LEFT)
		textLayout_->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
	else if (m_alignmentDirection == LEFT_TO_RIGHT)
		textLayout_->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);

	AngelGUI::GetRenderTarget2D()->SetTransform((pageTransform));

	// Draw the page
	D2D1_POINT_2F pageSize = GetPageSize(textLayout_);
	D2D1_RECT_F pageRect = { 0, 0,pageSize.x, pageSize.y };
	if(m_editable)
		AngelGUI::GetRenderTarget2D()->FillGeometry(m_rectangleGeometryBackground.Get(), 
			m_BrushBackground.Get());

	if (m_renderScrollBar && m_editable)
	{
		float precentShown = (m_rectangle.bottom / met.height);
		m_scrollBarRectangle.left = 0;
		m_scrollBarRectangle.right = 10;
		m_scrollBarRectangle.top = 0;
		m_scrollBarRectangle.bottom = m_rectangle.bottom - (m_rectangle.bottom*(1 - precentShown));
		DWRITE_MATRIX cacheTransform;
		
		if (m_scrollBarY < 0.0f)
			m_scrollBarY = 0.0f;
		if (m_scrollBarY + m_scrollBarRectangle.bottom > m_rectangle.bottom)
		{
			m_scrollBarY = m_rectangle.bottom - m_scrollBarRectangle.bottom;
		}
		{
			m_scrollBarTransformationMatrix =
				D2D1::Matrix3x2F::Translation(m_position.x + m_rectangle.right, m_position.y +
					m_scrollBarY);
		}

		m_scrolledTextPosition = m_position.y + m_scrollBarY;
		float unitInBar = met.height / m_rectangle.bottom;
		float unitInText = 1 / unitInBar;
		m_scrolledTextPosition = m_scrollBarY*unitInBar;
		
		AngelGUI::GetRenderTarget2D()->GetTransform(&AngelGUISystem::GUIGlobalTools::Cast(cacheTransform));

		AngelGUI::GetRenderTarget2D()->SetTransform((m_scrollBarTransformationMatrix));
		
		COM_RELEASE(m_rectangleGeometrySrollBar);

		AngelGUI::GetFactory()->CreateRoundedRectangleGeometry
			(D2D1::RoundedRect(m_scrollBarRectangle,2,2), m_rectangleGeometrySrollBar.GetAddressOf());
		AngelGUISystem::AngelGUI::GetRenderTarget2D()->
			FillGeometry(m_rectangleGeometrySrollBar.Get(), m_BrushScrollBar.Get());
		AngelGUI::GetRenderTarget2D()->SetTransform(&AngelGUISystem::GUIGlobalTools::Cast(cacheTransform));
	}
	else
		m_rectangleGeometrySrollBar = nullptr;
		
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

	if (caretRange.length > 0 && m_editable && !m_scrollBarSelected && m_selected )
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

	if (m_editable)
		AngelGUI::GetRenderTarget2D()->PushLayer(
			D2D1::LayerParameters(m_rectangle),
			m_Layer.Get()
			);
	// Draw the selection ranges behind the text.
	if (actualHitTestCount > 0 && m_editable && !m_scrollBarSelected && m_selected)
	{
		// Note that an ideal layout will return fractional values,
		// so you may see slivers between the selection ranges due
		// to the per-primitive antialiasing of the edges unless
		// it is disabled (better for performance anyway).
		AngelGUI::GetRenderTarget2D()->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
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
			if (m_renderScrollBar)
			{
				m_transformMatrix =
					D2D1::Matrix3x2F::Translation(m_position.x, m_position.y - m_scrolledTextPosition);
				AngelGUI::GetRenderTarget2D()->SetTransform(m_transformMatrix);

				AngelGUI::GetRenderTarget2D()->FillRectangle(highlightRect, m_BrushCaretHighlight.Get());
			}
			else
			{
				AngelGUI::GetRenderTarget2D()->FillRectangle(highlightRect, m_BrushCaretHighlight.Get());
			}
			
		}

		AngelGUI::GetRenderTarget2D()->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}

	// Draw our caret onto the render target.
	D2D1_RECT_F caretRect;
	GetCaretRect(caretRect);
	AngelGUI::GetRenderTarget2D()->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	
	if (m_editable)
	{
		if (m_renderScrollBar)
		{
			m_transformMatrix =
				D2D1::Matrix3x2F::Translation(m_position.x, m_position.y - m_scrolledTextPosition);
			AngelGUI::GetRenderTarget2D()->SetTransform(m_transformMatrix);
			if(m_selected)
				AngelGUI::GetRenderTarget2D()->FillRectangle(caretRect, m_BrushCursor.Get());
		}
		else
		{
			if (m_selected)
				AngelGUI::GetRenderTarget2D()->FillRectangle(caretRect, m_BrushCursor.Get());
		}
	}
		
	AngelGUI::GetRenderTarget2D()->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// Draw text
	if (m_fitMode == FIT_MODE::SCROLL_BAR && !m_useForButton)
	{
		DWRITE_MATRIX cacheTransform;
		if(m_renderScrollBar)
			m_transformMatrix =
				D2D1::Matrix3x2F::Translation(m_position.x,m_position.y- m_scrolledTextPosition);
		if (!m_renderScrollBar)
			m_transformMatrix =
			D2D1::Matrix3x2F::Translation(m_position.x, m_position.y);
		AngelGUI::GetRenderTarget2D()->GetTransform(&AngelGUISystem::GUIGlobalTools::Cast(cacheTransform));

		AngelGUI::GetRenderTarget2D()->SetTransform((m_transformMatrix));
		AngelGUI::GetRenderTarget2D()->DrawTextLayout(
			D2D1::Point2F(0.f, .0f),
			textLayout_, m_brush.Get());
		
		AngelGUI::GetRenderTarget2D()->SetTransform(&AngelGUISystem::GUIGlobalTools::Cast(cacheTransform));
	}
	else
	{
		AngelGUI::GetRenderTarget2D()->DrawTextLayout(
			D2D1::Point2F(0.f, .0f),
			textLayout_, m_brush.Get());
	}

	// Draw the selection ranges in front of images.
	// This shades otherwise opaque images so they are visibly selected,
	// checking the isText field of the hit-test metrics.
	if (actualHitTestCount > 0 && m_editable && !m_scrollBarSelected && m_selected)
	{
		// Note that an ideal layout will return fractional values,
		// so you may see slivers between the selection ranges due
		// to the per-primitive antialiasing of the edges unless
		// it is disabled (better for performance anyway).
		AngelGUI::GetRenderTarget2D()->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

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

			if (m_renderScrollBar)
			{
				m_transformMatrix =
					D2D1::Matrix3x2F::Translation(m_position.x, m_position.y - m_scrolledTextPosition);
				AngelGUI::GetRenderTarget2D()->SetTransform(m_transformMatrix);
				AngelGUI::GetRenderTarget2D()->FillRectangle(highlightRect, m_BrushCaretHighlight.Get());
			}
			else
			{
				AngelGUI::GetRenderTarget2D()->FillRectangle(highlightRect, m_BrushCaretHighlight.Get());
			}
			
		}

		AngelGUI::GetRenderTarget2D()->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_ALIASED);
	}
	if(m_editable)
		AngelGUI::GetRenderTarget2D()->PopLayer();
}

void AngelGUISystem::GUI2D::AGText::GetViewMatrix(OUT DWRITE_MATRIX* matrix)
{
	
	// Generates a view matrix from the current origin, angle, and scale.

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

	resultB.SetProduct(AngelGUISystem::GUIGlobalTools::Cast(translationMatrix), AngelGUISystem::GUIGlobalTools::Cast(rotationMatrix));
	resultA.SetProduct(resultB, AngelGUISystem::GUIGlobalTools::Cast(centerMatrix));

	// For better pixel alignment (less blurry text)
	resultA._31 = floor(resultA._31);
	resultA._32 = floor(resultA._32);

	*matrix = *reinterpret_cast<DWRITE_MATRIX*>(&resultA);
	*matrix = AngelGUISystem::GUIGlobalTools::Cast(m_transformMatrix);
}


void AngelGUISystem::GUI2D::AGText::GetInverseViewMatrix(OUT DWRITE_MATRIX* matrix)
{
	// Inverts the view matrix for hit-testing and scrolling.

	DWRITE_MATRIX viewMatrix;
	GetViewMatrix(&viewMatrix);
	ComputeInverseMatrix(viewMatrix, *matrix);
}

DWRITE_TEXT_RANGE AngelGUISystem::GUI2D::AGText::GetSelectionRange()
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

UINT32 AngelGUISystem::GUI2D::AGText::GetCaretPosition()
{
	return caretPosition_ + caretPositionOffset_;
}

void AngelGUISystem::GUI2D::AGText::AlignCaretToNearestCluster(bool isTrailingHit, bool skipZeroWidth)
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

void AngelGUISystem::GUI2D::AGText::UpdateSystemCaret(const D2D1_RECT_F& rect)
{
	// Moves the system caret to a new position.

	// Although we don't actually use the system caret (drawing our own
	// instead), this is important for accessibility, so the magnifier
	// can follow text we type. The reason we draw our own directly
	// is because intermixing DirectX and GDI content (the caret) reduces
	// performance.

	// Gets the current caret position (in untransformed space).

	if (GetFocus() != hwnd_) // Only update if we have focus.
		return;

	D2D1::Matrix3x2F pageTransform;
	GetViewMatrix(&AngelGUISystem::GUIGlobalTools::Cast(pageTransform));

	// Transform caret top/left and size according to current scale and origin.
	D2D1_POINT_2F caretPoint = pageTransform.TransformPoint(D2D1::Point2F(rect.left, rect.top));

	float width = (rect.right - rect.left);
	float height = (rect.bottom - rect.top);
	float transformedWidth = width * pageTransform._11 + height * pageTransform._21;
	float transformedHeight = width * pageTransform._12 + height * pageTransform._22;

	// Update the caret's location, rounding to nearest integer so that
	// it lines up with the text selection.

	int intX = AngelGUISystem::GUIGlobalTools::RoundToInt(caretPoint.x);
	int intY = AngelGUISystem::GUIGlobalTools::RoundToInt(caretPoint.y);
	int intWidth = AngelGUISystem::GUIGlobalTools::RoundToInt(transformedWidth);
	int intHeight = AngelGUISystem::GUIGlobalTools::RoundToInt(caretPoint.y + transformedHeight) - intY;

	CreateCaret(hwnd_, NULL, intWidth, intHeight);
	SetCaretPos(intX, intY);

	// Don't actually call ShowCaret. It's enough to just set its position.
}

void AngelGUISystem::GUI2D::AGText::GetLineFromPosition(
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

void AngelGUISystem::GUI2D::AGText::DeleteSelection()
{
	// Deletes selection.

	DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
	if (selectionRange.length <= 0)
		return;

	layoutEditor_.RemoveTextAt(textLayout_, text_, selectionRange.startPosition, selectionRange.length);

	SetSelection(SetSelectionModeAbsoluteLeading, selectionRange.startPosition, false);
	RefreshView();
}

void AngelGUISystem::GUI2D::AGText::OnMousePress(UINT message, float x, float y)
{
	MirrorXCoordinate(x);

	if (!m_editable)
		return;

	if (message == WM_LBUTTONDOWN)
	{
		// Start dragging selection.
		currentlySelecting_ = true;

		bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
		SetSelectionFromPoint(x, y, heldShift);
		BOOL *contains = new BOOL();
		*contains = false;
		const D2D1::Matrix3x2F * mat = &m_scrollBarTransformationMatrix;
		if (m_rectangleGeometrySrollBar)
		{
			m_rectangleGeometrySrollBar->FillContainsPoint(
					D2D1::Point2F(x, y), mat, contains);
			if (*contains == 1)
				m_scrollBarSelected = true;
			else
				m_scrollBarSelected = false;
		}
		mat = &D2D1::Matrix3x2F::Translation(m_position.x, m_position.y);
		if (m_rectangleGeometryBackground)
		{
			m_rectangleGeometryBackground->FillContainsPoint(
				D2D1::Point2F(x, y), mat, contains);
			if (*contains == 1)
				m_selected = true;
			else
				m_selected = false;
		}
		
	}
	else if (message == WM_RBUTTONDOWN)
	{
		
	}
	else if (message == WM_MBUTTONDOWN)
	{
		previousMouseX = x;
		previousMouseY = y;
		currentlyPanning_ = true;
	}
}

void AngelGUISystem::GUI2D::AGText::OnMouseRelease(UINT message, float x, float y)
{
	MirrorXCoordinate(x);
	m_scrollBarSelected = false;
	if (message == WM_LBUTTONUP)
	{
		currentlySelecting_ = false;
	}
	else if (message == WM_MBUTTONUP)
	{
		currentlyPanning_ = false;
	}
}


void AngelGUISystem::GUI2D::AGText::OnMouseMove(float x, float y)
{
	// Selects text or pans.
	if (!m_editable)
		return;
	if (m_rectangleGeometrySrollBar)
	{
		BOOL *contains = new BOOL();
		*contains = false;
		const D2D1::Matrix3x2F * mat = &m_scrollBarTransformationMatrix;
		if (m_rectangleGeometrySrollBar)
		{
			m_rectangleGeometrySrollBar->FillContainsPoint(
				D2D1::Point2F(x, y), mat, contains);
			if (*contains == 1)
				SetScrollBarCurrentColor(m_scrollBarHoverColor);
			else
				SetScrollBarCurrentColor(m_scrollBarDefaultColor);
		}
	}
	
	if (m_scrollBarSelected)
	{
		if (m_lastMouseY != y)
		{
			if (m_lastMouseY - y < 0.0f)
			{
				if(m_canScroll)
					m_scrollBarY += m_scrollBarSpeed;
			}
			else if (m_lastMouseY - y > 0.0f)
			{
				if (m_canScroll)
					m_scrollBarY -= m_scrollBarSpeed;
			}
		}
	}
	

	MirrorXCoordinate(x);

	if (currentlySelecting_)
	{
		// Drag current selection.
		SetSelectionFromPoint(x, y, true);
	}
	else if (currentlyPanning_)
	{
		DWRITE_MATRIX matrix;
		GetInverseViewMatrix(&matrix);

		float xDif = x - previousMouseX;
		float yDif = y - previousMouseY;
		previousMouseX = x;
		previousMouseY = y;

		originX_ -= (xDif * matrix.m11 + yDif * matrix.m21);
		originY_ -= (xDif * matrix.m12 + yDif * matrix.m22);
		ConstrainViewOrigin();

		RefreshView();
	}
	m_lastMouseX = x;
	m_lastMouseY = y;
}


void AngelGUISystem::GUI2D::AGText::OnMouseScroll(float xScroll, float yScroll)
{
	// Pans or scales the editor.
	if (!m_editable)
		return;
	{
		if (yScroll > 0.0f)
			m_scrollBarY -= m_scrollBarSpeed;
		if (yScroll < 0.0f)
			m_scrollBarY += m_scrollBarSpeed;
	}
}

void AngelGUISystem::GUI2D::AGText::ConstrainViewOrigin()
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

void AngelGUISystem::GUI2D::AGText::OnMouseExit()
{
	currentlySelecting_ = false;
	currentlyPanning_ = false;
}

bool AngelGUISystem::GUI2D::AGText::SetSelectionFromPoint(float x, float y, bool extendSelection)
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

void AngelGUISystem::GUI2D::AGText::MirrorXCoordinate(IN OUT float& x)
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

void AngelGUISystem::GUI2D::AGText::OnKeyPress(UINT32 keyCode)
{
	if (!m_editable)
		return;
	if (!m_selected)
		return;
	// Handles caret navigation and special presses that
	// do not generate characters.

	bool heldShift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
	bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

	UINT32 absolutePosition = caretPosition_ + caretPositionOffset_;

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
				if ((AngelGUISystem::GUIGlobalTools::IsLowSurrogate(charBackOne) &&
					AngelGUISystem::GUIGlobalTools::IsHighSurrogate(charBackTwo))
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


void AngelGUISystem::GUI2D::AGText::OnKeyCharacter(UINT32 charCode)
{
	if (!m_editable)
		return;
	if (!m_selected)
		return;
	// Inserts text characters.

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

void AngelGUISystem::GUI2D::AGText::CopyToClipboard()
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

void AngelGUISystem::GUI2D::AGText::UpdateScrollInfo()
{
	// Updates scroll bars.

	if (textLayout_ == NULL)
		return;

	// Determine scroll bar's step size in pixels by multiplying client rect by current view.
	RECT clientRect;
	GetClientRect(hwnd_, &clientRect);

	D2D1::Matrix3x2F pageTransform;
	GetInverseViewMatrix(&AngelGUISystem::GUIGlobalTools::Cast(pageTransform));

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

void AngelGUISystem::GUI2D::AGText::OnDestroy()
{

}


void AngelGUISystem::GUI2D::AGText::PasteFromClipboard()
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

void AngelGUISystem::GUI2D::AGText::RefreshView()
{
	// Redraws the text and scrollbars.

	UpdateScrollInfo();
	PostRedraw();
}


void AngelGUISystem::GUI2D::AGText::GetLineMetrics(
	OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics
	)
{
	// Retrieves the line metrics, used for caret navigation, up/down and home/end.

	DWRITE_TEXT_METRICS textMetrics;
	textLayout_->GetMetrics(&textMetrics);

	lineMetrics.resize(textMetrics.lineCount);
	textLayout_->GetLineMetrics(&lineMetrics.front(), textMetrics.lineCount, &textMetrics.lineCount);
}


void AngelGUISystem::GUI2D::AGText::UpdateCaretFormatting()
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

	AngelGUISystem::GUIGlobalTools::SafeRelease(&drawingEffect);
}

bool AngelGUISystem::GUI2D::AGText::SetSelection(SetSelectionMode moveMode, UINT32 advance, bool extendSelection, bool updateCaretFormat)
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

void AngelGUISystem::GUI2D::AGText::Update(float dt)
{
	
}

void AngelGUISystem::GUI2D::AGText::SetText(const wchar_t _newText[])
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

int AngelGUISystem::GUI2D::AGText::GetWcharSize(wchar_t text[])
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

void AngelGUISystem::GUI2D::AGText::ScaleDownFont(float amount)
{
	m_font->size -= amount;
	ReCreateFont();
}

void AngelGUISystem::GUI2D::AGText::ReCreateFont()
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

void AngelGUISystem::GUI2D::AGText::Destroy()
{
	COM_RELEASE(m_brush);
	COM_RELEASE(m_BrushCaretHighlight);
	COM_RELEASE(m_BrushCursor);
	COM_RELEASE(m_stateBlock);
	COM_RELEASE(m_textLayout);
	COM_RELEASE(m_textFormat);
	COM_RELEASE(m_BrushBackground);
	COM_RELEASE(m_Layer);
	COM_RELEASE(m_rectangleGeometrySrollBar);
	COM_RELEASE(m_BrushScrollBar);
	COM_RELEASE(m_rectangleGeometryBackground);
	if (textLayout_)
	{
		textLayout_->Release();
		textLayout_ = nullptr;
	}
	if (m_font)
	{
		delete m_font;
		m_font = nullptr;
	}
}

void AngelGUISystem::GUI2D::AGText::SetScrollBarCurrentColor(DirectX::XMFLOAT4 color)
{
	m_scrollBarCurrentColor = color;
	m_BrushScrollBar->SetColor(D2D1::ColorF(m_scrollBarCurrentColor.x,
		m_scrollBarCurrentColor.y,
		m_scrollBarCurrentColor.z,
		m_scrollBarCurrentColor.w));
}

void AngelGUISystem::GUI2D::AGText::SetCarretColor(float r, float g, float ba, float a)
{
	m_BrushCursor->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI2D::AGText::SetCarretColor(float &r, float &g, float &ba, float &a)
{
	m_BrushCursor->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI2D::AGText::SetBackgroundColor(float &r, float &g, float &ba, float &a)
{
	m_BrushBackground->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI2D::AGText::SetBackgroundColor(float r, float g, float ba, float a)
{
	m_BrushBackground->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI2D::AGText::SetTextColor(float &r, float  &g, float  &ba, float  &a)
{
	m_defaultColor.x = r;
	m_defaultColor.y = g;
	m_defaultColor.z = ba;
	m_defaultColor.w = a;
	m_brush->SetColor(D2D1::ColorF(r, g, ba, a));
}

void AngelGUISystem::GUI2D::AGText::SetTextColor(DirectX::XMFLOAT4 color)
{
	m_defaultColor = color;
	m_brush->SetColor(D2D1::ColorF(color.x, color.y, color.z, color.w));
}
void AngelGUISystem::GUI2D::AGText::SetText(std::string & txt)
{
	wchar_t * wtxt = new wchar_t[txt.size() + 1];
	for (unsigned int i = 0;i < txt.size();i++)
		wtxt[i] = txt[i];
	wtxt[txt.size()] = L'\0';
	SetText(wtxt);
}

void AngelGUISystem::GUI2D::AGText::SetActive(bool b)
{
	if (b == true)
		m_active = true;
	else
		m_active = false;
}

void AngelGUISystem::GUI2D::AGText::SetActive(bool &b)
{
	if (b == true)
		m_active = true;
	else
		m_active = false;
}

void AngelGUISystem::GUI2D::AGText::SetEditable(bool b)
{
	if (b == true)
	{
		m_editable = true;
	}
	else
		m_editable = false;
}

void AngelGUISystem::GUI2D::AGText::SetEditable(bool &b)
{
	if (b == true)
	{
		m_editable = true;
	}
	else
		m_editable = false;
}
