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
		class AGSprite;
		class AGTabGroup;
		class AGTab : public IGUI2D
		{
			friend class AGTabGroup;
		public:

			AGTab();

			virtual ~AGTab();

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

			//Added items will realign by the position of the current tab.
			void AddItem(IGUI2D * m_item);

		public:
			AGButton * const GetTabHeader()			{ return m_tabHeader; }
			AGSprite * const GetTabBackground()		{ return m_tabBackground; }

			void SetText(std::string &in);
			void SetBackgroundColor(float r, float g, float b, float a);
			void SetBackgroundColor(float &r, float &g, float &b, float &a);
			void SetTabHeaderTextColor(float &r, float &g, float &b, float &a);


		private:
			virtual void DefaultOnHover() override {};
			virtual void DefaultOnClick() override {};

			void AlignItems();
			//void AlignItem(int);
			void ChangeDragged() { m_dragged = !m_dragged; }

		public:
			virtual void SetActive(bool b) override;
			virtual void SetActive(bool &b) override;

		private:
			AGButton			*m_tabHeader;
			AGSprite			*m_tabBackground;
			std::vector<IGUI2D*> m_items;
			bool				 m_dragged;
			//special member when in tab groupe.
			bool				 m_isInTabGroupe;
			std::vector<DirectX::XMFLOAT2>	 m_cachedPositions;
			
		};
	}
}