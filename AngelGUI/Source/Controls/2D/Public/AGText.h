#pragma once
#include"../../../Core/GlobalTools.h"
#include<vector>
#include"../../../Core/EditableLayout.h"
#include"../../../Core/DrawingEffect.h"
#include"../../../Core/AngelGUI.h"
#include"IGUI2D.h"

#define MAX_TEXT_LENGTH 5000

namespace AngelGUISystem
{
	namespace GUI2D
	{
		class AGSprite;
		class AGText : public IGUI2D
		{
			friend class AGButton;
			enum SetSelectionMode
			{
				SetSelectionModeLeft,               // cluster left
				SetSelectionModeRight,              // cluster right
				SetSelectionModeUp,                 // line up
				SetSelectionModeDown,               // line down
				SetSelectionModeLeftChar,           // single character left (backspace uses it)
				SetSelectionModeRightChar,          // single character right
				SetSelectionModeLeftWord,           // single word left
				SetSelectionModeRightWord,          // single word right
				SetSelectionModeHome,               // front of line
				SetSelectionModeEnd,                // back of line
				SetSelectionModeFirst,              // very first position
				SetSelectionModeLast,               // very last position
				SetSelectionModeAbsoluteLeading,    // explicit position (for mouse click)
				SetSelectionModeAbsoluteTrailing,   // explicit position, trailing edge
				SetSelectionModeAll                 // select all text
			};

		public:
			enum FIT_MODE
			{
				SCALE_DOWN_TO_FIT,
				SCROLL_BAR
			};

			enum ALIGNMENT_DIRECTION
			{
				LEFT_TO_RIGHT,
				RIGHT_TO_LEFT,
			};
		public:

			AGText();
			AGText(const AGText &that);
			virtual ~AGText();

			AGText &operator =(const AGText &that);

		public:

			virtual void Initialize() override;

			virtual void Update(float dt) override;

			virtual void Render() override;

			virtual void Destroy() override;

			virtual void Resize() override {}

		public:

			///Getters
			std::wstring GetText() { return text_; }
			///

			///Setters	
			void SetFontSize(float &_size) { this->m_font->size = _size;ReCreateFont(); }
			void SetFontStyle(DWRITE_FONT_STYLE style) { this->m_font->style = style;ReCreateFont(); }
			void SetFontLocal(const wchar_t * _local) {  this->m_font->local = _local;ReCreateFont();}
			void SetFontFamilyName(const wchar_t * _familyName) {  this->m_font->fontFamilyName = _familyName; }
			void SetBackGroundColor(DirectX::XMFLOAT4 color) { this->m_Backcolor = color;  }
			virtual void SetActive(bool b) override;
			virtual void SetActive(bool &b) override;
			//Add New Line Or Scale Down Font To Fit The Rectangle
			void SetFittingMode(FIT_MODE fit) { m_fitMode = fit; }
			void SetFittingMode(int &fit)			 { m_fitMode = (FIT_MODE)fit; }
			//if use scrollbarr,how much scrollbar move when dragged.
			void SetScrollSpeed(float speed)			 { m_scrollBarSpeed = speed; }
			void SetCarretColor(float r, float g, float b, float a);
			void SetCarretColor(float &r, float &g, float &b, float &a);
			void SetTextColor(float &r, float &g, float &b, float &a);
			void SetBackgroundColor(float &r, float &g, float &b, float &a);
			void SetBackgroundColor(float r, float g, float b, float a);
			void SetTextColor(DirectX::XMFLOAT4 color);
			///

		public:
			// Clear all text and addes new.
			void SetText(const wchar_t _newText[]);
			void SetText(std::string &in);
			void SetEditable(bool b);
			void SetEditable(bool &b);
			bool SetSelectionFromPoint(float x, float y, bool extendSelection);
			void AlignCaretToNearestCluster(bool isTrailingHit = false, bool skipZeroWidth = false);
			void UpdateScrollInfo();
			void ConstrainViewOrigin();
			void SetReadingDirection(ALIGNMENT_DIRECTION dir) { m_alignmentDirection = dir; }
			void SetReadingDirection(int &dir) { m_alignmentDirection = (ALIGNMENT_DIRECTION)dir; }
			void GetLineMetrics(OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics);
			void GetLineFromPosition(
				const DWRITE_LINE_METRICS* lineMetrics,   // [lineCount]
				UINT32 lineCount,
				UINT32 textPosition,
				OUT UINT32* lineOut,
				OUT UINT32* linePositionOut
				);

			float GetAngle() { return angle_; }
			float SetAngle(float angle, bool relativeAdjustement);
			void  GetScale(OUT float* scaleX, OUT  float* scaleY);
			void  GetViewMatrix(OUT DWRITE_MATRIX* matrix);
			void  GetInverseViewMatrix(OUT DWRITE_MATRIX* matrix);
			bool GetEditable() const { return m_editable; }
			void  ResetView();
			void  RefreshView();
			void UpdateCaretFormatting();
			virtual void OnKeyPress(UINT32 keyCode)override;
			void CopyToClipboard();
			void DeleteSelection();
			void PasteFromClipboard();
			HRESULT InsertText(const wchar_t* text);
			virtual void OnKeyCharacter(UINT32 charCode)override;
			bool SetSelection(SetSelectionMode moveMode, UINT32 advance, bool extendSelection, bool updateCaretFormat = true);
			DWRITE_TEXT_RANGE GetSelectionRange();
			UINT32 GetCaretPosition();
			virtual void OnMousePress(UINT message, float x, float y) override;
			virtual void OnMouseRelease(UINT message, float x, float y)override;
			virtual void OnMouseMove(float x, float y)override;
			virtual void OnMouseScroll(float xScroll, float yScroll) override;
			void OnMouseExit();
			void MirrorXCoordinate(IN OUT float& x);
			IDWriteTextLayout* GetLayout() { return textLayout_; }
			EditableLayout::CaretFormat& GetCaretFormat() { return caretFormat_; }
			void ComputeInverseMatrix(
				DWRITE_MATRIX const& matrix,
				OUT DWRITE_MATRIX& result
				)
			{
				// Used for hit-testing, mouse scrolling, panning, and scroll bar sizing.

				float invdet = 1.f / AngelGUISystem::GUIGlobalTools::GetDeterminant(matrix);
				result.m11 = matrix.m22 * invdet;
				result.m12 = -matrix.m12 * invdet;
				result.m21 = -matrix.m21 * invdet;
				result.m22 = matrix.m11 * invdet;
				result.dx = (matrix.m21 * matrix.dy - matrix.dx  * matrix.m22) * invdet;
				result.dy = (matrix.dx  * matrix.m12 - matrix.m11 * matrix.dy)  * invdet;
			}

			D2D1_POINT_2F GetPageSize(IDWriteTextLayout* textLayout)
			{
				// Use the layout metrics to determine how large the page is, taking
				// the maximum of the content size and layout's maximal dimensions.

				DWRITE_TEXT_METRICS textMetrics;
				textLayout->GetMetrics(&textMetrics);

				float width = DirectX::XMMax<float>(textMetrics.layoutWidth, textMetrics.left + textMetrics.width);
				float height = DirectX::XMMax<float>(textMetrics.layoutHeight, textMetrics.height);

				D2D1_POINT_2F pageSize = { width, height };
				return pageSize;
			}

			void GetCaretRect(OUT D2D1_RECT_F& rect)
			{
				// Gets the current caret position (in untransformed space).

				D2D1_RECT_F zeroRect = {};
				rect = zeroRect;

				if (textLayout_ == NULL)
					return;

				// Translate text character offset to point x,y.
				DWRITE_HIT_TEST_METRICS caretMetrics;
				float caretX, caretY;

				textLayout_->HitTestTextPosition(
					caretPosition_,
					caretPositionOffset_ > 0, // trailing if nonzero, else leading edge
					&caretX,
					&caretY,
					&caretMetrics
					);

				// If a selection exists, draw the caret using the
				// line size rather than the font size.
				DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
				if (selectionRange.length > 0)
				{
					UINT32 actualHitTestCount = 1;
					textLayout_->HitTestTextRange(
						caretPosition_,
						0, // length
						0, // x
						0, // y
						&caretMetrics,
						1,
						&actualHitTestCount
						);

					caretY = caretMetrics.top;
				}

				// The default thickness of 1 pixel is almost _too_ thin on modern large monitors,
				// but we'll use it.
				DWORD caretIntThickness = 2;
				SystemParametersInfo(SPI_GETCARETWIDTH, 0, &caretIntThickness, FALSE);
				const float caretThickness = float(caretIntThickness);

				// Return the caret rect, untransformed.
				rect.left = caretX - caretThickness / 2.0f;
				rect.right = rect.left + caretThickness;
				rect.top = caretY;
				rect.bottom = caretY + caretMetrics.height;
			}

			static bool IsLandscapeAngle(float angle)
			{
				// Returns true if the angle is rotated 90 degrees clockwise
				// or anticlockwise (or any multiple of that).
				return fmod(abs(angle) + 45.0f, 180.0f) >= 90.0f;
			}

			void PostRedraw() { InvalidateRect(
				AngelGUI::GetHWND(), NULL, FALSE); }
			void UpdateSystemCaret(const D2D1_RECT_F& rect);
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
			HWND GetHwnd() { return hwnd_; }
			void OnDestroy();
		private:

			struct Font
			{
				DWRITE_FONT_WEIGHT weight;
				DWRITE_FONT_STYLE  style;
				DWRITE_FONT_STRETCH stretch;
				DWRITE_TEXT_ALIGNMENT text_alignment;
				DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment;
				float size;
				const WCHAR* local;
				const WCHAR* fontFamilyName;
			};


		private:
			int GetWcharSize(wchar_t text[]);
			virtual void DefaultOnHover() override {};
			virtual void DefaultOnClick() override {};
			void ScaleDownFont(float amount);
			void SetScrollBarCurrentColor(DirectX::XMFLOAT4 color);
			void ReCreateFont();

		private:

			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_BrushCursor;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_BrushCaretHighlight;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_BrushBackground;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_BrushScrollBar;
			Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>		m_stateBlock;
			Microsoft::WRL::ComPtr<IDWriteTextLayout>			m_textLayout;
			Microsoft::WRL::ComPtr<IDWriteTextFormat>			m_textFormat;
			DirectX::XMFLOAT4									m_Backcolor;
			DirectX::XMFLOAT4									m_textColor;
			Font *												m_font;
			UINT												m_indicesSize;
			UINT												m_stride;
			UINT												m_offset;
			int													m_currentTextSize;
			int													m_caretBlinkCounter;
			int													m_caretBlinkSpeed;
			bool												m_caretBlink;
			DirectX::XMFLOAT4									m_caretColor;
			float												m_scrollBarY;
			float												m_heightOffset;
			HWND hwnd_;
			float																	m_startX;
			float																	m_startY;		
			float scaleX_;          // horizontal scaling
			float scaleY_;          // vertical scaling
			float angle_;           // in degrees
			float originX_;         // focused point in document (moves on panning and caret navigation)
			float originY_;
			float contentWidth_;    // page size - margin left - margin right (can be fixed or variable)
			float contentHeight_;
			EditableLayout              layoutEditor_;
			IDWriteTextLayout*					textLayout_;
			std::wstring text_;
			UINT32 caretAnchor_;
			UINT32 caretPosition_;
			UINT32 caretPositionOffset_;
			EditableLayout::CaretFormat caretFormat_;
			bool currentlySelecting_;
			bool currentlyPanning_;
			float previousMouseX;
			float previousMouseY;
			bool		m_editable;
			FIT_MODE  m_fitMode;
			ALIGNMENT_DIRECTION m_alignmentDirection;
			//if this text is in button.
			bool m_useForButton;
			Microsoft::WRL::ComPtr<ID2D1Layer> m_Layer;
			D2D1_RECT_F		m_scrollBarRectangle;
			bool			m_renderScrollBar;
			bool			m_scrollBarSelected;
			Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry>			m_rectangleGeometrySrollBar;
			Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry>			m_rectangleGeometryBackground;
			D2D1::Matrix3x2F		m_scrollBarTransformationMatrix;
			float m_lastMouseX;
			float m_lastMouseY;
			float m_scrollBarSpeed;
			float m_scrolledTextPosition;
			bool	m_canScroll;
			DirectX::XMFLOAT4	m_scrollBarHoverColor;
			DirectX::XMFLOAT4	m_scrollBarSelectColor;
			DirectX::XMFLOAT4	m_scrollBarDefaultColor;
			DirectX::XMFLOAT4	m_scrollBarCurrentColor;
		};
	}
}
