// ***********************************************************************
// FileName         : RWTexture.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Loading and using RW(Read/Write) textures in compute shaders  </summary>
// ***********************************************************************

#pragma once
#include<wrl\client.h>
#include<d3d11_1.h>
#include<DirectXMath.h>
#include"../../../AngelCore/AngelSubSystem/Public/SubSystemsResource.h"

namespace AngelCore
{
	namespace AngelRendering
	{
		class RWTexture
		{
		public:

			void Load();

			RWTexture();
			~RWTexture();

#pragma region Properties
			ID3D11ShaderResourceView ** GetShaderResourceView()
			{
				return this->m_SRV.GetAddressOf();
			}

			ID3D11UnorderedAccessView ** GetUnorderedAccessView()
			{
				return this->m_UAV.GetAddressOf();
			}
			Microsoft::WRL::ComPtr<ID3D11Texture2D> GetResource()
			{
				return m_resource;
			}
#pragma endregion

		private:

			Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>									m_UAV;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>									m_SRV;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>												m_resource;
		};

	}
}


