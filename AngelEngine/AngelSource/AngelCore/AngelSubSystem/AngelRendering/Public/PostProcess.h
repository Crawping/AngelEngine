// ***********************************************************************
// FileName         : RenderTexture.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Handle PostProcess Effects  </summary>
// ***********************************************************************

#pragma once

#include<wrl\client.h>
#include<DirectXMath.h>
#include<d3d11_1.h>
#include<vector>
#include"CBuffer.h"

#define NUM_HELPER_TEXTURES 5

using Microsoft::WRL::ComPtr;

namespace AngelCore
{
	namespace AngelRendering
	{
		class Shader;
		class RWTexture;
		class PostProcess
		{
			struct downSamplingOffsets
			{
				DirectX::XMFLOAT4      tcDSOffsets[16];
			};
			struct kernelTexel
			{
				DirectX::XMFLOAT4 TexelKernel[13];
			};
			struct ScreenSize
			{
				DirectX::XMFLOAT2 size;
				float frameRate;
				float padd;
			};
		private:
			PostProcess() : 
				m_shader(nullptr) , 
				m_shaderBlurHorizontal(nullptr) ,
				m_shaderBlurVertical(nullptr)
			{ };
			PostProcess(const PostProcess & that) { };
			~PostProcess();

		public:
			enum  POST_PROCESS_TECHNIQUE
			{
				BLUR,
				DEPTH_OF_FIELD,
				MOTION_BLUR,
				DOWN_SAMPLE,
				BLOOM
			};
		public:

			static PostProcess * GetInstance();
			static void DestroyInstance();
			void CreateVRGUITexture(UINT width, UINT height);
		
			void Initialize(ID3D11Device1  * device);
			void RenderFinalImage();
			void RenderFinalImageCombined();


			void ProcessTechniques();
			//Push techniques for later dispatch
			void PushTechnique(POST_PROCESS_TECHNIQUE PPT);
			void PopTechnique(POST_PROCESS_TECHNIQUE PPT);

			void RenderCurrentFrameToTexture(ID3D11DeviceContext1 *deviceContext);

			ComPtr<ID3D11ShaderResourceView> GetDepthStencilViewSRV() { return m_srvDsv; }
			ID3D11Texture2D				* GetGUITexture() const { return m_guiTexture.Get(); }
		private:

			ID3D11Texture2D				*			m_renderTargetViewTexture[3];
			ID3D11RenderTargetView		*			m_renderTargetView[3];
			ID3D11ShaderResourceView	*			m_srvRtv[3];
			ComPtr<ID3D11ShaderResourceView>		m_srvDsv;
			ComPtr<ID3D11DepthStencilView>			m_dsv;
			UINT									m_indicesSize;
			UINT									m_stride;
			UINT									m_offset;
			AngelRendering::Shader*					m_shader;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_indexBuffer;
			static PostProcess *					m_instance;

			CBuffer<downSamplingOffsets>			m_cbDownSampling;
			CBuffer<kernelTexel>					m_cbkernelTexel;
			CBuffer<ScreenSize>						m_cbScreenSize;

			ComPtr<ID3D11Texture2D>					m_helperTexture[NUM_HELPER_TEXTURES];
			ComPtr<ID3D11ShaderResourceView>		m_helperSrv[NUM_HELPER_TEXTURES];

			RWTexture *								m_rwTexture;
			
			//Shader for each technique
			AngelRendering::Shader*					m_shaderBlurHorizontal;
			AngelRendering::Shader*					m_shaderBlurVertical;
			AngelRendering::Shader*					m_shaderDownSample;
			AngelRendering::Shader*					m_shaderDOF;
			AngelRendering::Shader*					m_shaderMotionBlur;
			AngelRendering::Shader*					m_shaderBrightPass;
			AngelRendering::Shader*					m_shaderBloomVertical;
			AngelRendering::Shader*					m_shaderBloomHorizontal;
			AngelRendering::Shader*					m_shaderCombine;

			//List of all post process techniques to dispatch
			std::vector<POST_PROCESS_TECHNIQUE>		m_techniques;

			//GUI Rendering Inside VR
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_guiTexture;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_guiSRV;

			bool		m_enableMotionBlur;
			bool		m_enableDepthOfField;
			bool		m_enableBloom;

		private:
			void SetRenderTarget(ID3D11DeviceContext1 * deviceContext);

			void ClearRenderTarget(ID3D11DeviceContext1 * deviceContext,
				float color[4]);

			void RenderQuad();
			void RenderQuadCombined();
			void InitalizeQuad();
			void DispatchTechniques();
			void DownSample();
		};
	}
}


