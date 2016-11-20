// ***********************************************************************
// FileName         : SpriteRenderer.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Component for rendering a 2D sprite  </summary>
// ***********************************************************************

#pragma once

#include"../../AngelComponent/Public/IComponent.h"
#include"../../../Public/TimeManager.h"
#include"../../../AngelRendering/Public/Shader.h"

#include<wrl\client.h>


#ifndef __CBUFFER
#define __CBUFFER
#include"../../../AngelRendering/Public/CBuffer.h"
#endif

#include<string>

namespace AngelCore
{
	namespace AngelWorldSystem
	{
		/// <summary>
		/// Class SpriteRenderer,we use this class for rendering a 2d sprite in game.
		/// </summary>
		class SpriteRenderer : public AngelCore::AngelWorldSystem::IComponent
		{

		public:
			SpriteRenderer();
		public:

			virtual void Initialize(AngelCore::AngelWorldSystem::Actor* _owner, ...) override;

			virtual void Update(const AngelCore::AngelSubSystem::TimeManager &timer) override;

			virtual void Render() override;

			//Render This sprite with give SRV
			void RenderSRV(ID3D11ShaderResourceView * srv);

		private:
			struct cbPerObjectBuffer
			{
				DirectX::XMFLOAT4X4 WVP;
			};


			Microsoft::WRL::ComPtr<ID3D11Buffer>								m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>								m_indexBuffer;
			UINT																m_stride;
			UINT																m_offset;
			AngelCore::AngelRendering::Shader *									m_shader;
			CBuffer<cbPerObjectBuffer>											m_cbPerObjectBuffer;

		private:
		};
	}
}