//--------------------------------------------------------------------------------------
// File: Model.cpp
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------


#include "../Public/Model.h"
#include"../../../Public/SubSystemsResource.h"
#include<string>
#include"../../../AngelCore/AngelTools/AngelMathHelper/Public/MathHelper.h"
#include"../../../AngelRendering/Public/LightManager.h"//Set light data and textures
#include"../Public/Transform.h"
#include"../Public/RigidBody.h"//GetRigidBody
#include"../../Public/Actor.h"//GetOnwer
#include"../../../Public/SubSystemManager.h"//GetPhysicManager
#include"../../../Public/PhysicManager.h"//GetDynamicWorld
#include"../../../Public/InputManager.h"//GetDynamicWorld

using namespace DirectX;

#ifndef _CPPRTTI 
#error Model requires RTTI
#endif

//--------------------------------------------------------------------------------------
// ModelMeshPart
//--------------------------------------------------------------------------------------

AngelCore::AngelWorldSystem::ModelMeshPart::ModelMeshPart() :
	indexCount(0),
	startIndex(0),
	vertexOffset(0),
	vertexStride(0),
	primitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	indexFormat(DXGI_FORMAT_R16_UINT),
	isAlpha(false)
{
}

AngelCore::AngelWorldSystem::ModelMeshPart::~ModelMeshPart()
{

}

_Use_decl_annotations_
void AngelCore::AngelWorldSystem::ModelMeshPart::Draw(ID3D11DeviceContext* deviceContext,
	ID3D11InputLayout* iinputLayout, AngelCore::AngelRendering::Shader * shader,bool depth
	, CXMMATRIX world, CXMMATRIX view, CXMMATRIX projection)
{
	//deviceContext->IASetInputLayout(iinputLayout);
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WVP, XMMatrixTranspose(world*view*projection));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.ViewProjection, XMMatrixTranspose(view*projection));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WVPLast, (XMLoadFloat4x4(&lWVP)));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WorldView, XMMatrixTranspose(world));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WorldInvTranspose, InverseTranspose(world));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.lightViewMatrix, XMMatrixTranspose(
		XMLoadFloat4x4(&AngelCore::AngelRendering::LightManager::GetInstance()->GetViews()[0])));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.lightProjectionMatrix,
		XMMatrixTranspose
		(XMLoadFloat4x4(&AngelCore::AngelRendering::LightManager::GetInstance()->GetProjections()[0])));
	if (!depth)
		lWVP = this->m_wvpBuffer.Const.WVP;
	///deviceContext->GenerateMips(this->textureView[0]);
	auto vb = vertexBuffer.Get();
	UINT vbStride = sizeof(VertexTypesStruct::VertexPositionNormalTangentTextureSkinned);
	UINT vbOffset = 0;
	
	this->m_wvpBuffer.Update();
	shader->SetConstantBuffer(0, 1, this->m_wvpBuffer.getBuffer());

	this->m_material.Const.Skinning = enableSkinning;
	
	this->m_material.Update();
	shader->SetConstantBuffer(1, 1, this->m_material.getBuffer());
	
	AngelCore::AngelRendering::LightManager::GetInstance()->Update();
	AngelCore::AngelRendering::LightManager::GetInstance()->GetBuffer().Update();
	shader->SetConstantBuffer(2, 1, AngelCore::AngelRendering::LightManager::GetInstance()->GetBuffer().getBuffer());

	shader->SetSampler(0, 1, 
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	shader->SetSampler(3, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(4).GetAddressOf());

	if (this->textureView[0] != nullptr)
		shader->BindSRV(0, 8, this->textureView, ShaderType::PixelShader);
	if (!depth)
	{
	shader->BindSRV(9, 1,
		AngelCore::AngelRendering::LightManager::GetInstance()->GetSRVDirectionalLight()
		, ShaderType::PixelShader);
	}
	//shader->BindSRV(3, 1, AngelCore::AngelRendering::LightManager::GetInstance()->GetSRV(),
	//	ShaderType::PixelShader);

	shader->Apply();

	deviceContext->IASetVertexBuffers(0, 1, &vb, &vbStride, &vbOffset);
	// Note that if indexFormat is DXGI_FORMAT_R32_UINT, this model mesh part requires a Feature Level 9.2 or greater device
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Draw the primitive.
	deviceContext->IASetPrimitiveTopology(primitiveType);
	
	deviceContext->DrawIndexed(indexCount, startIndex, vertexOffset);
	shader->UnBindSRV(0, 8,ShaderType::PixelShader);
	shader->UnBindSRV(9, 1, ShaderType::PixelShader);
	shader->UnBindSampler(0, 1);
	shader->UnBindSampler(3, 1);
	shader->Disable();
}

_Use_decl_annotations_
void AngelCore::AngelWorldSystem::ModelMeshPart::DrawInstanced(ID3D11DeviceContext* deviceContext,
	Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer,
	ID3D11InputLayout* iinputLayout, AngelCore::AngelRendering::Shader * shader, bool depth
	,CXMMATRIX view, CXMMATRIX projection, UINT instanceCount)
{
	//deviceContext->IASetInputLayout(iinputLayout);
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WVP, XMMatrixTranspose(DirectX::XMMatrixIdentity()*view*projection));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WVPLast, (XMLoadFloat4x4(&lWVP)));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.ViewProjection, XMMatrixTranspose(view*projection));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WorldView, XMMatrixTranspose(DirectX::XMMatrixIdentity()));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.WorldInvTranspose, InverseTranspose(DirectX::XMMatrixIdentity()));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.lightViewMatrix, XMMatrixTranspose(
		XMLoadFloat4x4(&AngelCore::AngelRendering::LightManager::GetInstance()->GetViews()[0])));
	XMStoreFloat4x4(&this->m_wvpBuffer.Const.lightProjectionMatrix,
		XMMatrixTranspose
		(XMLoadFloat4x4(&AngelCore::AngelRendering::LightManager::GetInstance()->GetProjections()[0])));
	if (!depth)
		lWVP = this->m_wvpBuffer.Const.WVP;
	///deviceContext->GenerateMips(this->textureView[0]);
	auto vb = vertexBuffer.Get();
	unsigned int strides[2];
	unsigned int offsets[2];
	strides[0] = sizeof(VertexTypesStruct::VertexPositionNormalTangentTextureSkinned);
	strides[1] = sizeof(VertexTypesStruct::Instancing);
	offsets[0] = 0;
	offsets[1] = 0;

	this->m_wvpBuffer.Update();
	shader->SetConstantBuffer(0, 1, this->m_wvpBuffer.getBuffer());

	this->m_material.Const.Skinning = enableSkinning;
	this->m_material.Const.instancing = 1;

	this->m_material.Update();
	shader->SetConstantBuffer(1, 1, this->m_material.getBuffer());


	AngelCore::AngelRendering::LightManager::GetInstance()->Update();
	AngelCore::AngelRendering::LightManager::GetInstance()->GetBuffer().Update();
	shader->SetConstantBuffer(2, 1, AngelCore::AngelRendering::LightManager::GetInstance()->GetBuffer().getBuffer());


	shader->SetSampler(0, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	shader->SetSampler(3, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(4).GetAddressOf());
	if (this->textureView[0] != nullptr)
		shader->BindSRV(0, 8, this->textureView, ShaderType::PixelShader);
	if (!depth) {
		shader->BindSRV(9, 1, AngelCore::AngelRendering::LightManager::GetInstance()->GetSRVDirectionalLight()
			, ShaderType::PixelShader);
	}
	//shader->BindSRV(3, 1, AngelCore::AngelRendering::LightManager::GetInstance()->GetSRV(),
	//	ShaderType::PixelShader);

	shader->Apply();

	ID3D11Buffer * combinedBuffer[2];
	combinedBuffer[0] = vb;
	combinedBuffer[1] = instanceBuffer.Get();

	deviceContext->IASetVertexBuffers(0, 2, combinedBuffer, strides, offsets);
	// Note that if indexFormat is DXGI_FORMAT_R32_UINT, this model mesh part requires a Feature Level 9.2 or greater device
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);



	//ieffect->Apply(deviceContext);


	// Draw the primitive.
	deviceContext->IASetPrimitiveTopology(primitiveType);

	deviceContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
	shader->UnBindSRV(0, 8, ShaderType::PixelShader);
	shader->UnBindSRV(9, 1, ShaderType::PixelShader);
	shader->UnBindSampler(3, 1);
	shader->UnBindSampler(0, 1);
	shader->Disable();
}




//--------------------------------------------------------------------------------------
// ModelMesh
//--------------------------------------------------------------------------------------

AngelCore::AngelWorldSystem::ModelMesh::ModelMesh() :
	ccw(true),
	pmalpha(true)
{

	m_transform = new AngelCore::AngelWorldSystem::Transform();
	m_transform->GetScalling()->SetX(0);
	m_transform->GetScalling()->SetY(0);
	m_transform->GetScalling()->SetZ(0);

	DirectX::XMStoreFloat4x4(&lWorld,XMMatrixIdentity());
}


AngelCore::AngelWorldSystem::ModelMesh::~ModelMesh()
{
}

_Use_decl_annotations_
void XM_CALLCONV AngelCore::AngelWorldSystem::ModelMesh::Draw(ID3D11DeviceContext* deviceContext,
	CXMMATRIX view, CXMMATRIX projection, AngelCore::AngelRendering::Shader * shader,
	std::wstring name,bool depth, AngelCore::AngelWorldSystem::Actor * owner)
{
	assert(deviceContext != 0);
	XMVECTOR rot = DirectX::XMVectorSet(
		owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX(),
		owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY(),
		owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetZ(),
		1.0f);
	XMVECTOR s = DirectX::XMVectorSet(
		owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetX(),
		owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetY(),
		owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetZ(),
		1.0f);
	XMVECTOR trans
		= DirectX::XMVectorSet(
			owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX(),
			owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetY(),
			owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ(),
			1.0f);
	
	//rot.m128_f32[0] += m_transform->GetRotation()->GetX();
	//rot.m128_f32[1] += m_transform->GetRotation()->GetY();
	//rot.m128_f32[2] += m_transform->GetRotation()->GetZ();

	//s.m128_f32[0] += m_transform->GetScalling()->GetX();
	//s.m128_f32[1] += m_transform->GetScalling()->GetY();
	//s.m128_f32[2] += m_transform->GetScalling()->GetZ();

	///*trans.m128_f32[0] += transformation.getOrigin().getX();
	//trans.m128_f32[1] += transformation.getOrigin().getY();
	//trans.m128_f32[2] += transformation.getOrigin().getZ();*/

	//trans.m128_f32[0] += m_transform->GetPosition()->GetX();
	//trans.m128_f32[1] += m_transform->GetPosition()->GetY();
	//trans.m128_f32[2] += m_transform->GetPosition()->GetZ();
	XMMATRIX world;
	if(!owner->GetComponent<AngelCore::AngelWorldSystem::RigidBody>())
	world = DirectX::XMMatrixScaling(s.m128_f32[0], s.m128_f32[1], s.m128_f32[2])*
		DirectX::XMMatrixRotationRollPitchYawFromVector(rot)*XMMatrixTranslationFromVector(trans);
	else
		world = DirectX::XMMatrixScaling(s.m128_f32[0], s.m128_f32[1], s.m128_f32[2])*
		owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotationMatrix()
		*XMMatrixTranslationFromVector(trans);

	/*bool updateAABB = false;
	if (!XMVector4Equal(lWorld., world.r[0])
		||
		!XMVector4Equal(lWorld.r[1], world.r[1])
		||
		!XMVector4Equal(lWorld.r[2], world.r[2])
		||
		!XMVector4Equal(lWorld.r[3], world.r[3]))
	{
		updateAABB = true;
	}*/

	for (auto it = meshParts.cbegin(); it != meshParts.cend(); ++it)
	{
		auto part = (*it).get();
		assert(part != 0);
	
		XMMATRIX translation = XMMatrixTranslation
			(part->boundingBox.Center.x, part->boundingBox.Center.y, part->boundingBox.Center.z);
		XMMATRIX scale = XMMatrixScaling
			(part->boundingBox.Extents.x, part->boundingBox.Extents.y, part->boundingBox.Extents.z);
		XMMATRIX rotation = XMMatrixRotationQuaternion(DirectX::XMVectorSet(
			part->boundingBox.Orientation.x, part->boundingBox.Orientation.y,
			part->boundingBox.Orientation.z, part->boundingBox.Orientation.w));

		if (std::wcscmp(name.c_str(), L"window_roof") == 0)
		{
			BoundingOrientedBox tmp;
			tmp = part->boundingBox;
			part->boundingBox.Transform(tmp, world);

			CollisionModel cm;
			cm.collisionShape = tmp;
			std::string nameS(name.begin(), name.end());
			cm.name = nameS;
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->UpdateCollisionObject(cm);
			part->collisionShape->SetBoundingBox(tmp);
			part->collisionShape->Update(view, projection);
			//if there is the part that we want then render it.
			part->collisionShape->Render();
		}

		armatureBuffer.Update();
		part->shader->SetConstantBuffer(3, 1, armatureBuffer.getBuffer());

		part->Draw(deviceContext, part->inputLayout.Get(), part->shader, depth
			, owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetWorldMatrix(),view,projection);
	}
}

_Use_decl_annotations_
void XM_CALLCONV AngelCore::AngelWorldSystem::ModelMesh::DrawInstanced(ID3D11DeviceContext* deviceContext
	,
	Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer,
	CXMMATRIX view, CXMMATRIX projection, AngelCore::AngelRendering::Shader * shader, bool updateAABB,
	std::wstring name, bool depth, UINT instanceCount)
{
	assert(deviceContext != 0);

	for (auto it = meshParts.cbegin(); it != meshParts.cend(); ++it)
	{
		auto part = (*it).get();
		assert(part != 0);

		if (updateAABB)
		{
			BoundingOrientedBox tmp;
			//part->boundingBox.Transform(tmp, world);
			tmp.Extents.y *= 1;
			//tmp.Extents.z *= 3;
			//tmp.Extents.x *= 3;
			part->boundingBox = tmp;
		}


		XMMATRIX translation = XMMatrixTranslation
			(part->boundingBox.Center.x, part->boundingBox.Center.y, part->boundingBox.Center.z);
		XMMATRIX scale = XMMatrixScaling
			(part->boundingBox.Extents.x, part->boundingBox.Extents.y, part->boundingBox.Extents.z);
		XMMATRIX rotation = XMMatrixRotationQuaternion(DirectX::XMVectorSet(
			part->boundingBox.Orientation.x, part->boundingBox.Orientation.y,
			part->boundingBox.Orientation.z, part->boundingBox.Orientation.w));

		part->DrawInstanced(
			deviceContext,instanceBuffer, part->inputLayout.Get(), part->shader, depth
			, view, projection,instanceCount);

	}
}


//--------------------------------------------------------------------------------------
// Model
//--------------------------------------------------------------------------------------

AngelCore::AngelWorldSystem::Model::~Model()
{
	for (auto it = meshes.cbegin(); it != meshes.cend(); ++it)
	{
		auto mesh = it->get();
	}
}


_Use_decl_annotations_
void XM_CALLCONV AngelCore::AngelWorldSystem::Model::Draw(ID3D11DeviceContext* deviceContext,
	CXMMATRIX view, CXMMATRIX projection, AngelCore::AngelRendering::Shader * shader
	,bool depth,AngelCore::AngelWorldSystem::Actor * owner)
{
	assert(deviceContext != 0);

	// Draw opaque parts
	for (auto it = meshes.cbegin(); it != meshes.cend(); ++it)
	{
		auto mesh = it->get();
		assert(mesh != 0);
		
		//shader->Apply();
		mesh->Draw(deviceContext, view, projection,shader,mesh->name,depth,owner);
	}
}


_Use_decl_annotations_
void XM_CALLCONV AngelCore::AngelWorldSystem::Model::DrawInstanced(_In_ ID3D11DeviceContext* deviceContext,
	DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection
	, AngelCore::AngelRendering::Shader * shader, bool depth, UINT instanceCount)
{
	// Draw opaque parts
	for (auto it = meshes.cbegin(); it != meshes.cend(); ++it)
	{
		auto mesh = it->get();
		assert(mesh != 0);

		//shader->Apply();
		mesh->DrawInstanced(deviceContext,m_instanceBuffer, view, projection, 
			shader, false, mesh->name, depth,instanceCount);
	}
}
