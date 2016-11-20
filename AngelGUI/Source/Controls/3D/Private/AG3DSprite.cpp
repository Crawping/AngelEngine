#include"../Public/AG3DSprite.h"
#include"../../../Core/AngelGUI.h"

AngelGUISystem::GUI3D::AGSprite::AGSprite() : IGUI3D()
{
	
}

AngelGUISystem::GUI3D::AGSprite::~AGSprite()
{

}

void AngelGUISystem::GUI3D::AGSprite::Initialize()
{
	COM_RELEASE(m_brush);
	m_renderTarget2D
		->CreateSolidColorBrush(D2D1::ColorF(
			m_defaultColor.x,
			m_defaultColor.y,
			m_defaultColor.z,
			m_defaultColor.w),
			&m_brush);

	AngelGUI::AddGUI3DItem(this);
}

void AngelGUISystem::GUI3D::AGSprite::Render()
{
	m_renderTarget2D->BeginDraw();
	D2D1_COLOR_F color;
	color.a = m_currentColor.w;
	color.b = m_currentColor.z;
	color.g = m_currentColor.y;
	color.r = m_currentColor.x;
	m_renderTarget2D->Clear(color);

	m_renderTarget2D->EndDraw();

	__super::Render();
}

void AngelGUISystem::GUI3D::AGSprite::Update(float dt
	, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
	__super::Update(dt, view, projection);
}

void AngelGUISystem::GUI3D::AGSprite::Destroy()
{
	COM_RELEASE(m_brush);
	COM_RELEASE(m_renderTarget2D);
	COM_RELEASE(m_bitmapImage);

}

void AngelGUISystem::GUI3D::AGSprite::SetActive(bool b)
{
	m_active = b;
}

void AngelGUISystem::GUI3D::AGSprite::SetActive(bool &b)
{
	if (b == true)
		m_active = true;
	else
		m_active = false;
}