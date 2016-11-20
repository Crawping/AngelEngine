// ***********************************************************************
// FileName         : RenderTexture.cpp
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Render scene to texture  </summary>
// ***********************************************************************

#include "../Public/RenderTexture.h"
#include"../../AngelLog/Public/AngelLog.h"
#include"../../../AngelTools/AngelGlobalTools.h"
#include"../../../AngelSubSystem/Public/SubSystemsResource.h"


AngelCore::AngelRendering::RenderTexture::RenderTexture()
{
}

void AngelCore::AngelRendering::RenderTexture::Initialize(ID3D11Device1 * device,int height, int width)
{
	

	HRESULT hr;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.Height = height;
	td.MipLevels = 6;
	td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.Width = width;
	HR(device->CreateTexture2D(&td, nullptr, &this->m_renderTargetViewTexture));
	
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	rtvd.Format = td.Format;
	rtvd.Texture2D.MipSlice = 0;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	HR(device->CreateRenderTargetView(this->m_renderTargetViewTexture.Get(), 
		&rtvd, this->m_renderTargetView.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 5;
	srvd.Texture2D.MostDetailedMip = 0;

	HR(device->CreateShaderResourceView(this->m_renderTargetViewTexture.Get(),
		&srvd, this->m_shaderResourceView.GetAddressOf()));

	D3D11_TEXTURE2D_DESC desc;

	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;

	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.Height = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();;

	ID3D11Texture2D * depthMap;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateTexture2D(&desc, nullptr, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvdesc;
	dsvdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvdesc.Texture2D.MipSlice = 0;
	dsvdesc.Flags = 0;
	dsvdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateDepthStencilView(depthMap, &dsvdesc, &this->m_dsv));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvdesc.Texture2D.MipLevels = desc.MipLevels;
	srvdesc.Texture2D.MostDetailedMip = 0;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateShaderResourceView(depthMap, &srvdesc, &this->m_srvDsv));
}

void AngelCore::AngelRendering::RenderTexture::SetRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, ID3D11BlendState *blendState)
{
	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };
	deviceContext->OMSetRenderTargets(1, this->m_renderTargetView.GetAddressOf(), m_dsv.Get());
	//deviceContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
}

void AngelCore::AngelRendering::RenderTexture::ClearRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, float color[4])
{
	deviceContext->ClearRenderTargetView(this->m_renderTargetView.Get(), color);
	deviceContext->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void AngelCore::AngelRendering::RenderTexture::RenderCurrentFrameToTexture(ID3D11DeviceContext1 *deviceContext, ID3D11DepthStencilView * depthStencil,ID3D11BlendState *blendState)
{
	SetRenderTarget(deviceContext, depthStencil,blendState);
	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ClearRenderTarget(deviceContext, depthStencil, color);
	
}