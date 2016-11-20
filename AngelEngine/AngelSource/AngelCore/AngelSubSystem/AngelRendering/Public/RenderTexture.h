// ***********************************************************************
// FileName         : RenderTexture.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Render scene to texture  </summary>
// ***********************************************************************

#pragma once

#include<wrl\client.h>
#include<DirectXMath.h>
#include<d3d11_1.h>

using Microsoft::WRL::ComPtr;

namespace AngelCore
{
	namespace AngelRendering
	{
		class RenderTexture
		{

		public:

			RenderTexture();

			void Initialize(ID3D11Device1  * device, int height, int width);

			void RenderCurrentFrameToTexture(ID3D11DeviceContext1 *deviceContext, ID3D11DepthStencilView * depthStencil, ID3D11BlendState *blendState);

			ID3D11ShaderResourceView ** GetShaderResourceViewRTV()
			{
				return this->m_shaderResourceView.GetAddressOf();
			}
			ID3D11ShaderResourceView ** GetShaderResourceViewDSV()
			{
				return this->m_srvDsv.GetAddressOf();
			}

		private:

			ComPtr<ID3D11Texture2D>												m_renderTargetViewTexture;
			ComPtr<ID3D11RenderTargetView>										m_renderTargetView;
			ComPtr<ID3D11ShaderResourceView>									m_shaderResourceView;
			ComPtr<ID3D11ShaderResourceView>									m_srvDsv;
			ComPtr<ID3D11DepthStencilView>										m_dsv;


		private:
			void SetRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, ID3D11BlendState *blendState);
			void ClearRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, float color[4]);
		};
	}
}


