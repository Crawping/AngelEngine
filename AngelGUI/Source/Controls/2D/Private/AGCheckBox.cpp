#include"../Public/AGCheckBox.h"
#include"../Public/AGSprite.h"
#include"../Public/AGButton.h"
#include"../Public/AGText.h"
#include"../../../Core/AngelGUI.h"


AngelGUISystem::GUI2D::AGCheckBox::AGCheckBox() : IGUI2D() , m_checkBox(nullptr)
,m_checkBoxFill(nullptr),m_checkBoxText(nullptr), m_isChecked(false)
{

}

void AngelGUISystem::GUI2D::AGCheckBox::Initialize()
{
	m_checkBox = new AGButton();
	m_checkBox->Initialize();
	m_checkBox->SetRounded(false);
	m_checkBox->SetRectangle(m_rectangle);
	
	m_callBack = new TSpecificFunctor<AGCheckBox> (this, &AGCheckBox::ChangeChecked);
	//static TSpecificFunctor<AGCheckBox> specFuncA(this, &AGCheckBox::ChangeChecked);
	m_checkBox->RegisterCallBackFunction(m_callBack);

	m_checkBoxFill = new AGSprite();
	m_checkBoxFill->Initialize();
	D2D1_RECT_F fillRect;
	fillRect = m_rectangle;
	fillRect.right -= 5;
	fillRect.bottom -= 5;
	fillRect.top += 5;
	fillRect.left += 5;
	m_checkBoxFill->SetRectangle(fillRect);
	m_checkBoxFill->SetDefaultColor(m_defaultColor);

	m_checkBoxText = new AGText();
	m_checkBoxText->Initialize();
	m_checkBoxText->SetRectangle(m_rectangle);
	m_checkBoxText->SetEditable(false);
	m_checkBoxText->SetFittingMode(AGText::FIT_MODE::SCALE_DOWN_TO_FIT);

	m_checkBoxText->SetRectangle(D2D1::RectF(0, 0, 200, m_rectangle.bottom));
	m_checkBoxText->SetPosition(DirectX::XMFLOAT2(m_rectangle.right, (m_rectangle.bottom)));
	AngelGUI::AddGUI2DItem(this);
}

void AngelGUISystem::GUI2D::AGCheckBox::Render()
{
	if (m_isChecked)
		m_checkBoxFill->SetActive(true);
	if (!m_isChecked)
		m_checkBoxFill->SetActive(false);
}

void AngelGUISystem::GUI2D::AGCheckBox::Update(float dt)
{
	if (m_recreate)
	{
		D2D1_RECT_F fillRect;
		fillRect = m_rectangle;
		fillRect.right -= 5;
		fillRect.bottom -= 5;
		fillRect.top += 5;
		fillRect.left += 5;
		m_checkBox->SetRectangle(m_rectangle);
		m_checkBoxFill->SetRectangle(fillRect);

		m_checkBox->SetPosition(m_position);
		m_checkBoxFill->SetPosition(m_position);
		m_checkBoxText->SetPosition(DirectX::XMFLOAT2(m_rectangle.right+m_position.x+3, 
			m_position.y));
		m_checkBoxText->SetRectangle(D2D1::RectF(0, 0, 200, m_rectangle.bottom));
		m_checkBox->SetOnClickColor(m_onClickColor);
		m_checkBox->SetOnHoverColor(m_onHoverColor);
	}
}

AngelGUISystem::GUI2D::AGCheckBox::~AGCheckBox()
{
	COM_RELEASE(m_brush);
}

void AngelGUISystem::GUI2D::AGCheckBox::Destroy()
{

	COM_RELEASE(m_brush);
}

void AngelGUISystem::GUI2D::AGCheckBox::SetText(wchar_t * text)
{
	m_checkBoxText->SetText(text);
}

void AngelGUISystem::GUI2D::AGCheckBox::SetText(std::string &text)
{
	m_checkBoxText->SetText(text);
}

void AngelGUISystem::GUI2D::AGCheckBox::SetActive(bool b)
{
	if (b == true)
	{
		m_checkBoxFill->SetActive(true);
		m_checkBoxText->SetActive(true);
		m_checkBox->SetActive(true);
	}
	else
	{
		m_checkBoxFill->SetActive(false);
		m_checkBoxText->SetActive(false);
		m_checkBox->SetActive(false);
	}
}

void AngelGUISystem::GUI2D::AGCheckBox::SetActive(bool &b)
{
	if (b == true)
	{
		m_checkBoxFill->SetActive(true);
		m_checkBoxText->SetActive(true);
		m_checkBox->SetActive(true);
	}
	else
	{
		m_checkBoxFill->SetActive(false);
		m_checkBoxText->SetActive(false);
		m_checkBox->SetActive(false);
	}
}

void AngelGUISystem::GUI2D::AGCheckBox::SetBackgroundColor(float r, float g, float b, float a)
{
	m_checkBox->SetDefaultColor(DirectX::XMFLOAT4(r, g, b, a));
}

void AngelGUISystem::GUI2D::AGCheckBox::SetBackgroundColor(float &r, float &g, float &b, float &a)
{
	m_checkBox->SetDefaultColor(DirectX::XMFLOAT4(r, g, b, a));
}

void AngelGUISystem::GUI2D::AGCheckBox::SetFilledColor(float r, float g, float b, float a)
{
	m_checkBoxFill->m_brush->SetColor(D2D1::ColorF(r, g, b, a));
}

void AngelGUISystem::GUI2D::AGCheckBox::SetFilledColor(float &r, float &g, float &b, float &a)
{
	m_checkBoxFill->m_brush->SetColor(D2D1::ColorF(r, g, b, a));
}