
// ***********************************************************************
// FileName         : Texture2D.cpp
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Loading and using 2D textures  </summary>
// ***********************************************************************


#include "../Public/Texture2D.h"
#include"../../Public/SubSystemsResource.h"

Texture2D::Texture2D() : m_textureView(nullptr), m_textureResource(nullptr)
{

}

Texture2D::~Texture2D()
{
	COM_RELEASE(m_textureResource);
	COM_RELEASE(m_textureView);
}

void Texture2D::LoadTexture(const wchar_t  * name, bool mipmap)
{
	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	path += L"\\AngelData\\Texture\\";
	path += name;

	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(path.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);
	std::unique_ptr<uint8_t[]> data;
	size_t dataSize;
	int fileState;
	
	if (_wcsicmp(ext, L".dds") == 0)
	{
		if (mipmap)
		{
			HR(DirectX::CreateDDSTextureFromFileEx(
				AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get(),
				AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext.Get()
				,
				path.c_str(), D3D10_FLOAT32_MAX, D3D11_USAGE_DEFAULT,
				D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0,
				D3D11_RESOURCE_MISC_GENERATE_MIPS, false,
				m_textureResource.GetAddressOf(), m_textureView.GetAddressOf()));
		}
		else
			HR(DirectX::CreateDDSTextureFromFile(
				AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get()
				, path.c_str(),m_textureResource.GetAddressOf(), m_textureView.GetAddressOf()));
	}
	else if (_wcsicmp(ext, L".tga") == 0)
	{
		HR(AngelCore::AngelSubSystem::FileSystemManager::ReadBinaryFile(path, data, &dataSize, &fileState));
		DirectX::TexMetadata data;
		DirectX::ScratchImage img;
		HR(DirectX::GetMetadataFromTGAFile(path.c_str(), data));
		HR(DirectX::LoadFromTGAFile(path.c_str(), &data, img));
		HR(DirectX::CreateShaderResourceView(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get()
			, img.GetImages(), img.GetImageCount()
			, data, m_textureView.GetAddressOf()));
	}
	else if (lstrcmpW(name,L"")!=0)
	{
		HR(AngelCore::AngelSubSystem::FileSystemManager::ReadBinaryFile(path, data, &dataSize, &fileState));
		HR(DirectX::CreateWICTextureFromMemory(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get()
			, data.get(), dataSize, m_textureResource.GetAddressOf(), m_textureView.GetAddressOf()));
	}
}

void Texture2D::CreateSampler(
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE AddressU,
	D3D11_TEXTURE_ADDRESS_MODE AddressV,
	D3D11_TEXTURE_ADDRESS_MODE AddressW,
	D3D11_COMPARISON_FUNC func)
{
	using namespace Microsoft::WRL;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = filter;

	samplerDesc.MaxAnisotropy = 16;

	samplerDesc.AddressU = AddressU;
	samplerDesc.AddressV = AddressV;
	samplerDesc.AddressW = AddressW;

	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerDesc.ComparisonFunc = func;

	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;

	ComPtr<ID3D11SamplerState> sampler;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateSamplerState(&samplerDesc, &sampler));
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.push_back(sampler);
}