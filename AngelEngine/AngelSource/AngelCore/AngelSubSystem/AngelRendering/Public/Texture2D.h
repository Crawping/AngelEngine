// ***********************************************************************
// FileName         : Texture2D.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Loading and using 2D textures  </summary>
// ***********************************************************************

#pragma once

#include "WICTextureLoader.h"
#include"DDSTextureLoader.h"
#include"../../../AngelSubSystem/Public/FileSystemManager.h"
#include"../../../AngelSubSystem/Public/SubSystemsResource.h"
#include"../Public/DirectXTex.h"
#include<d3d11_1.h>

#define __TEXTURE2D

class Texture2D
{
public:
	Texture2D();
	~Texture2D();
	void LoadTexture(const wchar_t  * name,bool mipmap);
	static void CreateSampler(
		D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE AddressU,
		D3D11_TEXTURE_ADDRESS_MODE AddressV,
		D3D11_TEXTURE_ADDRESS_MODE AddressW,
		D3D11_COMPARISON_FUNC func);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV()		{ return this->m_textureView; }
	Microsoft::WRL::ComPtr<ID3D11Resource> GetResource()			{ return this->m_textureResource; }

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>							m_textureView;
	Microsoft::WRL::ComPtr<ID3D11Resource>										m_textureResource;
};