// ***********************************************************************
// FileName         : Shader.cpp
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Creating and using all types of shaders  </summary>
// ***********************************************************************


#include "../Public/Shader.h"
#include"../../../Core/GlobalTools.h"
#include"../../../Core/AngelGUI.h"



AngelGUISystem::GUI3D::GUI3DRendering::Shader::Shader()
{
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::Load()
{
	std::unique_ptr<uint8_t[]> data;
	size_t dataSize;
	int fileState;
	TCHAR Buffer[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, Buffer);
	std::wstring path = Buffer;
	path += L"\\AngelGUIData\\";


	std::wstring pathVertexShader = path + L"VertexBillboardText.cso";
	std::wstring pathPixelShader = path + L"PixelBillboardText.cso";

	HR(ReadBinaryFile
		(pathVertexShader, data, &dataSize, &fileState));
	CreateVertexShader(data.get(), dataSize);

	HR(ReadBinaryFile
		(pathPixelShader, data, &dataSize, &fileState));
	CreatePixelShader(data.get(), dataSize);
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::CreateVertexShader
(const void* bytes, SIZE_T Length)
{
	HR(AngelGUI::GetDevice3D()->
		CreateVertexShader(bytes, Length, nullptr, &this->vShader));
	(CreateInputLayout(bytes, Length));
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::CreatePixelShader(const void* bytes, SIZE_T Length)
{
	HR(AngelGUI::GetDevice3D()->
		CreatePixelShader(bytes, Length, nullptr, &this->pShader));
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV[2], ShaderType ST)
{
	switch (ST)
	{
	case ShaderType::VertexShader:
		AngelGUI::GetDeviceContext3D()->VSSetShaderResources(startSlot, numBuffer, SRV);
		break;
	case ShaderType::PixelShader:
		if(SRV!=nullptr)
			AngelGUI::GetDeviceContext3D()->PSSetShaderResources(startSlot, numBuffer, SRV);
		break;
	default:
		break;
	}
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV, ShaderType ST)
{
	switch (ST)
	{
	case ShaderType::VertexShader:
		AngelGUI::GetDeviceContext3D()->VSSetShaderResources(startSlot, numBuffer, &SRV);
		break;
	case ShaderType::PixelShader:
		AngelGUI::GetDeviceContext3D()->PSSetShaderResources(startSlot, numBuffer, &SRV);
		break;
	default:
		break;
	}
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::UnBindSRV(UINT startSlot, UINT numBuffer, ShaderType ST)
{
	switch (numBuffer)
	{
	case 1 :
		ID3D11ShaderResourceView *srv1[1];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv1[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv1, ST);
		break;
	case 2:
		ID3D11ShaderResourceView *srv2[2];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv2[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv2, ST);
		break;
	case 3:
		ID3D11ShaderResourceView *srv3[3];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv3[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv3, ST);
		break;
	case 4:
		ID3D11ShaderResourceView *srv4[4];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv4[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv4, ST);
		break;
	case 5:
		ID3D11ShaderResourceView *srv5[5];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv5[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv5, ST);
		break;
	case 6:
		ID3D11ShaderResourceView *srv6[6];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv6[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv6, ST);
		break;
	case 7:
		ID3D11ShaderResourceView *srv7[7];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv7[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv7, ST);
		break;
	case 8:
		ID3D11ShaderResourceView *srv8[8];
		for (unsigned int i = 0;i < numBuffer;i++)
			srv8[i] = nullptr;
		BindSRV(startSlot, numBuffer, srv8, ST);
		break;
	default:
		break;
	}
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::UnBindSampler(UINT startSlot, UINT numBuffer)
{
	ID3D11SamplerState *uav = nullptr;
	SetSampler(startSlot, numBuffer, &uav);
}


void AngelGUISystem::GUI3D::GUI3DRendering::Shader::SetConstantBuffer(UINT startSlot, UINT numBuffer, _In_ ID3D11Buffer* CBuffer)
{
	if (vShader) AngelGUI::GetDeviceContext3D()->VSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
	if (pShader)AngelGUI::GetDeviceContext3D()->PSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::SetInputLayout(ID3D11InputLayout* value)
{
	this->inputLayout = value;
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::SetSampler(UINT startSlot, UINT numView, ID3D11SamplerState ** sampler)
{
	AngelGUI::GetDeviceContext3D()->PSSetSamplers(startSlot, numView, sampler);
	AngelGUI::GetDeviceContext3D()->GSSetSamplers(startSlot, numView, sampler);
	
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::Apply()
{
	
	AngelGUI::GetDeviceContext3D()->IASetInputLayout(this->inputLayout.Get());
	AngelGUI::GetDeviceContext3D()->VSSetShader(this->vShader.Get(), nullptr, 0);
	AngelGUI::GetDeviceContext3D()->PSSetShader(this->pShader.Get(), nullptr, 0);
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::Disable()
{
	AngelGUI::GetDeviceContext3D()->VSSetShader(nullptr, nullptr, 0);
	AngelGUI::GetDeviceContext3D()->PSSetShader(nullptr, nullptr, 0);
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* vertexDesc, const UINT vertexDescLength,
	const void* shaderData, const SIZE_T shaderDataLength)
{
	HR(AngelGUI::GetDevice3D()->CreateInputLayout(
		vertexDesc,
		vertexDescLength,
		shaderData,
		shaderDataLength,
		this->inputLayout.GetAddressOf()));
};

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::CreateInputLayout(
	const void* shaderData, SIZE_T shaderDataLength)
{
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
}

void AngelGUISystem::GUI3D::GUI3DRendering::Shader::Release()
{

}