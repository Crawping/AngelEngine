#pragma once
#include<d3d11_1.h>
#include<wincodec.h>
#include<wrl\client.h>
#include"IGUI2D.h"
#include<string>

namespace AngelGUISystem
{
	namespace GUI2D
	{
		class AGText;
		class AGSprite;
		class AGButton;
		class AGCheckBox : public IGUI2D
		{
			

		public:

			AGCheckBox();

			virtual ~AGCheckBox();

			//Draw just a rectangle,if texture path is set then render rectangle use given textrure
			virtual void Initialize() override;

			virtual void Update(float dt)override;

			virtual void Render()override;

			virtual void Resize() override {}

			virtual void Destroy() override;
			virtual void OnKeyPress(UINT32 keyCode) override {};
			virtual void OnKeyCharacter(UINT32 keyCode)override {};
			virtual void OnMousePress(UINT message, float x, float y)override {};
			virtual void OnMouseRelease(UINT message, float x, float y)override {};
			virtual void OnMouseMove(float x, float y)override {};
			virtual void OnMouseScroll(float xScroll, float yScroll) override {};

		public:
			void SetText(wchar_t * text);
			void SetText(std::string &in);

			AGText * const GetCheckBoxText()	{ return m_checkBoxText; }
			void SetBackgroundColor(float r, float g, float b, float a);
			void SetBackgroundColor(float &r, float &g, float &b, float &a);
			void SetFilledColor(float r, float g, float b, float a);
			void SetFilledColor(float &r, float &g, float &b, float &a);

		private:
			virtual void DefaultOnHover() override {};
			virtual void DefaultOnClick() override {};

			void ChangeChecked() { m_isChecked = !m_isChecked; }

		public:
			virtual void SetActive(bool b);
			virtual void SetActive(bool &b);

			bool GetChecked() const { return m_isChecked; }

		private:
			AGButton	*m_checkBox;
			AGSprite	*m_checkBoxFill;
			AGText		*m_checkBoxText;
			bool		m_isChecked;
			TSpecificFunctor<AGCheckBox> *m_callBack;
		};
	}
}