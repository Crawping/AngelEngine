#pragma once

#include"../../AngelGlobalTools.h"

namespace AngelCore
{
	namespace AngelTools
	{
		namespace AngelUtility
		{
			class AngelColor
			{
			public:
				AngelColor();
				AngelColor(float _red, float _green, float _blue, float _alpha);
				AngelColor(Vector4 const &_newColor);
				AngelColor(const AngelColor &that);
				AngelColor(AngelColor &&that);
				AngelColor(const DirectX::XMFLOAT4 &that);
				~AngelColor();

				AngelColor &operator =(const AngelColor &that);
				AngelColor &operator =(AngelColor &&that);
				AngelColor &operator =(const DirectX::XMFLOAT4 &that);

				bool operator ==(const AngelColor &that);
				const bool operator ==(const AngelColor &that) const;
				bool operator !=(const AngelColor &that);
				const bool operator !=(const AngelColor &that) const;

				const Vector4 *GetColor()											{ return this->m_color; }

				void SetColor(Vector4 *_newColor)									{ this->m_color = _newColor; }

				void SetColor(const Vector4 & _newColor);

			private:

				Vector4 *m_color;
			};
		}

		namespace AngelColors
		{
			extern AngelUtility::AngelColor Red;
			extern AngelUtility::AngelColor Green;
			extern AngelUtility::AngelColor Blue;
			extern AngelUtility::AngelColor White;
			extern AngelUtility::AngelColor Yellow;
			extern AngelUtility::AngelColor Black;
			extern AngelUtility::AngelColor Aqua;
		}
	}
}