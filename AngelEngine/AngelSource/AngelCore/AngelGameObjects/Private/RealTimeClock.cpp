//
//#include "../../AngelGameObjects/Public/RealTimeClock.h"
//#include"../../../AngelCore/AngelSubSystem/AngelWorldSystem/AngelComponent/Public/Transform.h"
//#include"../../../AngelCore/AngelSubSystem/Public/TimeManager.h"
//#define DEGREE_SECOND 6.0f
//#define DEGREE_MINUTE  6.0f
//#define DEGREE_HOUR    30.0f
//
//RealTimeClock::RealTimeClock() : m_useRegularTime(true)
//{
//
//}
//
//void RealTimeClock::Update(const AngelCore::AngelSubSystem::TimeManager & timer)
//{
//	
//	if (!m_useRegularTime)
//	{
//
//	}
//	else
//	{
//		int curTime = timer.GetTotalSeconds();
//		curTime = curTime % 86400;
//		int hour = 0;
//		int min = 0;
//		int sec = 0;
//		GetCurrentTimeHMS(curTime, &hour, &min, &sec);
//		GetComponent<AngelCore::AngelWorldSystem::MeshRenderer>()
//			->GetSubMeshByName(L"Sekunden")->GetTransform()->GetRotation()->SetX(DirectX::XMConvertToRadians
//			(-DEGREE_SECOND*sec));
//		GetComponent<AngelCore::AngelWorldSystem::MeshRenderer>()
//			->GetSubMeshByName(L"Minuten")->GetTransform()->GetRotation()->SetX(DirectX::XMConvertToRadians
//			(-DEGREE_MINUTE*min));
//		GetComponent<AngelCore::AngelWorldSystem::MeshRenderer>()
//			->GetSubMeshByName(L"Stunden")->GetTransform()->GetRotation()->SetX(DirectX::XMConvertToRadians
//			(-DEGREE_HOUR*hour));
//	}
//
//	__super::Update(timer);
//}
//
//void RealTimeClock::Initialize()
//{
//	m_currentTime = 0.0f;
//}
//
//void RealTimeClock::Render()
//{
//	__super::Render();
//}
//
//void RealTimeClock::GetCurrentTimeHMS(int curTime,int * hour, int *minute, int * second)
//{
//	//HOUR
//	if (curTime / 3600 > 0)
//	{
//		*hour = curTime / 3600;
//		curTime = curTime % 3600;
//	}
//
//	//MINUTE
//	if (curTime / 60 > 0)
//	{
//		*minute = curTime / 60;
//		curTime = curTime % 60;
//	}
//	*second = curTime;
//}