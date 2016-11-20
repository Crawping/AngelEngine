
#pragma once

#include<wrl\client.h>
#include<DirectXMath.h>
#include<d3d11_1.h>
#include"Shader.h"

using Microsoft::WRL::ComPtr;

namespace AngelCore
{
	namespace AngelRendering
	{
		class DefferedRenderTexture
		{
			static const int BUFFER_COUNT = 3;

			struct cbuffercbGBufferUnpack
			{
				DirectX::XMFLOAT4 PerspectiveValues;
				DirectX::XMFLOAT4X4 ViewInv;
			};

		public:

			DefferedRenderTexture();

			void Initialize(ID3D11Device1  * device, int height, int width, float screenDepth, float screenNear);

			void RenderCurrentFrameToTexture(ID3D11DeviceContext1 *deviceContext, ID3D11DepthStencilView * depthStencil, ID3D11BlendState *blendState);

			ID3D11ShaderResourceView ** GetShaderResourceView(int i)
			{
				return &this->m_shaderResourceView[i];
			}

			void Render();
			void RenderLights();

		private:
			void InitializeQuad();

		private:

			ID3D11Texture2D				*m_renderTargetViewTexture[BUFFER_COUNT];
			ID3D11RenderTargetView		*m_renderTargetView[BUFFER_COUNT];
			ID3D11ShaderResourceView	*m_shaderResourceView[BUFFER_COUNT];
			ID3D11ShaderResourceView	*m_depthTexture;
			ID3D11Texture2D				*m_depthStencilBuffer;
			ID3D11DepthStencilView		*m_depthStencilView;
			D3D11_VIEWPORT				 m_viewport;
			UINT indicesSize;
			UINT stride;
			UINT offset;
			Shader* shader;
			Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
			CBuffer<cbuffercbGBufferUnpack>		m_cbGBuffer;

		private:
			void SetRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, ID3D11BlendState *blendState);
			void ClearRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, float color[4]);
		};
	}
}