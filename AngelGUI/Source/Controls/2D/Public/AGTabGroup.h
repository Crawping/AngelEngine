#pragma once
#include<d3d11_1.h>
#include<wincodec.h>
#include<wrl\client.h>
#include"IGUI2D.h"
#include<vector>

namespace AngelGUISystem
{
	namespace GUI2D
	{
		class AGButton;
		class AGTab;
		class AGSprite;
		class AGTabGroup : public IGUI2D
		{
		public:

			AGTabGroup();

			virtual ~AGTabGroup();

			//Draw just a rectangle,if texture path is set then render rectangle use given textrure
			virtual void Initialize() override;

			virtual void Update(float dt)override;

			virtual void Render()override;

			virtual void Resize() override {}

			virtual void Destroy() override;
			virtual void OnKeyPress(UINT32 keyCode) override {};
			virtual void OnKeyCharacter(UINT32 keyCode)override {};
			virtual void OnMousePress(UINT message, float x, float y)override {};
			virtual void OnMouseRelease(UINT message, float x, float y)override;
			virtual void OnMouseMove(float x, float y)override;
			virtual void OnMouseScroll(float xScroll, float yScroll) override {};

			//Added tab rectangle,position,scale will be same size as tab groupe.
			void AddTab(AGTab * m_tab);

		public:
			AGButton * const GetTabHeader() { return m_tabHeader; }
			AGSprite * const GetTabBackground() { return m_tabBackground; }

			void SetTabHeaderText(std::string &txt);

		private:
			virtual void DefaultOnHover() override {};
			virtual void DefaultOnClick() override {};

			void AlignHeaders();
			void AlignTabs();
			void ChangeDragged() { m_dragged = !m_dragged; }
			void TabChanged();
			void FocusOnSelectedTab();

		public:
			virtual void SetActive(bool b) override;
			virtual void SetActive(bool &b) override;
			void SetTabsDefaultColor(float r, float g, float b, float a);
			void SetTabsDefaultColor(float &r, float &g, float &b, float &a);
			void SetTabsHoverColor(float r, float g, float b, float a);
			void SetTabsHoverColor(float &r, float &g, float &b, float &a);
			void SetTabsClickColor(float r, float g, float b, float a);
			void SetTabsClickColor(float &r, float &g, float &b, float &a);
			void SetTabsActiveColor(float r, float g, float b, float a);
			void SetTabsActiveColor(float &r, float &g, float &b, float &a);
			void SetTabHeaderTextColor(float &r, float &g, float &b, float &a);

		private:
			AGButton			 *m_tabHeader;
			AGSprite			 *m_tabBackground;
			std::vector<AGButton *> m_tabGroupeHeaders;
			std::vector<AGTab*>	  m_tabs;
			bool				  m_dragged;
			int					  m_currentTabIndex;
			DirectX::XMFLOAT4	  m_tabActiveColor;

		};
	}
}