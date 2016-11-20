#pragma once

#include<DirectXMath.h>

#define __VECTOR

	template<int size>
	class Vector;
#pragma region Vector2D

	template<>
	class Vector<2>
	{
		friend class ScriptManager;
	public:

		Vector();
		Vector(float _first, float _second);
		Vector(DirectX::XMFLOAT2 vec);
		~Vector();

		Vector(const Vector &that);
		Vector(Vector &&that);

		Vector &operator =(Vector const &that);
		Vector &operator =(Vector &&that);

		Vector &operator +(const Vector &rhs);
		Vector &operator -(const Vector &rhs);
		Vector &operator ++();
		Vector &operator ++(int);
		Vector &operator --();
		Vector &operator --(int);
		Vector &operator +=(const Vector &rhs);
		Vector &operator -=(const Vector &rhs);
		bool operator ==(const Vector &rhs);
		const bool operator ==(const Vector &rhs) const;
		bool operator !=(const Vector &rhs);
		const bool operator !=(const Vector &rhs) const;
		bool operator >(const Vector &rhs);
		const bool operator >(const Vector &rhs) const;
		bool operator <(const Vector &rhs);
		const bool operator <(const Vector &rhs) const;
		bool operator <=(const Vector &rhs);
		const bool operator <=(const Vector &rhs) const;
		bool operator >=(const Vector &rhs);
		const bool operator >=(const Vector &rhs)const;
		float  operator[](int i);
		const float operator[](int i) const;

		DirectX::XMFLOAT2 GetXMFLOAT2() const;
		DirectX::XMVECTOR GetXMVECTOR() const;

		float Magnitude();
		const float Magnitude() const;
		float Distance(const Vector &rhs);
		const float Distance(const Vector & rhs) const;

		float Dot(const Vector &rhs);
		const float Dot(const Vector &rhs) const;

		float Angle(const Vector &rhs);
		const float Angle(const Vector &rhs) const;

#pragma region Getter
		float GetX()															{ return this->m_x; }
		const float GetX() const												{ return this->m_x; }
		float GetY()															{ return this->m_y; }
		const float GetY() const												{ return this->m_y; }
#pragma endregion

#pragma region Setter
		void SetX(float _x)														{ this->m_x = _x; }
		void SetY(float _y)														{ this->m_y = _y; }
#pragma endregion
	public:
		float																	m_x;
		float																	m_y;
	};
#pragma endregion

#pragma region Vector3D

	template<>
	class Vector<3>
	{
		friend class ScriptManager;
	public:

		Vector();
		Vector(float _first, float _second, float _third);
		Vector(DirectX::XMFLOAT3 vec);
		Vector(DirectX::XMVECTOR vec);
		~Vector();

		Vector(const Vector &that);
		Vector(Vector &&that);

		Vector &operator =(Vector const &that);
		Vector &operator =(Vector &&that);

		Vector &operator +(const Vector &rhs);
		Vector &operator -(const Vector &rhs);
		Vector &operator ++();
		Vector &operator ++(int);
		Vector &operator --();
		Vector &operator --(int);
		Vector &operator +=(const Vector &rhs);
		Vector &operator -=(const Vector &rhs);
		bool operator ==(const Vector &rhs);
		const bool operator ==(const Vector &rhs) const;
		bool operator !=(const Vector &rhs);
		const bool operator !=(const Vector &rhs) const;
		bool operator >(const Vector &rhs);
		const bool operator >(const Vector &rhs) const;
		bool operator <(const Vector &rhs);
		const bool operator <(const Vector &rhs) const;
		bool operator <=(const Vector &rhs);
		const bool operator <=(const Vector &rhs) const;
		bool operator >=(const Vector &rhs);
		const bool operator >=(const Vector &rhs)const;
		float  operator[](int i);
		const float operator[](int i) const;

		DirectX::XMFLOAT3 GetXMFLOAT3() const;
		DirectX::XMVECTOR GetXMVECTOR() const;

		float Magnitude();
		const float Magnitude() const;
		float Distance(const Vector &rhs);
		const float Distance(const Vector & rhs) const;

		float Dot(const Vector &rhs);
		const float Dot(const Vector &rhs) const;

		float Angle(const Vector &rhs);
		const float Angle(const Vector &rhs) const;

		Vector Multiply(float &num);

		Vector Normalize();

#pragma region Getter
		float GetX()															{ return this->m_x; }
		const float GetX() const												{ return this->m_x; }
		float GetY()															{ return this->m_y; }
		const float GetY() const												{ return this->m_y; }
		float GetZ()															{ return this->m_z; }
		const float GetZ() const												{ return this->m_z; }
#pragma endregion

#pragma region Setter
		void SetX(float _x)														{ this->m_x = _x; }
		void SetY(float _y)														{ this->m_y = _y; }
		void SetZ(float _z)														{ this->m_z = _z; }
#pragma endregion
	public:
		float																	m_x;
		float																	m_y;
		float																	m_z;
	};
#pragma endregion

#pragma region Vector4D

	template<>
	class Vector<4>
	{

		friend class ScriptManager;
	public:

		Vector();
		Vector(float _first, float _second, float _third, float _forth);
		~Vector();

		Vector(const Vector &that);
		Vector(Vector &&that);

		Vector &operator =(Vector const &that);
		Vector &operator =(Vector &&that);

		Vector &operator +(const Vector &rhs);
		Vector &operator -(const Vector &rhs);
		Vector &operator ++();
		Vector &operator ++(int);
		Vector &operator --();
		Vector &operator --(int);
		Vector &operator +=(const Vector &rhs);
		Vector &operator -=(const Vector &rhs);
		bool operator ==(const Vector &rhs);
		const bool operator ==(const Vector &rhs) const;
		bool operator !=(const Vector &rhs);
		const bool operator !=(const Vector &rhs) const;
		bool operator >(const Vector &rhs);
		const bool operator >(const Vector &rhs) const;
		bool operator <(const Vector &rhs);
		const bool operator <(const Vector &rhs) const;
		bool operator <=(const Vector &rhs);
		const bool operator <=(const Vector &rhs) const;
		bool operator >=(const Vector &rhs);
		const bool operator >=(const Vector &rhs)const;
		float  operator[](int i);
		const float operator[](int i) const;

		DirectX::XMFLOAT4 GetXMFLOAT4() const;
		DirectX::XMVECTOR GetXMVECTOR() const;

		float Magnitude();
		const float Magnitude() const;
		float Distance(const Vector &rhs);
		const float Distance(const Vector & rhs) const;

		float Dot(const Vector &rhs);
		const float Dot(const Vector &rhs) const;

		float Angle(const Vector &rhs);
		const float Angle(const Vector &rhs) const;

#pragma region Getter
		float GetX()															{ return this->m_x; }
		const float GetX() const												{ return this->m_x; }
		float GetY()															{ return this->m_y; }
		const float GetY() const												{ return this->m_y; }
		float GetZ()															{ return this->m_z; }
		const float GetZ() const												{ return this->m_z; }
		float GetW()															{ return this->m_w; }
		const float GetW() const												{ return this->m_w; }
#pragma endregion

#pragma region Setter

		void SetX(float _x)														{ this->m_x = _x; }
		void SetY(float _y)														{ this->m_y = _y; }
		void SetZ(float _z)														{ this->m_z = _z; }
		void SetW(float _w)														{ this->m_w = _w; }

#pragma endregion
	public:

		float																	m_x;
		float																	m_y;
		float																	m_z;
		float																	m_w;
	};
#pragma endregion

#ifndef __VECTOR_TYPE
#define __VECTOR_TYPE
using Vector2 = Vector<2>;

using Vector3 = Vector<3>;

using Vector4 = Vector<4>;
#endif