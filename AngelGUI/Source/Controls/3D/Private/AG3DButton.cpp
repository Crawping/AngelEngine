#include"../Public/AG3DButton.h"
#include"../Public/AG3DText.h"
#include"../../../Core/AngelGUI.h"
#include"../../../Core/WICTextureLoader.h"
#include"../../../../../AngelEngine/AngelSource/AngelCore/AngelSubSystem/Public/SubSystemManager.h"
#include"../../../../../AngelEngine/AngelSource/AngelCore/AngelSubSystem/Public/ScriptManager.h"

AngelGUISystem::GUI3D::AGButton::AGButton() : IGUI3D() , m_renderUsingBitmap(false)
, m_bitmapSRV(nullptr)
{

}

AngelGUISystem::GUI3D::AGButton::~AGButton()
{
	COM_RELEASE(m_brush);
	COM_RELEASE(m_renderTarget2D);
}

void AngelGUISystem::GUI3D::AGButton::Initialize()
{
	m_text = new AGText();
	m_text->Initialize();
	m_text->SetRectangle(m_rectangle);
	m_text->SetFittingMode(AGText::FIT_MODE::SCALE_DOWN_TO_FIT);
	m_text->m_useForButton = true;
	m_text->SetEditable(false);
	AngelGUI::AddGUI3DItem(this);
}

void AngelGUISystem::GUI3D::AGButton::Render()
{
	__super::Render();
}

void AngelGUISystem::GUI3D::AGButton::Update(float dt
	, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
	if (m_recreate)
	{
		m_text->SetPosition(m_position);
		m_text->SetScale(m_scale);
		m_text->SetRotation(m_rotation);
		m_text->SetRectangle(m_rectangle);
		m_recreate = false;
	}

	if(m_billboard==true)
		m_text->SetBillboard(true);
	else
		m_text->SetBillboard(false);

	if (m_selected)
		DefaultOnClick();
	else if (m_hovered && !m_selected)
		DefaultOnHover();
	else
		m_text->m_currentColor = m_currentColor = m_defaultColor;

	/*if (!m_selected)
	{
		m_text->m_currentColor = m_currentColor = m_defaultColor;
	}*/
	__super::Update(dt, view, projection);
}

void AngelGUISystem::GUI3D::AGButton::Destroy()
{
	COM_RELEASE(m_brush);
	COM_RELEASE(m_renderTarget2D);
}

void AngelGUISystem::GUI3D::AGButton::SetActive(bool b)
{
	if (b == true)
	{
		m_active = true;
		m_text->SetActive(true);
	}

	else
	{
		m_active = false;
		m_text->SetActive(false);
	}
}

void AngelGUISystem::GUI3D::AGButton::SetActive(bool &b)
{
	if (b == true)
	{
		m_active = true;
		m_text->SetActive(true);
	}

	else
	{
		m_active = false;
		m_text->SetActive(false);
	}
}

void AngelGUISystem::GUI3D::AGButton::OnMouseRelease(UINT message, float x, float y)
{
	m_selected = false;
}

void AngelGUISystem::GUI3D::AGButton::OnMousePress(UINT message, float x, float y)
{
	if (message == WM_LBUTTONDOWN)
	{
		float dist = 0;
		if (TestIntersects(nullptr, nullptr, &dist, x, y))
		{
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
			m_selected = true;
		}		
	}
}

void AngelGUISystem::GUI3D::AGButton::OnMouseMove(float x, float y)
{
	{
		float dist = 0;
		if (TestIntersects(&m_text->m_mousePosition, nullptr, &dist, x, y))
		{
			m_hovered = true;
		}
		else
		{
			m_hovered = false;
		}
	}
}

void AngelGUISystem::GUI3D::AGButton::DefaultOnClick()
{
	m_text->m_currentColor = m_onClickColor;
	m_currentColor = m_onClickColor;
}

void AngelGUISystem::GUI3D::AGButton::DefaultOnHover()
{
	m_text->m_currentColor = m_onHoverColor;
	m_currentColor = m_onHoverColor;
}

void AngelGUISystem::GUI3D::AGButton::SetText(const wchar_t * text)
{
	m_text->SetText(text);
}

void AngelGUISystem::GUI3D::AGButton::SetText(std::string &text)
{
	m_text->SetText(text);
}

void AngelGUISystem::GUI3D::AGButton::SetTextColor(float r, float g, float b, float a)
{
	this->m_text->SetTextColor(DirectX::XMFLOAT4(r, g, b, a));
}

void AngelGUISystem::GUI3D::AGButton::SetTextColor(float &r, float &g, float &b, float &a)
{
	this->m_text->SetTextColor(DirectX::XMFLOAT4(r, g, b, a));
}

void AngelGUISystem::GUI3D::AGButton::SetTextColor(DirectX::XMFLOAT4 color)
{
	this->m_text->SetTextColor(color);
}

void AngelGUISystem::GUI3D::AGButton::RegisterCallBackFunction(AngelGUISystem::GUI3D::TFunctor* callback)
{
	m_callBackFunctions.push_back(callback);
}

void AngelGUISystem::GUI3D::AGButton::RegisterCallBackFunctionScript(asIScriptFunction * callback)
{
	m_callBackFunctionsScript.push_back(callback);
}

std::wstring AngelGUISystem::GUI3D::AGButton::GetText() const
{
	return m_text->GetText();
}

void AngelGUISystem::GUI3D::AGButton::SetTextFontSize(float size)
{
	m_text->SetFontSize(size);
}
void AngelGUISystem::GUI3D::AGButton::SetTextFontSize(float &size)
{
	m_text->SetFontSize(size);
}

void AngelGUISystem::GUI3D::AGButton::SetBitmapPath(const wchar_t * p)
{
	std::wstring data;
	std::wstring toWstr = p;
	data += AngelGUI::GetTextureFolderPath();
	data += L"\\";
	data += toWstr;
	HR(DirectX::CreateWICTextureFromFile(
		AngelGUI::GetDevice3D().Get()
		,data.c_str(),nullptr, m_resourceSRV.GetAddressOf()));

	m_renderFromResource = true;
	m_text->SetActive(false);
}

void AngelGUISystem::GUI3D::AGButton::SetBitmapPath(std::string & p)
{
	std::wstring data;
	std::wstring toWstr(p.begin(),p.end());
	data += AngelGUI::GetTextureFolderPath();
	data += L"\\";
	data += toWstr;
	HR(DirectX::CreateWICTextureFromFile(
		AngelGUI::GetDevice3D().Get()
		, data.c_str(), nullptr, m_resourceSRV.GetAddressOf()));
	m_renderFromResource = true;
	m_text->SetActive(false);
}