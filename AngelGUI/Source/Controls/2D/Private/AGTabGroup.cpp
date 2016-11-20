#include"../Public/AGTabGroup.h"
#include"../Public/AGSprite.h"
#include"../Public/AGButton.h"
#include"../Public/AGTabGroup.h"
#include"../Public/AGTab.h"
#include"../../../Core/AngelGUI.h"

AngelGUISystem::GUI2D::AGTabGroup::AGTabGroup() : IGUI2D(),
m_dragged(false) , m_currentTabIndex(0)
{

}

void AngelGUISystem::GUI2D::AGTabGroup::Initialize()
{
	//m_tabBackground = new AGSprite();
	//m_tabBackground->Initialize();
	//m_tabBackground->SetRectangle(m_rectangle);
	//m_tabBackground->SetPosition(m_position);

	m_tabHeader = new AGButton();
	m_tabHeader->Initialize();
	m_tabHeader->SetRectangle(0, 0, m_rectangle.right, 20);
	m_tabHeader->SetPosition(m_position.x, m_position.y - 40);
	static TSpecificFunctor<AGTabGroup> specFuncA(&*this, &AGTabGroup::ChangeDragged);
	m_tabHeader->RegisterCallBackFunction(&specFuncA);
	AngelGUI::AddGUI2DItem(this);
	
	D2D1::ColorF *c;
	c = &D2D1::ColorF::ColorF(D2D1::ColorF::DeepSkyBlue,1.0f);
	m_tabActiveColor.x = c->r;
	m_tabActiveColor.y = c->g;
	m_tabActiveColor.z = c->b;
	m_tabActiveColor.w = c->a;
	m_tabActiveColor.w = 1.0f;
}

void AngelGUISystem::GUI2D::AGTabGroup::Render()
{

}
void AngelGUISystem::GUI2D::AGTabGroup::Update(float dt)
{
	if (m_recreate)
	{
		//m_tabBackground->SetRectangle(m_rectangle);
		//m_tabBackground->SetPosition(m_position);
		m_tabHeader->SetRectangle(0, 0, m_rectangle.right, 20);
		m_tabHeader->SetPosition(m_position.x, m_position.y - 40);
		
		AlignTabs();
		AlignHeaders();
		m_recreate = false;
	}
}

AngelGUISystem::GUI2D::AGTabGroup::~AGTabGroup()
{
	COM_RELEASE(m_brush);
}


void AngelGUISystem::GUI2D::AGTabGroup::Destroy()
{
	COM_RELEASE(m_brush);
}

void AngelGUISystem::GUI2D::AGTabGroup::AddTab(AngelGUISystem::GUI2D::AGTab* item)
{
	static TSpecificFunctor<AGTabGroup> specFuncA(&*this, &AGTabGroup::TabChanged);
	
	item->m_isInTabGroupe = true;
	item->SetPosition(m_position);
	item->SetRectangle(m_rectangle);
	AGButton * tabHeader = new AGButton();
	tabHeader->Initialize();
	tabHeader->RegisterCallBackFunction(&specFuncA);
	tabHeader->SetText(item->GetTabHeader()->GetText().c_str());
	m_tabGroupeHeaders.push_back(tabHeader);
	AlignTabs();
	m_tabs.push_back(item);
	AlignHeaders();
	FocusOnSelectedTab();
}

void AngelGUISystem::GUI2D::AGTabGroup::SetActive(bool b)
{
	for (auto member : m_tabs)
	{
		if (b == true)
			member->SetActive(true);
		else
			member->SetActive(false);
	}
}

void AngelGUISystem::GUI2D::AGTabGroup::SetActive(bool &b)
{
	for (auto member : m_tabs)
	{
		if (b == true)
			member->SetActive(true);
		else
			member->SetActive(false);
	}
}

void AngelGUISystem::GUI2D::AGTabGroup::AlignHeaders()
{
	float tabWidth = m_rectangle.right / m_tabGroupeHeaders.size();
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		D2D1_RECT_F rect;
		rect.top = 0;
		rect.bottom = 20;
		rect.left = 0;
		rect.right = tabWidth;
		m_tabGroupeHeaders[i]->SetRectangle(rect);
		DirectX::XMFLOAT2 position;
		position.x = m_position.x+ i*tabWidth;
		position.y = m_position.y - 20;
		m_tabGroupeHeaders[i]->SetPosition(position);

	}
}

void AngelGUISystem::GUI2D::AGTabGroup::AlignTabs()
{
	for (unsigned int i = 0;i < m_tabs.size();i++)
	{
		m_tabs[i]->SetRectangle(m_rectangle);
		m_tabs[i]->SetPosition(m_position);
	}
}

void AngelGUISystem::GUI2D::AGTabGroup::OnMouseMove(float x, float y)
{
	if (m_dragged)
	{
		m_position.x = x;
		m_position.y = y;
		//AlignItems();
		m_recreate = true;
	}
}

void AngelGUISystem::GUI2D::AGTabGroup::OnMouseRelease(UINT message, float x, float y)
{
	m_dragged = false;
}

void AngelGUISystem::GUI2D::AGTabGroup::TabChanged()
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		
		if (m_tabGroupeHeaders[i]->IsClicked())
		{
			m_currentTabIndex = i;
			break;
		}
	}
	FocusOnSelectedTab();
}

void AngelGUISystem::GUI2D::AGTabGroup::FocusOnSelectedTab()
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		m_tabGroupeHeaders[i]->SetDefaultColor(m_defaultColor);
		m_tabGroupeHeaders[i]->SetOnHoverColor(m_onHoverColor);
		m_tabGroupeHeaders[i]->SetOnClickColor(m_onClickColor);
		if (i == m_currentTabIndex)
		{
			m_tabs[i]->SetActive(true);
			continue;
		}
		m_tabs[i]->SetActive(false);
	}

	m_tabGroupeHeaders[m_currentTabIndex]->SetDefaultColor(m_tabActiveColor);
	m_tabGroupeHeaders[m_currentTabIndex]->SetOnHoverColor(m_tabActiveColor);
	m_tabGroupeHeaders[m_currentTabIndex]->SetOnClickColor(m_tabActiveColor);
}


void AngelGUISystem::GUI2D::AGTabGroup::SetTabsDefaultColor(float r, float g, float b, float a)
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		m_tabGroupeHeaders[i]->SetDefaultColor(DirectX::XMFLOAT4(r, g, b, a));
	}
	m_defaultColor = DirectX::XMFLOAT4(r, g, b, a);
}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabsDefaultColor(float &r, float &g, float &b, float &a)
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		m_tabGroupeHeaders[i]->SetDefaultColor(DirectX::XMFLOAT4(r, g, b, a));
	}
	m_defaultColor = DirectX::XMFLOAT4(r, g, b, a);
}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabsHoverColor(float r, float g, float b, float a)
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		m_tabGroupeHeaders[i]->SetOnHoverColor(DirectX::XMFLOAT4(r, g, b, a));
	}
	m_onHoverColor = DirectX::XMFLOAT4(r, g, b, a);

}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabsHoverColor(float &r, float &g, float &b, float &a)
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		m_tabGroupeHeaders[i]->SetOnHoverColor(DirectX::XMFLOAT4(r, g, b, a));
	}
	m_onHoverColor = DirectX::XMFLOAT4(r, g, b, a);

}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabsClickColor(float r, float g, float b, float a)
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		m_tabGroupeHeaders[i]->SetOnClickColor(DirectX::XMFLOAT4(r, g, b, a));
	}
	m_onClickColor = DirectX::XMFLOAT4(r, g, b, a);
}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabsClickColor(float &r, float &g, float &b, float &a)
{
	for (unsigned int i = 0;i < m_tabGroupeHeaders.size();i++)
	{
		m_tabGroupeHeaders[i]->SetOnClickColor(DirectX::XMFLOAT4(r, g, b, a));
	}
	m_onClickColor = DirectX::XMFLOAT4(r, g, b, a);
}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabsActiveColor(float r, float g, float b, float a)
{
	m_tabActiveColor = DirectX::XMFLOAT4(r, g, b, a);
}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabsActiveColor(float &r, float &g, float &b, float &a)
{
	m_tabActiveColor = DirectX::XMFLOAT4(r, g, b, a);
}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabHeaderText(std::string &txt)
{
	m_tabHeader->SetText(txt);
}

void AngelGUISystem::GUI2D::AGTabGroup::SetTabHeaderTextColor(float &r, float &g, float &b, float &a)
{
	m_tabHeader->SetTextColor(DirectX::XMFLOAT4(r, g, b, a));
}