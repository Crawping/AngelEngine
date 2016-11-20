#pragma once
#include<d3d11_1.h>
#include<wincodec.h>
#include<wrl\client.h>
#include"IGUI2D.h"
#include<vector>
#include<angelscript.h>

namespace AngelGUISystem
{
	namespace GUI2D
	{
		class AGSprite;
		class AGText;
		class AGButton : public IGUI2D
		{
		public:

			AGButton();

			virtual ~AGButton();

			//if you want to draw with image then set bitmap path.
			virtual void Initialize()override;

			virtual void Update(float dt)override;

			virtual void Render()override;

			virtual void Destroy() override;

			virtual void Resize() override {}

			virtual void OnKeyPress(UINT32 keyCode) override {};
			virtual void OnKeyCharacter(UINT32 keyCode)override {};
			virtual void OnMousePress(UINT message, float x, float y)override;
			virtual void OnMouseRelease(UINT message, float x, float y)override;
			virtual void OnMouseMove(float x, float y)override;
			virtual void OnMouseScroll(float xScroll, float yScroll) override {};

		private:

			virtual void DefaultOnHover() override;
			virtual void DefaultOnClick() override;

		public:

			void SetBitmapPath(const wchar_t * p);
			void SetBitmapPath(std::string & p);
			void SetText(const wchar_t * text);
			void SetText(std::string &txt);
			void SetTextColor(float r, float g, float b, float a);
			void SetTextColor(float &r, float &g, float &b, float &a);
			void SetTextColor(DirectX::XMFLOAT4 color);
			void SetTextFontSize(float &size);
			void SetTextFontSize(float size);
			//Set button corners rounded.
			void SetRounded(bool b);
			void SetRounded(bool &b);
			void SetRoundedRadius(float &x, float &y);
			void SetRoundedRadius(float x, float y);
			void RegisterCallBackFunction(TFunctor* callback);
			virtual void SetActive(bool b) override;
			virtual void SetActive(bool &b) override;

			bool IsClicked() const			{ return m_selected; }
			std::wstring GetText() const;

			//For angelscript use only
			void RegisterCallBackFunctionScript(asIScriptFunction * callback);

		private:
			AGSprite							*m_sprite;
			AGText								*m_text;
			bool								 m_rounded;
			float								 m_radiusX;
			float								 m_radiusY;
			std::vector<TFunctor*>		 m_callBackFunctions;
			//For angelscript use only
			std::vector<asIScriptFunction*>		 m_callBackFunctionsScript;

		};
	}
}