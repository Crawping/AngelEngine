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
#include "../Public/VertexTypes.h"



AngelCore::AngelRendering::Shader::Shader()
{
}

void AngelCore::AngelRendering::Shader::CreateVertexShader(const void* bytes, SIZE_T Length, VertexTypes VDeclaration)
{
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		CreateVertexShader(bytes, Length, nullptr, &this->vShader));
	HR(CreateInputLayout(VDeclaration, bytes, Length));
}

void AngelCore::AngelRendering::Shader::CreatePixelShader(const void* bytes, SIZE_T Length)
{
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		CreatePixelShader(bytes, Length, nullptr, &this->pShader));
}

void AngelCore::AngelRendering::Shader::CreateComputeShader(const void * bytes, SIZE_T length)
{
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		CreateComputeShader(bytes, length, nullptr, cShader.GetAddressOf()));
}

void AngelCore::AngelRendering::Shader::CreateGeometryShaderWithSO(const void * bytes, SIZE_T length)
{

	D3D11_SO_DECLARATION_ENTRY desc[4];
	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].ComponentCount = 3;
	desc[0].StartComponent = 0;
	desc[0].Stream = 0;
	desc[0].OutputSlot = 0;

	desc[1].SemanticName = "VELOCITY";
	desc[1].SemanticIndex = 0;
	desc[1].ComponentCount = 3;
	desc[1].StartComponent = 0;
	desc[1].Stream = 0;
	desc[1].OutputSlot = 0;
		 
	desc[2].SemanticName = "LIFE";
	desc[2].SemanticIndex = 0;
	desc[2].ComponentCount =1;
	desc[2].StartComponent = 0;
	desc[2].Stream = 0;
	desc[2].OutputSlot = 0;

	desc[3].SemanticName = "SIZE";
	desc[3].SemanticIndex = 0;
	desc[3].ComponentCount = 1;
	desc[3].StartComponent = 0;
	desc[3].Stream = 0;
	desc[3].OutputSlot = 0;

	UINT stride = sizeof(VertexTypesStruct::VertexParticel);
	UINT offset = 0;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		CreateGeometryShaderWithStreamOutput(bytes, length, desc, ARRAYSIZE(desc)
		, &stride, 1, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, this->gShaderSO.GetAddressOf()));
}

void AngelCore::AngelRendering::Shader::CreateGeometryShader(const void * bytes, SIZE_T length)
{
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		CreateGeometryShader(bytes, length, nullptr, this->gShader.GetAddressOf()));
}
void AngelCore::AngelRendering::Shader::BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV[2], ShaderType ST)
{
	switch (ST)
	{
	case ShaderType::VertexShader:
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->VSSetShaderResources(startSlot, numBuffer, SRV);
		break;
	case ShaderType::PixelShader:
		if(SRV!=nullptr)
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetShaderResources(startSlot, numBuffer, SRV);
		break;
	case ShaderType::ComputeShader:
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetShaderResources(startSlot, numBuffer, SRV);
		break;
	default:
		break;
	}
}

void AngelCore::AngelRendering::Shader::BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV, ShaderType ST)
{
	switch (ST)
	{
	case ShaderType::VertexShader:
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->VSSetShaderResources(startSlot, numBuffer, &SRV);
		break;
	case ShaderType::PixelShader:
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetShaderResources(startSlot, numBuffer, &SRV);
		break;
	case ShaderType::ComputeShader:
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetShaderResources(startSlot, numBuffer, &SRV);
		break;
	case ShaderType::GeometryShader:
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->GSSetShaderResources(startSlot, numBuffer, &SRV);
		break;
	default:
		break;
	}
}

void AngelCore::AngelRendering::Shader::BindUAV(UINT startSlot, UINT numbuffer, ID3D11UnorderedAccessView *UAV)
{
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetUnorderedAccessViews(startSlot, numbuffer, &UAV, nullptr);
}

void AngelCore::AngelRendering::Shader::UnBindSRV(UINT startSlot, UINT numBuffer, ShaderType ST)
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
void AngelCore::AngelRendering::Shader::UnBindUAV(UINT startSlot, UINT numBuffer)
{
	ID3D11UnorderedAccessView *uav = nullptr;
	BindUAV(startSlot, numBuffer, uav);
}

void AngelCore::AngelRendering::Shader::UnBindSampler(UINT startSlot, UINT numBuffer)
{
	ID3D11SamplerState *uav = nullptr;
	SetSampler(startSlot, numBuffer, &uav);
}

void AngelCore::AngelRendering::Shader::SetComputeShader(_In_ ID3D11ComputeShader * CS)
{
	this->cShader = CS;
}

void AngelCore::AngelRendering::Shader::SetVertexShader(_In_  ID3D11VertexShader* VS)
{
	this->vShader = VS;
}

void AngelCore::AngelRendering::Shader::SetPixelShader(_In_  ID3D11PixelShader* PS)
{
	this->pShader = PS;
}

void AngelCore::AngelRendering::Shader::SetConstantBuffer(UINT startSlot, UINT numBuffer, _In_ ID3D11Buffer* CBuffer)
{
	if (vShader) AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->VSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
	if (pShader) AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
	if (cShader) AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
	if (gShader || gShaderSO) AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->GSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
}

void AngelCore::AngelRendering::Shader::SetInputLayout(ID3D11InputLayout* value)
{
	this->inputLayout = value;
}

void AngelCore::AngelRendering::Shader::SetTexture2D(UINT StartSLot, UINT NumViews, Texture2D* texture2D)
{
	if (AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.size() != 0)
	{
		auto sampler = AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf();

		if (pShader)
		{
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetShaderResources(StartSLot, NumViews, texture2D->GetSRV().GetAddressOf());
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetSamplers(StartSLot, 1, sampler);
		}

		if (cShader != nullptr)
		{
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetShaderResources(StartSLot, NumViews, texture2D->GetSRV().GetAddressOf());
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetSamplers(StartSLot, 1, sampler);
		}
	}
}

void AngelCore::AngelRendering::Shader::SetSampler(UINT startSlot, UINT numView, ID3D11SamplerState ** sampler)
{
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetSamplers(startSlot, numView, sampler);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->GSSetSamplers(startSlot, numView, sampler);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetSamplers(startSlot, numView, sampler);
}

void AngelCore::AngelRendering::Shader::SetTexture2D(UINT StartSLot, UINT NumViews, ID3D11ShaderResourceView ** SRV)
{
	if (AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.size() != 0)
	{
		auto sampler = AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf();
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetShaderResources(StartSLot, NumViews, SRV);
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetSamplers(StartSLot, NumViews, sampler);
	}
}

void AngelCore::AngelRendering::Shader::Apply()
{
	
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetInputLayout(this->inputLayout.Get());
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->VSSetShader(this->vShader.Get(), nullptr, 0);

	if (gShader)
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->GSSetShader(this->gShader.Get(), nullptr, 0);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetShader(this->pShader.Get(), nullptr, 0);
	if (this->cShader)
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetShader(this->cShader.Get(), nullptr, 0);
}

void AngelCore::AngelRendering::Shader::ApplySO(ID3D11Buffer * buffer)
{
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetInputLayout(this->inputLayout.Get());

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->VSSetShader(this->vShader.Get(), nullptr, 0);
	UINT offset = 0;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->SOSetTargets(1, &buffer, &offset);
	if (gShaderSO)
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->GSSetShader(this->gShaderSO.Get(), nullptr, 0);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetShader(nullptr, nullptr, 0);

}

void AngelCore::AngelRendering::Shader::Disable()
{
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->VSSetShader(nullptr, nullptr, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->PSSetShader(nullptr, nullptr, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetShader(nullptr, nullptr, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->GSSetShader(nullptr, nullptr, 0);
}


void AngelCore::AngelRendering::Shader::Dispatch(UINT X, UINT Y, UINT Z)
{
	if (cShader)
	{
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CSSetShader(this->cShader.Get(), nullptr, 0);
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->Dispatch(X, Y, Z);
	}
}

HRESULT AngelCore::AngelRendering::Shader::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* vertexDesc, const UINT vertexDescLength,
	const void* shaderData, const SIZE_T shaderDataLength)
{
	return AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateInputLayout(
		vertexDesc,
		vertexDescLength,
		shaderData,
		shaderDataLength,
		this->inputLayout.GetAddressOf());
};

HRESULT AngelCore::AngelRendering::Shader::CreateInputLayout(VertexTypes vertexTypes, const void* shaderData, SIZE_T shaderDataLength)
{
	HRESULT hr = S_OK;
	if (vertexTypes == Position)
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == VertexTypes::PositionTexCoord)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == PositionColor)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == VertexTypes::PositionNormalTexture)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == VertexTypes::PositionNormalTextureColor)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == VertexTypes::Terrain)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "LAYERS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TILENUMBER", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == PositionNormalTexture)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == PositionNormalTextureTangent)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == PositionNormalTextureTangentBinormal)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == PositionNormalTangentColorTexture)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == PositionNormalTangentTextureSkinned)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES",0, DXGI_FORMAT_R32G32B32A32_UINT,      0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "mTransform", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "mTransform", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "mTransform", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "mTransform", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == VertexTypes::Particle)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "LIFE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == VertexTypes::SkyVertex)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else if (vertexTypes == VertexTypes::PlaneVertex)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HR(CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength));
	}
	else
	{
		hr = S_FALSE;
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Unkown Shader Format");
	}
	return hr;
}

void AngelCore::AngelRendering::Shader::Release()
{

}