// ***********************************************************************
// FileName         : BaseCamera.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Base camera class represent all functionality camera can have  </summary>
// ***********************************************************************

#pragma once

#include<DirectXMath.h>
#include<DirectXCollision.h>

#ifndef __CBUFFER
#define __CBUFFER
#include"CBuffer.h"
#endif

#include"../../../AngelTools/AngelGlobalTools.h"


class GraphicDevice;

enum ProjectionType
{
	Perspective = 0,
	Orthographic = 1
};

struct CollisionModel
{
	DirectX::BoundingOrientedBox collisionShape;
	std::string name;
};


class BaseCamera
{
public:

	BaseCamera();

#pragma region Properties
	//get view MATRIX in render for calculate world view projection matrix
	DirectX::XMMATRIX GetView()
	{
		return DirectX::XMLoadFloat4x4(&this->View);
	}

	void SetView(DirectX::XMFLOAT4X4 V)
	{
		this->View = (V);
	}

	void SetProjection(DirectX::XMFLOAT4X4 P)
	{
		this->Projection = (P);
	}

	void SetView(const DirectX::XMMATRIX &_v)
	{
		DirectX::XMStoreFloat4x4(&this->View ,_v);
	}

	DirectX::XMFLOAT3 GetCameraPosition()
	{
		return this->position;
	}

	Vector3 GetCameraPosition() const
	{
		return Vector3(this->position);
	}

	DirectX::XMFLOAT3 GetCameraDirection()
	{
		return this->direction;
	}

	Vector3 GetCameraDirection() const
	{
		return Vector3(this->direction);
	}

	void SetCameraDirection(const DirectX::XMFLOAT3 &_dir)
	{
		this->direction = _dir;
	}

	void SetCameraDirection(Vector3 &_dir)
	{
		this->direction = _dir.GetXMFLOAT3();
	}

	void SetCameraPosition(const DirectX::XMFLOAT3 &_pos)
	{
		this->position = _pos;
	}
	

	void SetCameraPosition(Vector3 &_pos)
	{
		this->position = _pos.GetXMFLOAT3();
	}

	void SetCameraPosition(const DirectX::XMVECTOR &_pos)
	{
		DirectX::XMStoreFloat3(&position, _pos);
	}


	DirectX::XMFLOAT3 GetCameraTarget()
	{
		return this->Target;
	}

	Vector3 GetCameraTarget() const
	{
		return Vector3(this->Target);
	}

	void SetCameraTarget(const DirectX::XMFLOAT3 &_tar)
	{
		this->Target = _tar;
	}

	void SetCameraTarget(const DirectX::XMVECTOR &_tar)
	{
		DirectX::XMStoreFloat3(&Target, _tar);
	}

	void SetCameraTarget(Vector3 &_tar)
	{
		this->Target = _tar.GetXMFLOAT3();
	}
	

	DirectX::XMFLOAT3 GetCameraUp()
	{
		return this->Up;
	}

	Vector3 GetCameraUp() const
	{
		return Vector3(this->Up);
	}

	void SetCameraUp(const DirectX::XMFLOAT3 &_up)
	{
		this->Up = _up;
	}

	void SetCameraUp(const DirectX::XMVECTOR &_up)
	{
		DirectX::XMStoreFloat3(&Up, _up);
	}

	void SetCameraUp(Vector3 &_up)
	{
		this->Up = _up.GetXMFLOAT3();
	}
	

	DirectX::XMMATRIX GetProjection()
	{
		return DirectX::XMLoadFloat4x4(&this->Projection);
	}

	void SetProjection(const DirectX::XMMATRIX &_p)
	{
		DirectX::XMStoreFloat4x4(&this->Projection,_p);
	}
	

	DirectX::XMFLOAT3 GetDefaultForward()
	{
		return this->DefaultForward;
	}


	Vector3 GetDefaultForward() const
	{
		return Vector3(this->DefaultForward);
	}

	DirectX::XMFLOAT3 GetDefaultRight()
	{
		return this->DefaultRight;
	}

	Vector3 GetDefaultRight() const
	{
		return this->DefaultRight;
	}

	DirectX::XMFLOAT3 GetCameraForward()
	{
		return this->camForward;
	}


	Vector3 GetCameraForward() const
	{
		return this->camForward;
	}

	DirectX::BoundingFrustum GetFrustum() const { return m_frustum; }
	void SetFustum(DirectX::BoundingFrustum f)  {  m_frustum = f; }

	void SetCameraForward(const DirectX::XMFLOAT3 &_forw)
	{
		this->camForward = _forw;
	}

	void SetCameraForward(const DirectX::XMVECTOR &_forward)
	{
		DirectX::XMStoreFloat3(&camForward, _forward);
	}

	void SetCameraForward(Vector3 &_forw)
	{
		this->camForward = _forw.GetXMFLOAT3();
	}

	DirectX::XMFLOAT3 GetCameraRight()
	{
		return this->camRight;
	}

	Vector3 GetCameraRight() const
	{
		return this->camRight;
	}

	void SetCameraRight(const DirectX::XMFLOAT3 &_right)
	{
		this->camRight = _right;
	}

	void SetCameraRight(Vector3 &_right)
	{
		this->camRight = _right.GetXMFLOAT3();
	}

	void SetCameraRight(const DirectX::XMVECTOR &_right)
	{
		DirectX::XMStoreFloat3(&camRight, _right);
	}

	DirectX::BoundingOrientedBox GetCameraAABB() const
	{
		return m_cameraAABB;
	}
	void SetCameraAABB(DirectX::BoundingOrientedBox b)
	{
		m_cameraAABB = b;
	}
	void SetCollided(bool b)
	{
		m_isCollided = b;
	}
	bool GetCollided() const
	{
		return m_isCollided;
	}

	std::vector<CollisionModel> GetCollisionObjects() const {
		return m_collisionObjects;
	}

	void AddCollisionObject(CollisionModel box)
	{
		this->m_collisionObjects.push_back(box);
	}

	void UpdateCollisionObject(CollisionModel box)
	{
		int id = 0;
		for (unsigned int i = 0;i < m_collisionObjects.size();i++)
		{
			if (HaveMember(m_collisionObjects, box, &id))
			{
				assert(id >= 0);
				m_collisionObjects.at(id) = box;
				return;
			}
		}
	}
#pragma endregion 

	void UpdateView(DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);
	void UpdateView(Vector3 &eye, Vector3 &target, Vector3 &up);

	void UpdateView(DirectX::XMVECTOR eye, DirectX::XMVECTOR target, DirectX::XMVECTOR up);

	void UpdateView(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 lookat);

	void RefreshProjection();

	bool TestIntersects(DirectX::BoundingOrientedBox b, 
		DirectX::CXMMATRIX invWorld,
		DirectX::XMFLOAT2 * HitPoint2DSpace,
		DirectX::XMFLOAT3 * HitPointLocalSpace,float *dist);

	bool TestIntersectsTriangle(
		DirectX::CXMMATRIX invWorld,DirectX::XMVECTOR v1,
		DirectX::XMVECTOR v2,
		DirectX::XMVECTOR v3);

	DirectX::XMFLOAT3 GetMouseInWorldSpace();

	void UpdateProjection(float& fov);

	float viewLeft = 0;
	float viewRight = 800;
	float viewBottom = 600;
	float viewTop = 0;
	float Near = 2;
	float Far = 100;

	
private:
	
	DirectX::XMFLOAT3							position;
	DirectX::XMFLOAT3							direction;
	DirectX::XMFLOAT3							Up;
	DirectX::XMFLOAT3							Target;
	DirectX::XMFLOAT3							DefaultForward;
	DirectX::XMFLOAT3							DefaultRight;
	DirectX::XMFLOAT3							camForward;
	DirectX::XMFLOAT3							camRight;
	//SkyMapProperties							m_skyMapProperties;
	DirectX::XMFLOAT4							m_renderTargetColor;
	float										m_nearPlane;
	float										m_farPlane;
	float										m_fieldOfViewAngle;
	float										m_orthoGraphicWidth;
	float										m_orthoGraphicHeight;
	ProjectionType								m_projectionType;
	DirectX::XMFLOAT4X4							View;
	DirectX::XMFLOAT4X4							Projection;
	DirectX::BoundingOrientedBox						m_cameraAABB;
	bool										m_isCollided;
	std::vector<CollisionModel>	m_collisionObjects;
	DirectX::BoundingFrustum m_frustum;
	//SkyMap ^							m_skyMap;

private:

	void UpdateProjection(float, float, float, float);
	

	bool HaveMember(std::vector<CollisionModel> vec, CollisionModel item, int * id)
	{
		for (unsigned int i = 0;i < vec.size();i++)

		{
			if (vec[i].name==item.name)
			{
				*id = i;
				return true;
			}
		}
		*id = -1;
		return false;
	}
};