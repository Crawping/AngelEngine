#include"../Public/AGTab.h"
#include"../Public/AGSprite.h"
#include"../Public/AGButton.h"
#include"../../../Core/AngelGUI.h"

AngelGUISystem::GUI2D::AGTab::AGTab() : IGUI2D() ,
m_dragged(false) , m_isInTabGroupe(false)
{

}

void AngelGUISystem::GUI2D::AGTab::Initialize()
{
	m_tabBackground = new AGSprite();
	m_tabBackground->Initialize();
	m_tabBackground->SetRectangle(m_rectangle);
	m_tabBackground->SetPosition(m_position);

	m_tabHeader = new AGButton();
	m_tabHeader->Initialize();
	m_tabHeader->SetRectangle(0, 0, m_rectangle.right, 20);
	m_tabHeader->SetPosition(m_position.x, m_position.y - 20);
	static TSpecificFunctor<AGTab> specFuncA(&*this, &AGTab::ChangeDragged);
	m_tabHeader->RegisterCallBackFunction(&specFuncA);
	AngelGUI::AddGUI2DItem(this);
}

void AngelGUISystem::GUI2D::AGTab::Render()
{
	
}
void AngelGUISystem::GUI2D::AGTab::Update(float dt)
{
	if (m_recreate)
	{
		m_tabBackground->SetRectangle(m_rectangle);
		m_tabBackground->SetPosition(m_position);
		m_tabHeader->SetRectangle(0, 0, m_rectangle.right, 20);
		m_tabHeader->SetPosition(m_position.x, m_position.y - 20);
		AlignItems();
		m_recreate = false;
	}

	if (m_isInTabGroupe)
	{
		m_tabHeader->SetActive(false);
	}	
}

AngelGUISystem::GUI2D::AGTab::~AGTab()
{
	COM_RELEASE(m_brush);
}


void AngelGUISystem::GUI2D::AGTab::Destroy()
{
	COM_RELEASE(m_brush);
}

void AngelGUISystem::GUI2D::AGTab::AddItem(AngelGUISystem::GUI2D::IGUI2D* item)
{
	m_items.push_back(item);
	m_cachedPositions.push_back(item->GetPosition());
	AlignItems();
}

void AngelGUISystem::GUI2D::AGTab::SetActive(bool b)
{
	for (auto member : m_items)
	{
		if (b == true)
			member->SetActive(true);
		else
			member->SetActive(false);
	}
		
	if (b == true)
		m_tabBackground->SetActive(true);
	else
		m_tabBackground->SetActive(false);

}

void AngelGUISystem::GUI2D::AGTab::SetActive(bool &b)
{
	for (auto member : m_items)
	{
		if (b == true)
			member->SetActive(true);
		else
			member->SetActive(false);
	}

	if (b == true)
		m_tabBackground->SetActive(true);
	else
		m_tabBackground->SetActive(false);
}

void AngelGUISystem::GUI2D::AGTab::AlignItems()
{
	for (unsigned int i = 0;i < m_items.size();i++)
	{
		DirectX::XMFLOAT2 position = m_cachedPositions[i];
		position.x += m_position.x;
		position.y += m_position.y;
		m_items[i]->SetPosition(position);
		//m_cachedPositions[i] = position;
	}
}

void AngelGUISystem::GUI2D::AGTab::OnMouseMove(float x,float y)
{
	if (m_dragged)
	{
		m_position.x = x;
		m_position.y = y;
		AlignItems();
		m_recreate = true;
	}
}

void AngelGUISystem::GUI2D::AGTab::OnMouseRelease(UINT message, float x, float y)
{
	if (m_dragged)
	{
		m_dragged = false;
	}
}

void AngelGUISystem::GUI2D::AGTab::SetText(std::string &text)
{
	m_tabHeader->SetText(text);
}

void AngelGUISystem::GUI2D::AGTab::SetBackgroundColor(float r, float g, float b, float a)
{
	m_tabBackground->m_brush->SetColor(D2D1::ColorF(r, g, b, a));
	m_tabHeader->SetDefaultColor(DirectX::XMFLOAT4(r, g, b, a));
}

void AngelGUISystem::GUI2D::AGTab::SetBackgroundColor(float &r, float &g, float &b, float &a)
{
	m_tabHeader->SetDefaultColor(DirectX::XMFLOAT4(r, g, b, a));
	m_tabBackground->m_brush->SetColor(D2D1::ColorF(r, g, b, a));
}
void AngelGUISystem::GUI2D::AGTab::SetTabHeaderTextColor(float &r, float &g, float &b, float &a)
{
	m_tabHeader->SetTextColor(DirectX::XMFLOAT4(r, g, b, a));
}