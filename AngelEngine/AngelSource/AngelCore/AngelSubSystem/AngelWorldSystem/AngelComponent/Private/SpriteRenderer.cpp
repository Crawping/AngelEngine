// ***********************************************************************
// FileName         : SpriteRenderer.cpp
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Component for rendering a 2D sprite  </summary>
// ***********************************************************************


#include "../Public/SpriteRenderer.h"
#include"../../../Public/SubSystemsResource.h"
#include"../../Public/Actor.h"
#include"../Public/Transform.h"



AngelCore::AngelWorldSystem::SpriteRenderer::SpriteRenderer()
{
	

}

void AngelCore::AngelWorldSystem::SpriteRenderer::Initialize(AngelCore::AngelWorldSystem::Actor* _owner, ...)
{
	using namespace DirectX;
	this->m_owner = _owner;
	this->m_shader = new AngelCore::AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ShaderType::VertexShader, "VertexLitSprite.cso", VertexTypes::PositionTexCoord, this->m_shader);
	AngelRendering::Shader::LoadShader(ShaderType::PixelShader, "PixelLitSprite.cso", VertexTypes::NOP, this->m_shader);


	this->m_stride = sizeof(VertexTypesStruct::PositionTexCoord);
	this->m_offset = 0;

	const VertexTypesStruct::PositionTexCoord vertices[] =
	{
		{ XMFLOAT3(0.5f, 0.5f, 0.0f),XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypesStruct::PositionTexCoord)*ARRAYSIZE(vertices);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA VBSD;
	VBSD.pSysMem = vertices;

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer(&vertexBufferDesc, &VBSD, this->m_vertexBuffer.GetAddressOf());

	const unsigned short indices[] = { 0, 1, 2, 0, 3, 1 };

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(unsigned short)*ARRAYSIZE(indices);
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA IBSD;
	IBSD.pSysMem = indices;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer(&indexBufferDesc, &IBSD, this->m_indexBuffer.GetAddressOf());

	this->m_cbPerObjectBuffer.Load();
	this->m_stride = sizeof(VertexTypesStruct::PositionTexCoord);
	this->m_offset = 0;
}


void AngelCore::AngelWorldSystem::SpriteRenderer::Update(const AngelCore::AngelSubSystem::TimeManager &timer)
{
	
}

void AngelCore::AngelWorldSystem::SpriteRenderer::RenderSRV(ID3D11ShaderResourceView * srv)
{
	DirectX::XMMATRIX World = this->GetOwner()->GetComponent<Transform>()->GetWorldMatrix();
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Projecion;
	View = AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView();
	Projecion = AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection();

	DirectX::XMStoreFloat4x4(&this->m_cbPerObjectBuffer.Const.WVP, DirectX::XMMatrixTranspose(World*View*Projecion));
	this->m_cbPerObjectBuffer.Update();
	this->m_shader->SetConstantBuffer(0, 1, this->m_cbPerObjectBuffer.getBuffer());
	this->m_shader->SetSampler(0, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	this->m_shader->BindSRV(0, 1, srv, ShaderType::PixelShader);

	this->m_shader->Apply();
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &this->m_stride, &this->m_offset);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->DrawIndexed(6, 0, 0);
	this->m_shader->Disable();
}

void AngelCore::AngelWorldSystem::SpriteRenderer::Render()
{
	
}
