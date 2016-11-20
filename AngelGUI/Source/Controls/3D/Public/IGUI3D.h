#pragma once
#include<d3d11_1.h>
#include<wincodec.h>
#include<wrl\client.h>
#include<dwrite_1.h>
#include<dwrite.h>
#include<d2d1_1.h>
#include<DirectXMath.h>
#include<DirectXCollision.h>
#include"Shader.h"
#include"../../../Core/GlobalTools.h"
#include"../../3D/Public/CBuffer.h"
#include<angelscript.h>

namespace AngelGUISystem
{
	class AngelGUI;
	namespace GUI3D
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

		class IGUI3D
		{

		protected:

			struct World
			{
				DirectX::XMFLOAT4X4 WVP;
				DirectX::XMFLOAT4X4 View;
				DirectX::XMFLOAT4X4 Projection;
				DirectX::XMFLOAT3 eye;
				float padd;
			};
			struct ColorBuffer
			{
				DirectX::XMFLOAT4 color;
			};

			struct VertexType
			{
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT2 uv;
			};

		public:

			IGUI3D();

			virtual void Initialize() = 0;

			virtual void Update(float dt, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) = 0;

			virtual void Render();

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
			bool TestIntersects(
				DirectX::XMFLOAT2 * HitPoint2DSpace,
				DirectX::XMFLOAT3 * HitPointLocalSpace, 
				float *dist,
				float mouseX, float mouseY);

			bool TestIntersects(
				DirectX::CXMMATRIX invWorld,
				float *dist,
				float mouseX, float mouseY);

		public:

			void SetPosition(float x, float y, float z) { m_position.x = x; m_position.y = y; m_position.z = z;  m_recreate = true; }
			void SetPosition(float &x, float &y,float &z) { m_position.x = x; m_position.y = y;m_position.z = z;  m_recreate = true; }
			void SetPosition(DirectX::XMFLOAT3 pos) { this->m_position = pos; }
			void SetScale(float &x, float &y, float &z) { m_scale.x = x; m_scale.y = y; m_scale.z = z; m_recreate = true; }
			void SetScale(float x, float y, float z) { m_scale.x = x; m_scale.y = y; m_scale.z = z;  m_recreate = true; }
			void SetRotation(float &x, float &y, float &z) { m_rotation.x = x; m_rotation.y = y; m_rotation.z = z; m_recreate = true; }
			void SetRotation(float x, float y, float z)		{ m_rotation.x = x; m_rotation.y = y; m_rotation.z = z;  m_recreate = true; }
			void SetRotation(DirectX::XMFLOAT3  rot)		{ m_rotation = rot;  m_recreate = true; }
			void SetScale(DirectX::XMFLOAT3  scale) { this->m_scale = scale;  m_recreate = true; }
			void SetSelected(bool b) { this->m_selected = b; }
			void SetHovered(bool b) { this->m_hovered = b; }
			void SetOnHoverColor(DirectX::XMFLOAT4 color) { m_onHoverColor = color; }
			void SetOnHoverColor(float &r, float &g, float &b, float &a) { m_onHoverColor = DirectX::XMFLOAT4(r, g, b, a);m_recreate = true; }
			void SetOnHoverColor(float r, float g, float b, float a) { m_onHoverColor = DirectX::XMFLOAT4(r, g, b, a);m_recreate = true; }
			void SetOnClickColor(DirectX::XMFLOAT4 color) { m_onClickColor = color;m_recreate = true; }
			void SetOnClickColor(float r, float g, float b, float a) { m_onClickColor = DirectX::XMFLOAT4(r, g, b, a);m_recreate = true; }
			void SetOnClickColor(float &r, float &g, float &b, float &a) { m_onClickColor = DirectX::XMFLOAT4(r, g, b, a);m_recreate = true; }
			void SetDefaultColor(DirectX::XMFLOAT4 color) { m_defaultColor = color; m_recreate = true; }
			void SetDefaultColor(float r, float g, float b, float a) { m_defaultColor = DirectX::XMFLOAT4(r, g, b, a); m_recreate = true; }
			void SetDefaultColor(float &r, float &g, float &b, float &a) { m_defaultColor = DirectX::XMFLOAT4(r, g, b, a); m_recreate = true; }
			void SetCurrentColor(float &r, float &g, float &b, float &a) { m_currentColor = DirectX::XMFLOAT4(r, g, b, a); m_recreate = true; }
			void SetCurrentColor(float r, float g, float b, float a) { m_currentColor = DirectX::XMFLOAT4(r, g, b, a); m_recreate = true; }
			void SetCurrentColor(DirectX::XMFLOAT4 color) { m_currentColor = color; m_recreate = true; }
			virtual void SetActive(bool b)											PURE;
			virtual void SetActive(bool &b)											PURE;

			void SetBillboard(bool b) { this->m_billboard = b; }
			void SetBillboard(bool &b) { if (b == true) this->m_billboard = true; else this->m_billboard = false; }


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

			DirectX::XMFLOAT3	 GetPosition() const	{ return m_position; }
			DirectX::XMFLOAT3	 GetRotation() const	{ return m_rotation; }
			DirectX::XMFLOAT3	 GetScale() const		{ return m_scale; }
			D2D1_RECT_F			 GetRect() const		{ return m_rectangle; }
			bool				 GetSelected() const	{ return m_selected; }
			bool				 GetHovered() const		{ return m_hovered; }
			bool GetActive() const						{ return m_active; }
			bool GetBillboard() const					{ return m_billboard; }

		protected:
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_brush;
			bool									m_selected;
			bool									m_hovered;
			DirectX::XMFLOAT4						m_onHoverColor;
			DirectX::XMFLOAT4						m_onClickColor;
			DirectX::XMFLOAT4						m_defaultColor;
			DirectX::XMFLOAT4						m_currentColor;
			bool									m_recreate;
			bool									m_active;
			AngelGUISystem::GUI3D::GUI3DRendering::Shader*								m_shader;
			DirectX::XMFLOAT3									m_position;
			DirectX::XMFLOAT3									m_rotation;
			DirectX::XMFLOAT3									m_scale;
			UINT												m_indicesSize;
			UINT												m_stride;
			UINT												m_offset;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				m_indexBuffer;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceView;
			GUI3DRendering::CBuffer<World>						m_cbWorld;
			GUI3DRendering::CBuffer<ColorBuffer>				m_cbColor;
			DirectX::BoundingOrientedBox						m_boundingBox;
			D2D1_RECT_F											m_rectangle;
			Microsoft::WRL::ComPtr<ID2D1RenderTarget>			m_renderTarget2D;
			DirectX::XMFLOAT4X4									m_worldMatrix;
			DirectX::XMFLOAT4X4									m_invWorldMatrix;
			DirectX::XMFLOAT4X4									m_view;
			DirectX::XMFLOAT4X4									m_projection;
			asILockableSharedBool *								m_weakRefFlag;
			unsigned int										m_refCount;
			bool												m_billboard;
			bool												m_renderFromResource;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_resourceSRV;
		};
	}
}