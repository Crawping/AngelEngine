//// ***********************************************************************
//// FileName         : MainCamera.h
//// Author           : Peyman Tahghighi
////
//// ***********************************************************************
////     Copyright (c) Peyman Tahghighi . All rights reserved.
////
//// <summary> Floating Camera  </summary>
//// ***********************************************************************
//
//#pragma once
//#include"../../AngelSubSystem/AngelWorldSystem/Public/Actor.h"
//#include"../../AngelSubSystem/Public/InputManager.h"
//#include"../../AngelSubSystem/AngelRendering/Public/SkyMap.h"
//#include"../../AngelSubSystem/AngelRendering/Public/DebugCollisionShape.h"
//#include<DirectXMath.h>
//
//
//class MainCamera : public AngelCore::AngelWorldSystem::Actor
//{
//public:
//	virtual void Render() override;
//
//	virtual void Update(const AngelCore::AngelSubSystem::TimeManager & timer) override;
//
//	virtual void Initialize() override;
//
//	MainCamera();
//
//private:
//	DirectX::XMFLOAT2															lastPointerPos;
//	DirectX::XMFLOAT2															currentPointerPos;
//	DirectX::XMFLOAT2															m_lastPosition;
//	DirectX::XMFLOAT2															m_curPosition;
//	float																		m_camRotationSpeed;
//	float																		m_camMovmentSpeed;
//	float																		camYaw;
//	float																		camPitch;
//	float																		m_lastXPosition;
//	float																		m_lastZPosition;
//	float																		m_lastYPosition;
//	AngelCore::AngelRendering::DebugCollisionShape*								m_debugCollisionShape;
//	bool																		m_updateAABB;
//	//w=0 a=1 s=2 d=3
//	DirectX::XMVECTOR m_lastCameraPosition;
//};
//
