#include"../Public/AGButton.h"
#include"../../../Core/AngelGUI.h"
#include"../Public/AGSprite.h"
#include"../Public/AGText.h"
#include"../../../../../AngelEngine/AngelSource/AngelCore/AngelSubSystem/Public/SubSystemManager.h"
#include"../../../../../AngelEngine/AngelSource/AngelCore/AngelSubSystem/Public/ScriptManager.h"

AngelGUISystem::GUI2D::AGButton::AGButton() : 
	IGUI2D() , m_rounded(true),
	m_radiusX(2),m_radiusY(2)
,m_sprite(nullptr)
{
	
}

void AngelGUISystem::GUI2D::AGButton::Initialize()
{
	AngelGUISystem::AngelGUI::AddGUI2DItem(this);
	m_sprite = new AGSprite();
	m_sprite->Initialize();
	m_sprite->SetRectangle(m_rectangle);
	m_text = new AGText();
	m_text->Initialize();
	m_text->SetRectangle(m_rectangle);
	m_text->SetEditable(false);
	m_text->m_useForButton = true;
	m_text->SetFittingMode(AGText::FIT_MODE::SCALE_DOWN_TO_FIT);
	COM_RELEASE(m_sprite->m_brush);
	AngelGUISystem::AngelGUI::GetRenderTarget2D()
		->CreateSolidColorBrush(D2D1::ColorF(
			m_defaultColor.x,
			m_defaultColor.y,
			m_defaultColor.z,
			m_defaultColor.w),
			&m_sprite->m_brush);
}
void AngelGUISystem::GUI2D::AGButton::Render()
{
	//m_sprite->Render();
	//m_text->Render();
}
void AngelGUISystem::GUI2D::AGButton::Update(float dt)
{
	if (m_recreate)
	{
		this->m_sprite->SetRectangle(m_rectangle);
		m_text->SetTransformationMatrix(m_sprite->m_transformMatrix);
		m_text->SetPosition(((m_sprite->m_rectangle.right*m_scale.x) - (m_sprite->m_rectangle.left*m_scale.x)) / 2 + m_position.x
			, ((m_sprite->m_rectangle.bottom*m_scale.y) - (m_sprite->m_rectangle.top)*m_scale.y) / 2 + m_position.y);
		m_text->SetScale(m_scale);
		m_sprite->SetScale(m_scale);
		m_sprite->SetPosition(m_position);
		m_sprite->SetRotation(m_rotationAngle);
		m_text->SetRectangle(m_rectangle);
		m_recreate = false;
		m_sprite->m_defaultColor = m_defaultColor;
	}
	if (!m_selected && !m_hovered)
	{
		{
			m_sprite->m_brush->SetColor
				(D2D1::ColorF(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, m_defaultColor.w));
		}
	}

	//rest selection if we are not hovering
	if (!m_hovered)
		m_selected = false;
}

AngelGUISystem::GUI2D::AGButton::~AGButton()
{
	COM_RELEASE(m_brush);
	COM_RELEASE(m_sprite->m_brush);
}

void AngelGUISystem::GUI2D::AGButton::OnMouseRelease(UINT message, float x, float y)
{
	if (m_hovered)
		DefaultOnHover();
	else
	{
		m_sprite->m_brush->SetColor
			(D2D1::ColorF(m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, m_defaultColor.w));
	}
}

void AngelGUISystem::GUI2D::AGButton::OnMousePress(UINT message, float x, float y)
{
	if (message == WM_LBUTTONDOWN)
	{
		BOOL contains = false;
		const D2D1::Matrix3x2F * mat = &m_sprite->m_transformMatrix;
		if (m_rounded)
		{
			if (m_sprite->m_rectangleRoundedGeometry)
			{
				m_sprite->m_rectangleRoundedGeometry->FillContainsPoint(
					D2D1::Point2F(x, y), mat, &contains);
			}
		}
		else
		{
			if (m_sprite->m_rectangleGeometry)
			{
				m_sprite->m_rectangleGeometry->FillContainsPoint(
					D2D1::Point2F(x, y), mat, &contains);
			}
		}

		if (contains == 1)
		{
			m_selected = true;
			for (unsigned int i = 0;i < m_callBackFunctions.size();i++)
				m_callBackFunctions[i]->Call();

#pragma region AngelScript
			for (unsigned int i = 0;i < m_callBackFunctionsScript.size();i++)
			{
				asIScriptContext *ctx =
					AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->
					PrepareContextFromPool(m_callBackFunctionsScript[i]);
				AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->ExecuteCall(ctx);
				AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->ReturnContextToPool(ctx);
			}
#pragma endregion
			DefaultOnClick();
		}
		else
			m_selected = false;
	}
}

void AngelGUISystem::GUI2D::AGButton::OnMouseMove(float x, float y)
{
	{
		BOOL contains = false;
		const D2D1::Matrix3x2F * mat = &m_sprite->m_transformMatrix;
		if (m_rounded)
		{
			if (m_sprite->m_rectangleRoundedGeometry)
			{
				m_sprite->m_rectangleRoundedGeometry->FillContainsPoint(
					D2D1::Point2F(x, y), mat, &contains);
			}
		}
		else
		{
			if (m_sprite->m_rectangleGeometry)
			{
				m_sprite->m_rectangleGeometry->FillContainsPoint(
					D2D1::Point2F(x, y), mat, &contains);
			}
		}
			
		if (contains == 1)
		{
			m_hovered = true;
			DefaultOnHover();
		}
		else
			m_hovered = false;
	}
}

void AngelGUISystem::GUI2D::AGButton::DefaultOnClick()
{
	m_sprite->m_brush->SetColor
		(D2D1::ColorF(m_onClickColor.x, m_onClickColor.y, m_onClickColor.z, m_onClickColor.w));
	m_currentColor = m_onClickColor;
}

void AngelGUISystem::GUI2D::AGButton::DefaultOnHover()
{
	m_sprite->m_brush->SetColor
		(D2D1::ColorF(m_onHoverColor.x, m_onHoverColor.y, m_onHoverColor.z, m_onHoverColor.w));
	m_currentColor = m_onHoverColor;
}

void AngelGUISystem::GUI2D::AGButton::SetText(const wchar_t * text)
{
	this->m_text->SetText(text);
}

void AngelGUISystem::GUI2D::AGButton::SetText(std::string & txt)
{
	wchar_t * wtxt =new wchar_t[txt.size()+1];
	for (unsigned int i = 0;i < txt.size();i++)
		wtxt[i] = txt[i];
	wtxt[txt.size()] = L'\0';
	this->m_text->SetText(wtxt);
	delete wtxt;
}

void AngelGUISystem::GUI2D::AGButton::SetTextColor(float r,float g,float b,float a)
{
	this->m_text->SetTextColor(DirectX::XMFLOAT4(r, g, b, a));
}

void AngelGUISystem::GUI2D::AGButton::SetTextColor(float &r, float &g, float &b, float &a)
{
	this->m_text->SetTextColor(DirectX::XMFLOAT4(r,g,b,a));
}

void AngelGUISystem::GUI2D::AGButton::SetTextColor(DirectX::XMFLOAT4 color)
{
	this->m_text->SetTextColor(color);
}

void AngelGUISystem::GUI2D::AGButton::SetBitmapPath(const wchar_t * p)
{
	this->m_sprite->SetBitmapPath(p);
}

void AngelGUISystem::GUI2D::AGButton::SetBitmapPath(std::string & p)
{
	std::wstring data;
	std::wstring toWstr(p.begin(), p.end());
	//data += L"\\";
	data += AngelGUI::GetTextureFolderPath();
	data += L"\\";
	data += toWstr;
	this->m_sprite->SetBitmapPath(data.c_str());
}


void AngelGUISystem::GUI2D::AGButton::SetRounded(bool b)
{
	m_rounded = b;
	m_sprite->SetRounded(b);
	m_recreate = true;
}

void AngelGUISystem::GUI2D::AGButton::SetRounded(bool &b)
{
	m_rounded = b;
	m_sprite->SetRounded(b);
	m_recreate = true;
}

void AngelGUISystem::GUI2D::AGButton::SetRoundedRadius(float x,float y)
{
	m_radiusX = x;
	m_radiusY = y;
	m_sprite->SetRoundedRadius(x,y);
	m_recreate = true;
}

void AngelGUISystem::GUI2D::AGButton::SetRoundedRadius(float &x, float &y)
{
	m_radiusX = x;
	m_radiusY = y;
	m_sprite->SetRoundedRadius(x, y);
	m_recreate = true;
}


void AngelGUISystem::GUI2D::AGButton::Destroy()
{
	m_sprite->Destroy();
	m_text->Destroy();
	for (unsigned int i = 0;i < m_callBackFunctionsScript.size();i++)
	{
		m_callBackFunctionsScript[i]->Release();
	}
}

void AngelGUISystem::GUI2D::AGButton::RegisterCallBackFunction(AngelGUISystem::GUI2D::TFunctor* callback)
{
	m_callBackFunctions.push_back(callback);
}

void AngelGUISystem::GUI2D::AGButton::RegisterCallBackFunctionScript(asIScriptFunction * callback)
{
	m_callBackFunctionsScript.push_back(callback);
}

void AngelGUISystem::GUI2D::AGButton::SetActive(bool b)
{
	m_active = b;
	if (b == true)
	{
		m_text->SetActive(true);
		m_sprite->SetActive(true);
	}
	else
	{
		m_text->SetActive(false);
		m_sprite->SetActive(false);
	}
}

void AngelGUISystem::GUI2D::AGButton::SetActive(bool &b)
{
	m_active = b;
	if (b == true)
	{
		m_text->SetActive(true);
		m_sprite->SetActive(true);
	}
	else
	{
		m_text->SetActive(false);
		m_sprite->SetActive(false);
	}
}

std::wstring AngelGUISystem::GUI2D::AGButton::GetText() const
{
	return m_text->GetText();
}

void AngelGUISystem::GUI2D::AGButton::SetTextFontSize(float size)
{
	m_text->SetFontSize(size);
}
void AngelGUISystem::GUI2D::AGButton::SetTextFontSize(float &size)
{
	m_text->SetFontSize(size);
}