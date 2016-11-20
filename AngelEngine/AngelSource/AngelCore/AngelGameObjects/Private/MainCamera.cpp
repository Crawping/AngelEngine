//// ***********************************************************************
//// FileName         : MainCamera.cpp
//// Author           : Peyman Tahghighi
////
//// ***********************************************************************
////     Copyright (c) Peyman Tahghighi . All rights reserved.
////
//// <summary> Floating Camera  </summary>
//// ***********************************************************************
//
//#include "../../AngelGameObjects/Public/MainCamera.h"
//#include"../../../AngelCore/AngelSubSystem/AngelWorldSystem/AngelComponent/Public/Transform.h"
//#include<AntTweakBar.h>
//
//
//
//MainCamera::MainCamera()
//{
//
//}
//float angle = 2.5;
//void MainCamera::Update(const AngelCore::AngelSubSystem::TimeManager & timer)
//{
//	using AngelCore::AngelSubSystem::inputManager;
//	using namespace AngelCore::AngelSubSystemResources;
//	using namespace DirectX;
//	
//	float tmpX = GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX();
//	float tmpY = 0.0f;
//	float tmpZ = GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ();;
//
//	DirectX::XMVECTOR tmpPosition;
//	tmpPosition = DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraPosition());
//	this->m_lastPosition = this->m_curPosition;
//	this->m_curPosition = AngelCore::AngelSubSystem::inputManager->GetMousePosition();
//	
//	//MessageBoxA(NULL, messageS.c_str(), "Error", MB_ERROR);
//
//	if (inputManager->IsRightMouseClicked())
//	{
//		if (this->m_lastPosition.x != this->m_curPosition.x || this->m_lastPosition.y != this->m_curPosition.y)
//		{
//			GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetY
//				(GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY()
//					-(this->m_lastPosition.x - this->m_curPosition.x)*(static_cast<float>(timer.GetElapsedSeconds()))
//					);
//
//			GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetX
//				(GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX() -
//					(this->m_lastPosition.y - this->m_curPosition.y)*static_cast<float>(timer.GetElapsedSeconds()));
//		}
//	}
//
//	/*if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::A))
//	{
//		{
//			m_updateAABB = true;
//			tmpX =
//				(GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX()
//					- (timer.GetElapsedSeconds() * 1));
//		}
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::D))
//	{
//		{
//			m_updateAABB = true;
//			tmpX = ((timer.GetElapsedSeconds() * 1 + 
//				GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX()));
//		}
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::W))
//	{
//		{
//			m_updateAABB = true;
//			tmpZ = (((timer.GetElapsedSeconds() * 1) + GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ()));
//		}
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::S))
//	{
//		{
//			m_updateAABB = true;
//			tmpZ =
//				(GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ() -
//					(timer.GetElapsedSeconds() * 1));
//		}
//	}*/
//
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::UP))
//	{
//	{
//	GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetX(
//	GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX() + XMConvertToRadians(10));
//	}
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::DOWN))
//	{
//	{
//		GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetX(
//		GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX() - XMConvertToRadians(10));
//	}
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::LEFT))
//	{
//	{
//	GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetY(
//	GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY() + XMConvertToRadians(10));
//	}
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::RIGHT))
//	{
//	{
//	GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetY(
//	GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY() - XMConvertToRadians(10));
//	}
//	}
//	float dir = atan2(-BaseCameraProperties::BCamera->GetCameraPosition().x,
//		-BaseCameraProperties::BCamera->GetCameraPosition().z);
//	OutputDebugStringA("\n");
//	OutputDebugStringA(std::to_string(dir).c_str());
//	
//
//	XMFLOAT4X4 rotate;
//	XMMATRIX temprotate = DirectX::XMMatrixRotationRollPitchYaw(
//		GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX(),
//		dir, 0);
//
//	BaseCameraProperties::BCamera->SetCameraDirection
//		(DirectX::XMFLOAT3(GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX()
//			, GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY(),
//			GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetZ()));
//
//	XMStoreFloat4x4(&rotate, temprotate);
//	BaseCameraProperties::BCamera->SetCameraTarget(DirectX::XMVector3TransformCoord
//		(DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetDefaultForward())
//			, XMLoadFloat4x4(&rotate)));
//	BaseCameraProperties::BCamera->SetCameraTarget(DirectX::XMVector3Normalize
//		(DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraTarget())));
//	temprotate = DirectX::XMMatrixRotationRollPitchYaw
//		(0, GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY(),  0);
//	XMStoreFloat4x4(&rotate, temprotate);
//
//	BaseCameraProperties::BCamera->SetCameraRight
//		(DirectX::XMVector3TransformCoord(
//			DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetDefaultRight()),
//			XMLoadFloat4x4(&rotate)));
//	BaseCameraProperties::BCamera->
//		SetCameraForward(XMVector3TransformCoord(
//			DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetDefaultForward()),
//			XMLoadFloat4x4(&rotate)));
//	BaseCameraProperties::BCamera->
//		SetCameraUp(XMVector3Cross(DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraForward()),
//			DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraRight())));
//
//	
//
//	if (this->m_lastXPosition != tmpX)
//	{
//		tmpPosition = (XMVectorAdd(DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraPosition())
//			, (tmpX
//				- this->m_lastXPosition)*DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraRight())));
//	}
//	if (this->m_lastZPosition != tmpZ)
//	{
//		tmpPosition = (XMVectorAdd(DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraPosition())
//			, (tmpZ
//				- this->m_lastZPosition)*DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraForward())));
//	}
//	if (this->m_lastYPosition !=tmpY)
//	{
//		tmpPosition = (
//			XMVectorAdd(DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraPosition())
//				, (tmpY
//					- this->m_lastYPosition)*DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraUp())));
//	}
//
//	DirectX::XMVECTOR camTarget = DirectX::XMVectorSet(100, -100, 0, 0);
//	dir = atan2(-BaseCameraProperties::BCamera->GetCameraPosition().x,
//		-BaseCameraProperties::BCamera->GetCameraPosition().z);
//	OutputDebugStringA("\n");
//	OutputDebugStringA(std::to_string(dir).c_str());
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::A))
//	{
//		angle += 0.05f;
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::D))
//	{
//		angle -= 0.05f;
//	}
//
//	temprotate = DirectX::XMMatrixRotationRollPitchYaw(
//		0,
//		angle, 0);
//
//	DirectX::XMVECTOR camPos = XMVector3TransformCoord(
//		DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetDefaultForward())
//		, temprotate);
//	camPos = DirectX::XMVector3Normalize(camPos);
//	camPos *= -10;
//	camTarget.m128_f32[0] += 5;
//
//	camPos = XMVectorAdd(camPos, camTarget);
//	DirectX::XMVECTOR camForward = DirectX::XMVectorSet(
//		camPos.m128_f32[0] - camTarget.m128_f32[0],
//		0,
//		camPos.m128_f32[0] - camTarget.m128_f32[0],1.0f);
//
//	camForward = XMVector3Normalize(camForward);
//	DirectX::XMVECTOR camRight =
//		DirectX::XMVectorSet(-camForward.m128_f32[2],
//			0.0f,
//			camForward.m128_f32[0], 1.0f);
//	DirectX::XMVECTOR camUp = XMVector3Cross(camForward, camRight);
//	camUp = XMVector3Normalize(camUp);
//	camUp.m128_f32[0] *= -1;
//	camUp.m128_f32[1] *= -1;
//	camUp.m128_f32[2] *= -1;
//
//	BaseCameraProperties::BCamera->SetCameraRight(camRight);
//	BaseCameraProperties::BCamera->SetCameraTarget(camTarget);
//	BaseCameraProperties::BCamera->SetCameraUp(camUp);
//	BaseCameraProperties::BCamera->SetCameraPosition(camPos);
//
//	//XMMATRIX invView = DirectX::XMMatrixInverse(
//	//	&(XMMatrixDeterminant(BaseCameraProperties::BCamera->GetView())),
//	//	BaseCameraProperties::BCamera->GetView());
//
//	//// Set the translation parts of matrix to zero.  This means it will only rotate,
//	//// not translate.
//	//invView.r[3].m128_f32[0] = invView.r[3].m128_f32[1] = invView.r[3].m128_f32[2] = 0;
//
//	//	// Create a translation vector that's in object space for the camera.
//	//XMFLOAT3 translationVector(0, 0, 0);
//	//if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::A))
//	//{
//	//	translationVector.x = -0.00001f;
//	//}
//	//if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::D))
//	//{
//	//	translationVector.x = 0.00001f;
//	//}
//	//if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::W))
//	//{
//	//	translationVector.z = 0.00001f;
//	//}
//	//if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::S))
//	//{
//	//	translationVector.z = -0.00001f;
//	//}
//
//	//DirectX::XMStoreFloat3(&translationVector
//	//	,DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&translationVector), invView));
//
//	//DirectX::XMFLOAT3 start;
//	///*start.x = 10;
//	//start.y = 0;
//	//start.z = 0;
//	//translationVector.x += start.x;
//	//translationVector.y += start.y;
//	//translationVector.z += start.z;*/
//	//
//
//	//// Now rotate our translation vector so that aligned with the camera's view
//	//// direction
//	////D3DXVec3TransformCoord(&translationVector, &translationVector, &cameraWorldMatrix);
//
//	//// Now, we flatten the vector so it's in the XZ plane.
//	//translationVector.y = 0;
//
//	//DirectX::XMStoreFloat3(&translationVector,
//	//	DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&translationVector)));
//
//	//// Normalize the vector
//	////D3DXVec3Normalize(&translationVector, &translationVector);
//
//	//// Multiply so it's magnitude == distance
//	//translationVector.x *= 2;
//	//translationVector.y *= 2;
//	//translationVector.z *= 2;
//
//	//// Translate the camera's position
//	////camera.position += translationVector;
//	//BaseCameraProperties::BCamera->SetCameraPosition(translationVector);
//
//	/*if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::A))
//	{
//		angle += 0.05f;
//	}
//	if (inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::Keys::D))
//	{
//		angle -= 0.05f;
//	}
//	tmpPosition.m128_f32[0] = sin(angle)*5;
//	tmpPosition.m128_f32[2] = cos(angle)*5;
//	BaseCameraProperties::BCamera->SetCameraPosition(tmpPosition);*/
//
//
//	if (true)
//	{
//		BoundingOrientedBox tmp;
//		tmp.Center = DirectX::XMFLOAT3(
//			tmpPosition.m128_f32[0],
//			tmpPosition.m128_f32[1],
//			tmpPosition.m128_f32[2]);
//		BaseCameraProperties::BCamera->SetCameraAABB(tmp);
//		bool intersects = false;
//		for (auto const & member : BaseCameraProperties::BCamera->GetCollisionObjects())
//		{
//			if (member.collisionShape.Intersects(tmp))
//			{
//				intersects = true;
//				break;
//			}
//		}
//
//		/*if (true)
//		{
//			BaseCameraProperties::BCamera->SetCameraPosition(tmpPosition);
//			GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->
//				SetZ(tmpZ);
//			GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->
//				SetX(tmpX);
//		}*/
//
//		m_updateAABB = false;
//	}
//	this->m_debugCollisionShape->SetBoundingBox(BaseCameraProperties::BCamera->GetCameraAABB());
//	this->m_debugCollisionShape->Update(
//		BaseCameraProperties::BCamera->GetView(),
//		BaseCameraProperties::BCamera->GetProjection());
//
//	/*if (!BaseCameraProperties::BCamera.GetCollided())
//	{
//	m_lastCameraPosition = BaseCameraProperties::BCamera.GetCameraPosition();
//	}
//	if (BaseCameraProperties::BCamera.GetCollided())
//	{
//	BaseCameraProperties::BCamera.SetCameraPosition(m_lastCameraPosition);
//	}
//
//
//	*/
//
//	/*BaseCameraProperties::BCamera->SetCameraTarget(XMVectorAdd(
//		DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraTarget()),
//		DirectX::XMLoadFloat3(&BaseCameraProperties::BCamera->GetCameraPosition())));*/
//	BaseCameraProperties::BCamera->UpdateView(BaseCameraProperties::BCamera->GetCameraPosition(),
//		BaseCameraProperties::BCamera->GetCameraTarget(),
//		BaseCameraProperties::BCamera->GetCameraUp());
//
//	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->RefreshProjection();
//	this->m_lastXPosition = GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX();
//	this->m_lastZPosition = GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ();
//	this->m_lastYPosition = GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetY();
//	
//}
//
//void MainCamera::Initialize()
//{
//	this->m_camRotationSpeed = 1.0f;
//	this->m_camMovmentSpeed = 10.0f;
//	this->m_lastPosition.x = 0.0f;
//	this->m_lastPosition.y = 0.0f;
//	this->m_curPosition.x = 0.0f;
//	this->m_curPosition.y = 0.0f;
//
//	//this->m_skyMapn->Initialize("night.dds",0.5f, AngelCore::AngelRendering::SkyMap::SkyMapType::Static);
//	this->m_debugCollisionShape = new AngelCore::AngelRendering::DebugCollisionShape();
//	this->m_debugCollisionShape->Create();
//	m_updateAABB = true;
//	DirectX::BoundingOrientedBox tmp;
//	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(10, 10, 10);
//	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraAABB().Transform(tmp, scale);
//	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetCameraAABB(tmp);
//	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetCollided(false);
//	AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetCameraPosition(
//		DirectX::XMVectorSet(10, 0, 0, 0));
//
//
//	DirectX::BoundingFrustum f;
//	f.Far = 1000;
//	f.Near = 0.1f;
//
//
//	m_lastXPosition = 0.0f;
//	m_lastYPosition = 0.0f;
//	m_lastZPosition = 0.0f;
//
//	//
//
//	////f.
//
//}
//
//void MainCamera::Render()
//{
//	
//	//this->m_skyMapn->Update();
//	///this->m_skyMapn->Render();
//	//this->m_debugCollisionShape->Render();
//}