#pragma once
#include"IGUI3D.h"

namespace AngelGUISystem
{
	namespace GUI3D
	{
		class AGButton;
		class AGText;
		class AGSprite : public IGUI3D
		{
			friend class AGButton;
			friend class AGText;
		public:
			AGSprite();

			virtual ~AGSprite();

			//Draw just a rectangle,if texture path is set then render rectangle use given textrure
			virtual void Initialize() override;

			virtual void Update(float dt, 
				DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)override;

			virtual void Render()override;

			virtual void Destroy() override;
			virtual void OnKeyPress(UINT32 keyCode) override {};
			virtual void OnKeyCharacter(UINT32 keyCode)override {};
			virtual void OnMousePress(UINT message, float x, float y)override {};
			virtual void OnMouseRelease(UINT message, float x, float y)override {};
			virtual void OnMouseMove(float x, float y)override {};
			virtual void OnMouseScroll(float xScroll, float yScroll) override {};

		private:
			virtual void DefaultOnHover() override {};
			virtual void DefaultOnClick() override {};

		public:
			void SetBitmapPath(const wchar_t * p) { this->m_bitmapPath = p; LoadBitmapImage(m_bitmapPath); }
			//Set button corners rounded.
			void SetRounded(bool b) { this->m_rounded = b; m_recreate = true; }
			void SetRoundedRadius(float x, float y) { this->m_radiusX = x;m_radiusY = y; m_recreate = true; }
			virtual void SetActive(bool b) override;
			virtual void SetActive(bool &b) override;
			void SetResource(ID3D11ShaderResourceView * srv) { this->m_resourceSRV = srv; m_renderFromResource = true; }

		private:
			void LoadBitmapImage(const wchar_t * name);
		
		private:
			Microsoft::WRL::ComPtr<ID2D1Bitmap>						m_bitmapImage;
			const wchar_t *											m_bitmapPath;
			bool													m_useBitmap;
			Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry>	m_rectangleRoundedGeometry;
			Microsoft::WRL::ComPtr<ID2D1RectangleGeometry>			m_rectangleGeometry;
			bool													m_rounded;
			float													m_radiusX;
			float													m_radiusY;
		};
		
	}
}