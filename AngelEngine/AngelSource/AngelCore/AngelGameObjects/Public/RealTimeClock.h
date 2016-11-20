//// ***********************************************************************
//// FileName         : Clock.h
//// Author           : Peyman Tahghighi
////
//// ***********************************************************************
////     Copyright (c) Peyman Tahghighi . All rights reserved.
////
//// <summary> Real time clock  </summary>
//// ***********************************************************************
//
//#pragma once
//#include"../../AngelSubSystem/AngelWorldSystem/Public/Actor.h"
//#include"../../AngelSubSystem/AngelRendering/Public/DebugCollisionShape.h"
//#include"../../AngelSubSystem/AngelWorldSystem/AngelComponent/Public/MeshRenderer.h"
//#include<DirectXMath.h>
//
//
//class RealTimeClock : public AngelCore::AngelWorldSystem::Actor
//{
//public:
//	virtual void Render() override;
//
//	virtual void Update(const AngelCore::AngelSubSystem::TimeManager & timer) override;
//
//	virtual void Initialize() override;
//
//	RealTimeClock();
//
//public:
//	//Set this if we don't want to use regular time.
//	void SetCurrentTime(float time)		{ this->m_currentTime = time; }
//	//If true we use timer time,otherwise we use custom time here.
//	void SetUseRegularTime(bool b)		{ this->m_useRegularTime = b; }
//
//private:
//	//Get the current time Hour-Minute-Second
//	void GetCurrentTimeHMS(int curTime,int * hour, int *minute, int * second);
//
//
//private:
//	float										m_currentTime;
//	bool										m_useRegularTime;
//};
//
