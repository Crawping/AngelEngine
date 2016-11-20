// ***********************************************************************
// FileName         : BaseCamera.cpp
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Base camera class represent all functionality camera can have  </summary>
// ***********************************************************************

#include "../Public/BaseCamera.h"
#include"../../../AngelCore/AngelSubSystem/Public/SubSystemsResource.h"
#include"../../Public\InputManager.h"

BaseCamera::BaseCamera()
{
	position = DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f);
	direction = DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f);
	DefaultForward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	DefaultRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	camForward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	camRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
}


void BaseCamera::UpdateView(DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up)
{
	this->position = eye;
	this->Up = up;

	DirectX::XMVECTOR vPosition = DirectX::XMLoadFloat3(&this->position);
	DirectX::XMVECTOR vLook = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat3(&this->Up);

	DirectX::XMStoreFloat4x4(&this->View,DirectX::XMMatrixLookAtLH(vPosition, vLook, vUp));
	
}

void BaseCamera::UpdateView(DirectX::XMVECTOR eye, DirectX::XMVECTOR target, DirectX::XMVECTOR up)
{
	DirectX::XMStoreFloat4x4(&this->View, DirectX::XMMatrixLookAtLH(eye, target, up));
}

void BaseCamera::UpdateView(Vector3 &eye, Vector3 &target, Vector3 &up)
{
	DirectX::XMStoreFloat4x4(&this->View, DirectX::XMMatrixLookAtLH(eye.GetXMVECTOR(),
		target.GetXMVECTOR(),
		up.GetXMVECTOR()));
}

void BaseCamera::UpdateProjection(float fieldOfView, float aspectRatio, float nearplane, float farplane)
{
	float fovAngleY = fieldOfView * DirectX::XM_PI / 180.0f;
	
	DirectX::XMStoreFloat4x4(&this->Projection,
		DirectX::XMMatrixPerspectiveFovLH(fovAngleY, 
			aspectRatio,
			nearplane, farplane));
}

void BaseCamera::UpdateProjection(float &fieldOfView)
{
	UpdateProjection(fieldOfView,
		static_cast<float>(AngelCore::AngelSubSystemResources::WindowProperties::GetWidth())
		/
		static_cast<float>(AngelCore::AngelSubSystemResources::WindowProperties::GetHeight())
		, 0.01f,
		10000.0f);
}

void BaseCamera::UpdateView(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 lookAt)
{
	DirectX::XMVECTOR vPosition = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR vLook = DirectX::XMLoadFloat3(&lookAt);
	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat3(&this->Up);
}

void BaseCamera::RefreshProjection()
{
	UpdateProjection(70.0f, 
		static_cast<float>(AngelCore::AngelSubSystemResources::WindowProperties::GetWidth())
		/ 
		static_cast<float>(AngelCore::AngelSubSystemResources::WindowProperties::GetHeight())
		, 0.01f, 
		10000.0f);
}

bool BaseCamera::TestIntersects(DirectX::BoundingOrientedBox b, DirectX::CXMMATRIX invWorld,
	DirectX::XMFLOAT2 * HitPoint2DSpace, DirectX::XMFLOAT3 * HitPointLocalSpace,float *dist)
{
	//Compute in view space
	using namespace DirectX;

	UINT numOfVP = 1;
	D3D11_VIEWPORT vp;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->RSGetViewports(&numOfVP, &vp);

	float vx = (+2.0f*AngelCore::AngelSubSystem::inputManager->GetMousePosition().x / vp.Width - 1.0f) / 
		Projection(0, 0);
	float vy = (-2.0f*AngelCore::AngelSubSystem::inputManager->GetMousePosition().y / vp.Height + 1.0f) / 
		Projection(1, 1);


	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	//we are now in view space.

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&View)),
		DirectX::XMLoadFloat4x4(&View));

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin,
		toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);
	//intersection tests.
	rayDir = XMVector3Normalize(rayDir);
	
	float tmin = 0.0f;
	
	if (b.Intersects(rayOrigin, rayDir, tmin))
	{
		XMVECTOR hitPosition = rayOrigin + rayDir*tmin;
		if (dist)
			*dist = tmin;
		if (HitPointLocalSpace)
		{
			HitPointLocalSpace->x = hitPosition.m128_f32[0];
			HitPointLocalSpace->y = hitPosition.m128_f32[1];
			HitPointLocalSpace->z = hitPosition.m128_f32[2];
		}

		if (HitPoint2DSpace) {
			HitPoint2DSpace->x = (hitPosition.m128_f32[0] *
				AngelCore::AngelSubSystemResources::WindowProperties::GetWidth() / 2) +
				AngelCore::AngelSubSystemResources::WindowProperties::GetWidth() / 2;
			float height = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
			HitPoint2DSpace->y = (hitPosition.m128_f32[1] *
				-height / 2) +
				height / 2;
		}
		
		return true;
	}
	return false;

}


bool BaseCamera::TestIntersectsTriangle(DirectX::CXMMATRIX invWorld,
	DirectX::XMVECTOR v1,
	DirectX::XMVECTOR v2,
	DirectX::XMVECTOR v3)
{
	//Compute in view space
	using namespace DirectX;

	UINT numOfVP = 1;
	D3D11_VIEWPORT vp;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->RSGetViewports(&numOfVP, &vp);

	float vx = (+2.0f*AngelCore::AngelSubSystem::inputManager->GetMousePosition().x / vp.Width - 1.0f) /
		Projection(0, 0);
	float vy = (-2.0f*AngelCore::AngelSubSystem::inputManager->GetMousePosition().y / vp.Height + 1.0f) /
		Projection(1, 1);


	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	//we are now in view space.

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&View)),
		DirectX::XMLoadFloat4x4(&View));

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin,
		toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);
	/*OutputDebugStringA("\nX:");
	OutputDebugStringA(std::to_string(rayDir.m128_f32[0]).c_str());
	OutputDebugStringA("\nY:");
	OutputDebugStringA(std::to_string(rayDir.m128_f32[1]).c_str());*/
	// Make the ray direction unit length for the
	//intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	float tmin = 0.0f;
	//OutputDebugStringA("\nX:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[0]).c_str());
	//OutputDebugStringA("\nY:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[1]).c_str());
	//OutputDebugStringA("\nZ:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[2]).c_str());
	if (TriangleTests::Intersects(rayOrigin, rayDir, v1, v2, v3, tmin))
	{
		XMVECTOR hitPosition = rayOrigin + rayDir*tmin;
		/*OutputDebugStringA("\nX:");
		OutputDebugStringA(std::to_string(hitPosition.m128_f32[0]).c_str());
		OutputDebugStringA("\nY:");
		OutputDebugStringA(std::to_string(hitPosition.m128_f32[1]).c_str());
		OutputDebugStringA("\nZ:");
		OutputDebugStringA(std::to_string(hitPosition.m128_f32[2]).c_str());*/
		return true;
	}
	return false;

}

//DirectX::XMFLOAT3 BaseCamera::GetMouseInWorldSpace()
//{
//	//Compute in view space
//	using namespace DirectX;
//
//	UINT numOfVP = 1;
//	D3D11_VIEWPORT vp;
//	AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->RSGetViewports(&numOfVP, &vp);
//
//	XMFLOAT4X4 proj;
//	XMStoreFloat4x4(&proj, Projection);
//
//	float vx = (+2.0f*AngelCore::AngelSubSystem::inputManager->GetMousePosition().x / vp.Width - 1.0f) / proj(0, 0);
//	float vy = (-2.0f*AngelCore::AngelSubSystem::inputManager->GetMousePosition().y / vp.Height + 1.0f) / proj(1, 1);
//
//
//	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//	XMVECTOR rayDir = XMVectorSet(vx, vy, -1.0f, 0.0f);
//
//	//we are now in view space.
//
//	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(View), View);
//
//	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);
//
//	rayOrigin = XMVector3TransformCoord(rayOrigin,
//		toLocal);
//	rayDir = XMVector3TransformNormal(rayDir, toLocal);
//	/*OutputDebugStringA("\nX:");
//	OutputDebugStringA(std::to_string(rayDir.m128_f32[0]).c_str());
//	OutputDebugStringA("\nY:");
//	OutputDebugStringA(std::to_string(rayDir.m128_f32[1]).c_str());*/
//	// Make the ray direction unit length for the
//	//intersection tests.
//	rayDir = XMVector3Normalize(rayDir);
//
//	float tmin = 0.0f;
//	//OutputDebugStringA("\nX:");
//	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[0]).c_str());
//	//OutputDebugStringA("\nY:");
//	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[1]).c_str());
//	//OutputDebugStringA("\nZ:");
//	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[2]).c_str());
//	if (TriangleTests::Intersects(rayOrigin, rayDir, v1, v2, v3, tmin))
//	{
//		return true;
//	}
//	return false;
//
//}