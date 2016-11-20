#pragma once
#include<d3d11_1.h>
#include<wincodec.h>
#include<wrl\client.h>
#include"IGUI2D.h"

namespace AngelGUISystem
{
	namespace GUI2D
	{
		class AGText;
		class AGSprite : public IGUI2D
		{
			friend class AGButton;
			friend class AGText;
			friend class AGCheckBox;
			friend class AGTab;
		public:

			AGSprite();

			virtual ~AGSprite();

			//Draw just a rectangle,if texture path is set then render rectangle use given textrure
			virtual void Initialize() override;

			virtual void Update(float dt)override;

			virtual void Render() override;

			virtual void Resize() override;

			virtual void Destroy() override;
			virtual void OnKeyPress(UINT32 keyCode) override {};
			virtual void OnKeyCharacter(UINT32 keyCode)override {};
			virtual void OnMousePress(UINT message, float x, float y)override {};
			virtual void OnMouseRelease(UINT message, float x, float y)override {};
			virtual void OnMouseMove(float x, float y)override {};
			virtual void OnMouseScroll(float xScroll, float yScroll) override {};

		private:
			virtual void DefaultOnHover() override {} ;
			virtual void DefaultOnClick() override {};

		public:
			void SetBitmapPath(const wchar_t * p)	{ this->m_bitmapPath = p; LoadBitmapImage(m_bitmapPath); }
			//Set button corners rounded.
			void SetRounded(bool b)					{ this->m_rounded = b; m_recreate = true; }
			void SetRoundedRadius(float x, float y) { this->m_radiusX = x;m_radiusY = y; m_recreate = true; }
			virtual void SetActive(bool b) override;
			virtual void SetActive(bool &b) override;
			void SetFromResource(ID3D11Texture2D * resource);

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
			Microsoft::WRL::ComPtr<IDXGISurface>					m_dxgiData;
			Microsoft::WRL::ComPtr<ID2D1Bitmap>						m_bitmapData;
		};
	}
}

