// ***********************************************************************
// FileName         : RWTexture.cpp
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Loading and using RW(Read/Write) textures in compute shaders  </summary>
// ***********************************************************************

#include "../Public/RWTexture.h"
#include"../../../AngelTools/AngelGlobalTools.h"


AngelCore::AngelRendering::RWTexture::RWTexture() : m_SRV(nullptr) , m_UAV(nullptr)
{

}

AngelCore::AngelRendering::RWTexture::~RWTexture()
{
	COM_RELEASE(m_UAV);
	COM_RELEASE(m_SRV);
	COM_RELEASE(m_resource);
}

void AngelCore::AngelRendering::RWTexture::Load()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC td;

	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	td.CPUAccessFlags = 0;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.Height = AngelSubSystemResources::WindowProperties::GetHeight();
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.Width = AngelSubSystemResources::WindowProperties::GetWidth();
	
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateTexture2D(&td, nullptr, 
		m_resource.GetAddressOf()));
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;


	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateShaderResourceView
		(m_resource.Get(), &srvd, this->m_SRV.GetAddressOf()));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	uavd.Format = td.Format;
	uavd.Texture2D.MipSlice = 0;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateUnorderedAccessView(
		m_resource.Get(), &uavd, this->m_UAV.GetAddressOf()));
}