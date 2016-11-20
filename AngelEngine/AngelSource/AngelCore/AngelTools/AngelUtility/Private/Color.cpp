#include"../Public/Color.h"

AngelCore::AngelTools::AngelUtility::AngelColor::AngelColor()
{
	this->m_color = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

AngelCore::AngelTools::AngelUtility::AngelColor::AngelColor(const Vector4 &_newColor)
{
	this->m_color->SetX(_newColor.GetX());
	this->m_color->SetY(_newColor.GetY());
	this->m_color->SetZ(_newColor.GetZ());
	this->m_color->SetW(_newColor.GetW());
}

AngelCore::AngelTools::AngelUtility::AngelColor::AngelColor(float _red, float _green, float _blue, float _alpha)
{
	this->m_color = new Vector4();

	this->m_color->SetX(_red);
	this->m_color->SetY(_green);
	this->m_color->SetZ(_blue);
	this->m_color->SetW(_alpha);
}

AngelCore::AngelTools::AngelUtility::AngelColor::AngelColor(const AngelColor &that)
{
	this->m_color = new Vector4();

	this->m_color->SetX(that.m_color->GetX());
	this->m_color->SetY(that.m_color->GetY());
	this->m_color->SetZ(that.m_color->GetZ());
	this->m_color->SetW(that.m_color->GetW());
}

AngelCore::AngelTools::AngelUtility::AngelColor::AngelColor(AngelColor &&that)
{
	this->m_color = new Vector4();
	this->m_color = std::move(that.m_color);
}

AngelCore::AngelTools::AngelUtility::AngelColor::~AngelColor()
{
	delete this->m_color;
}

void AngelCore::AngelTools::AngelUtility::AngelColor::SetColor(const Vector4 &_newColor)
{
	if (!this->m_color)
		this->m_color = new Vector4();

	this->m_color->SetX(_newColor.GetX());
	this->m_color->SetY(_newColor.GetY());
	this->m_color->SetZ(_newColor.GetZ());
	this->m_color->SetW(_newColor.GetW());
}

AngelCore::AngelTools::AngelUtility::AngelColor & AngelCore::AngelTools::AngelUtility::AngelColor::operator=(const AngelColor &that)
{
	if (!this->m_color)
		this->m_color = new Vector4();

	this->m_color->SetX(that.m_color->GetX());
	this->m_color->SetY(that.m_color->GetY());
	this->m_color->SetZ(that.m_color->GetZ());
	this->m_color->SetW(that.m_color->GetW());
	return *this;
}

AngelCore::AngelTools::AngelUtility::AngelColor &AngelCore::AngelTools::AngelUtility::AngelColor::operator=(AngelColor &&that)
{
	if (!this->m_color)
		this->m_color = new Vector4();
	this->m_color = std::move(that.m_color);
	return *this;
};

bool AngelCore::AngelTools::AngelUtility::AngelColor::operator==(const AngelColor &that)
{
	if (this->m_color == that.m_color)
		return true;

	return false;
}

const bool AngelCore::AngelTools::AngelUtility::AngelColor::operator==(const AngelColor &that) const
{
	if (this->m_color == that.m_color)
		return true;

	return false;
}

bool AngelCore::AngelTools::AngelUtility::AngelColor::operator!=(const AngelColor &that)
{
	if (this->m_color != that.m_color)
		return true;

	return false;
}

const bool AngelCore::AngelTools::AngelUtility::AngelColor::operator!=(const AngelColor &that) const
{
	if (this->m_color != that.m_color)
		return true;

	return false;
}

AngelCore::AngelTools::AngelUtility::AngelColor &AngelCore::AngelTools::AngelUtility::AngelColor::operator=(const DirectX::XMFLOAT4 &that)
{
	if (!this->m_color)
		this->m_color = new Vector4();

	this->m_color->SetX(that.x);
	this->m_color->SetY(that.y);
	this->m_color->SetZ(that.z);
	this->m_color->SetW(that.w);
	return *this;
}

AngelCore::AngelTools::AngelUtility::AngelColor::AngelColor(const DirectX::XMFLOAT4 &that)
{
	if (!this->m_color)
		this->m_color = new Vector4();

	this->m_color->SetX(that.x);
	this->m_color->SetY(that.y);
	this->m_color->SetZ(that.z);
	this->m_color->SetW(that.w);
}

#pragma region Definition
AngelCore::AngelTools::AngelUtility::AngelColor AngelCore::AngelTools::AngelColors::Red(1.0f, 0.0f, 0.0f, 0.0f);
AngelCore::AngelTools::AngelUtility::AngelColor AngelCore::AngelTools::AngelColors::Blue(0.0f, 0.0f, 1.0f, 0.0f);
AngelCore::AngelTools::AngelUtility::AngelColor AngelCore::AngelTools::AngelColors::Green(0.0f, 1.0f, 0.0f, 0.0f);
AngelCore::AngelTools::AngelUtility::AngelColor AngelCore::AngelTools::AngelColors::White(1.0f, 1.0f, 1.0f, 1.0f);
AngelCore::AngelTools::AngelUtility::AngelColor AngelCore::AngelTools::AngelColors::Black(0.0f, 0.0f, 0.0f, 0.0f);
AngelCore::AngelTools::AngelUtility::AngelColor AngelCore::AngelTools::AngelColors::Yellow(1.0f, 1.0f, 0.0f, 0.0f);
AngelCore::AngelTools::AngelUtility::AngelColor AngelCore::AngelTools::AngelColors::Aqua(0.0f, 1.0f, 1.0f, 0.0f);
#pragma endregion