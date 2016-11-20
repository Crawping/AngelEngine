#pragma once
#include<d3d11_1.h>
#include<wincodec.h>
#include<wrl\client.h>
#include<dwrite_1.h>
#include<dwrite.h>
#include<d2d1_1.h>
#include<DirectXMath.h>
#include<angelscript.h>
#include"../../../Core/GlobalTools.h"
#include"../../../Core/Vector.h"

namespace AngelGUISystem
{
	namespace GUI2D
	{
		class TFunctor
		{
		public:

			// two possible functions to call member function. virtual cause derived
			// classes will use a pointer to an object and a pointer to a member function
			// to make the function call
			virtual void operator()() = 0;  // call using operator
			virtual void Call() = 0;        // call using function
		};


		// derived template class
		template <class TClass> class TSpecificFunctor : public TFunctor
		{
		private:
			void (TClass::*fpt)();   // pointer to member function
			TClass* pt2Object;                  // pointer to object

		public:

			// constructor - takes pointer to an object and pointer to a member and stores
			// them in two private variables
			TSpecificFunctor(TClass* _pt2Object, void(TClass::*_fpt)())
			{
				pt2Object = _pt2Object;  fpt = _fpt;
			};

			// override operator "()"
			virtual void operator()()
			{
				(*pt2Object.*fpt)();
			};              // execute member function

							// override function "Call"
			virtual void Call()
			{
				(*pt2Object.*fpt)();
			};             // execute member function
		};

		class IGUI2D
		{
		public:

			IGUI2D()
			{
				m_position = DirectX::XMFLOAT2(0.0f, 0.0f);
				m_scale = DirectX::XMFLOAT2(1.0f, 1.0f);

				m_rectangle.left = 0;
				m_rectangle.right = 50;
				m_rectangle.bottom = 50;
				m_rectangle.top = 0;

				m_rotationAngle = 0.0f;

				m_defaultColor = DirectX::XMFLOAT4(0.5f, 0.8f, 0.5f, 0.7f);
				m_onHoverColor = DirectX::XMFLOAT4(0.6f, 0.85f, 0.7f, 0.7f);
				m_onClickColor = DirectX::XMFLOAT4(0.6f, 0.9f, 0.7f, 0.7f);
				m_currentColor = m_defaultColor;

				m_selected = false;
				m_hovered = false;
				m_transformMatrix = D2D1::Matrix3x2F::Identity();

				m_recreate = false;
				m_active = true;
				m_refCount = 1;
			}

			virtual void Initialize() = 0;

			virtual void Update(float dt) = 0;

			virtual void Render() = 0;

			virtual void Resize() = 0;

			virtual void OnKeyPress(UINT32 keyCode) PURE;
			virtual void OnKeyCharacter(UINT32 keyCode) PURE;
			virtual void OnMousePress(UINT message, float x, float y) PURE;
			virtual void OnMouseRelease(UINT message, float x, float y) PURE;
			virtual void OnMouseMove(float x, float y) PURE;
			virtual void OnMouseScroll(float xScroll, float yScroll) PURE;
			virtual void Destroy() PURE;

			int AddRef()
			{
				return ++m_refCount;
			}

			int Release()
			{
				if (--m_refCount == 0)
				{
					delete this;
					return 0;
				}
				return m_refCount;
			}



		protected:
			virtual void DefaultOnHover() PURE;
			virtual void DefaultOnClick() PURE;

		public:

			void SetPosition(float x, float y)								{ m_position.x = x; m_position.y = y;  m_recreate = true;}
			void SetPosition(float &x, float &y)							{ m_position.x = x; m_position.y = y;  m_recreate = true;}
			void SetPosition(DirectX::XMFLOAT2 pos)							{ this->m_position = pos; m_recreate = true; }
			void SetScale(float &x, float &y)								{ m_scale.x = x; m_scale.y = y; m_recreate = true; }
			void SetScale(float x, float y)									{ m_scale.x = x; m_scale.y = y;  m_recreate = true;}
			void SetScale(DirectX::XMFLOAT2  scale)							{ this->m_scale = scale;  m_recreate = true;}
			void SetSelected(bool b)										{ this->m_selected = b; }
			void SetHovered(bool b)											{ this->m_hovered = b; }
			void SetRotation(float rot)										{ this->m_rotationAngle = rot;  m_recreate = true;}
			void SetOnHoverColor(DirectX::XMFLOAT4 color)					{ m_onHoverColor = color; }
			void SetOnHoverColor(float &r, float &g, float &b, float &a)	{ m_onHoverColor = DirectX::XMFLOAT4(r, g, b, a);m_recreate = true;}
			void SetOnHoverColor(float r,float g,float b,float a)			{ m_onHoverColor = DirectX::XMFLOAT4(r,g,b,a);m_recreate = true;}
			void SetOnClickColor(DirectX::XMFLOAT4 color)					{ m_onClickColor = color;m_recreate = true;}
			void SetOnClickColor(float r, float g, float b, float a)		{ m_onClickColor = DirectX::XMFLOAT4(r, g, b, a);m_recreate = true;}
			void SetOnClickColor(float &r, float &g, float &b, float &a)	{ m_onClickColor = DirectX::XMFLOAT4(r, g, b, a);m_recreate = true;}
			void SetDefaultColor(DirectX::XMFLOAT4 color)					{ m_defaultColor = color; m_recreate = true; }
			void SetDefaultColor(float r, float g, float b, float a)		{ m_defaultColor = DirectX::XMFLOAT4(r, g, b, a); m_recreate = true; }
			void SetDefaultColor(float &r, float &g, float &b, float &a)	{ m_defaultColor = DirectX::XMFLOAT4(r, g, b, a); m_recreate = true; }
			void SetTransformationMatrix(D2D1::Matrix3x2F mat)				{ this->m_transformMatrix = mat; m_recreate = true; }
			virtual void SetActive(bool b)											PURE;
			virtual void SetActive(bool &b)											PURE;


			void SetRectangle(float left, float top, float right, float bottom)
			{
				m_rectangle.left = left;
				m_rectangle.right = right;
				m_rectangle.bottom = bottom;
				m_rectangle.top = top;

				m_recreate = true;
			}

			void SetRectangle(float &left, float &top, float &right, float &bottom)
			{
				m_rectangle.left = left;
				m_rectangle.right = right;
				m_rectangle.bottom = bottom;
				m_rectangle.top = top;

				m_recreate = true;
			}

			void SetRectangle(D2D1_RECT_F rect)
			{
				m_rectangle = rect;
				m_recreate = true;
			}

			float	 GetPositionX() { return m_position.x; }
			float	 GetPositionY() { return m_position.y; }
			DirectX::XMFLOAT2	 GetPosition()				 { return m_position; }
			float				 GetRotation() const		 { return m_rotationAngle; }
			DirectX::XMFLOAT2	 GetScale() const			 { return m_scale; }
			D2D1_RECT_F			 GetRect() const			 { return m_rectangle; }
			bool				 GetSelected() const		 { return m_selected; }
			bool				 GetHovered() const			 { return m_hovered; }
			D2D1::Matrix3x2F GetTransformationMatrix() const { return m_transformMatrix; }
			bool GetActive() const							 { return m_active; }
			
		protected:
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_brush;
			DirectX::XMFLOAT2						m_position;
			DirectX::XMFLOAT2						m_scale;
			float									m_rotationAngle;
			D2D1_RECT_F								m_rectangle;
			bool									m_selected;
			bool									m_hovered;
			DirectX::XMFLOAT4						m_onHoverColor;
			DirectX::XMFLOAT4						m_onClickColor;
			DirectX::XMFLOAT4						m_defaultColor;
			DirectX::XMFLOAT4						m_currentColor;
			D2D1::Matrix3x2F						m_transformMatrix;
			bool									m_recreate;
			bool									m_active;
			asILockableSharedBool *				m_weakRefFlag;
			unsigned int						m_refCount;
		};
	}
}