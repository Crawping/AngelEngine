#pragma once
#include"IGUI3D.h"
#include<angelscript.h>

namespace AngelGUISystem
{
	namespace GUI3D
	{
		class AGSprite;
		class AGText;
		class AGButton : public IGUI3D
		{
		public:
			AGButton();

			virtual ~AGButton();

			//Draw just a rectangle,if texture path is set then render rectangle use given textrure
			virtual void Initialize() override;

			virtual void Update(float dt,
				DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)override;

			virtual void Render()override;

			virtual void Destroy() override;
			virtual void OnKeyPress(UINT32 keyCode) override {};
			virtual void OnKeyCharacter(UINT32 keyCode)override {};
			virtual void OnMousePress(UINT message, float x, float y)override;
			virtual void OnMouseRelease(UINT message, float x, float y)override;
			virtual void OnMouseMove(float x, float y)override;
			virtual void OnMouseScroll(float xScroll, float yScroll) override {};

		public:
			void SetText(const wchar_t * text);
			void SetText(std::string &text);

			void SetTextColor(float r, float g, float b, float a);
			void SetTextColor(float &r, float &g, float &b, float &a);
			void SetTextColor(DirectX::XMFLOAT4 color);
			void SetTextFontSize(float &size);
			void SetTextFontSize(float size);

			void SetBitmapPath(const wchar_t * p);
			void SetBitmapPath(std::string & p);

		private:
			virtual void DefaultOnHover() override;
			virtual void DefaultOnClick() override;

		public:
			void RegisterCallBackFunction(TFunctor* callback);
			virtual void SetActive(bool b) override;
			virtual void SetActive(bool &b) override;

			

			bool IsClicked() const { return m_selected; }
			std::wstring GetText() const;

			//For angelscript use only
			void RegisterCallBackFunctionScript(asIScriptFunction * callback);
		private:
			AGText * m_text;
			std::vector<TFunctor*>		 m_callBackFunctions;
			//For angelscript use only
			std::vector<asIScriptFunction*>		 m_callBackFunctionsScript;
			//using bitmap image.
			bool m_renderUsingBitmap;
			//texture used for bitmap SRV.
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bitmapSRV;
		};

	}
}
