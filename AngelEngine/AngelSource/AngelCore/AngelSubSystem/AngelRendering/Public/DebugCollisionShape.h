#pragma once
#include<DirectXMath.h>
#include<wrl.h>
#include"Shader.h"
#include"CBuffer.h"

namespace AngelCore
{
	namespace AngelRendering
	{
		
		class DebugCollisionShape
		{
			struct cbWVP
			{
				DirectX::XMFLOAT4X4 WVP;
			};
			struct cbColor
			{
				DirectX::XMFLOAT4 color;
			};
		public:
			void Create(DirectX::XMFLOAT3 * points);
			void Create();
			void Render();

			void Update(DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection);

		public:
			void SetBoundingBox(DirectX::BoundingOrientedBox b) { this->m_boundingBox = b; }


		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
			Shader * m_shader;
			CBuffer<cbColor> m_cbColor;
			CBuffer<cbWVP> m_cbWVP;
			DirectX::BoundingOrientedBox m_boundingBox;
		};
	}
}

