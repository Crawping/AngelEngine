//
//#include "../../AngelGameObjects/Public/Map2D.h"
//#include"../../../AngelCore/AngelSubSystem/AngelWorldSystem/AngelComponent/Public/Transform.h"
//#include"../../AngelSubSystem/Public/SubSystemManager.h"
//#include"../../../AngelCore/AngelSubSystem/AngelRendering/Public/BaseCamera.h"
//#include<AntTweakBar.h>
//
//Map2D::Map2D() 
//{
//
//}
//
//void Map2D::Update(const AngelCore::AngelSubSystem::TimeManager & timer)
//{
//	/*m_mapMarker->SetPosition(
//		AngelCore::AngelSubSystemResources::WindowProperties::GetWidth()-
//		(AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().m128_f32[0]
//			*
//			m_xInMapSpace
//			+
//			m_mapCenterY),
//		AngelCore::AngelSubSystemResources::WindowProperties::GetHeight() 
//		-(AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().m128_f32[2]
//			*
//			m_yInMapSpace
//			+
//			m_mapCenterX));
//
//	m_mapMarker->SetRotation(
//		DirectX::XMConvertToDegrees(
//			-AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraDirection().m128_f32[1])-180);*/
//
//	/*OutputDebugStringA("\nX : ");
//	OutputDebugStringA(
//		std::to_string(
//			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().m128_f32[0]).c_str());
//	OutputDebugStringA("\nZ : ");
//	OutputDebugStringA(
//		std::to_string(
//			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().m128_f32[2]).c_str());*/
//
//	/*OutputDebugStringA("\nMX : ");
//	OutputDebugStringA(
//		std::to_string(
//			m_mapMarker->GetPosition()->GetX()).c_str());
//	OutputDebugStringA("\nMY : ");
//	OutputDebugStringA(
//		std::to_string(
//			m_mapMarker->GetPosition()->GetY()).c_str());*/
//
//	__super::Update(timer);
//}
//
//void Map2D::Initialize()
//{
//	TwBar *bar = TwNewBar("TweakBar");
//	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar into a DirectX11 application.' "); // Message added to the help bar.
//	int barSize[2] = { 224, 320 };
//	TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
//
//	TwStructMember pointMembers[] = {
//		{ "X", TW_TYPE_FLOAT, offsetof(Vector2, Vector2::m_x), " Min=-100 Max=100 Step=0.01 " },
//		{ "Y", TW_TYPE_FLOAT, offsetof(Vector2, Vector2::m_y), " Min=-100 Max=100 Step=0.01 " }};
//	TwType pointType = TwDefineStruct("POINT", pointMembers, 2, sizeof(Vector2), NULL, NULL);
//
//
//	/* m_mapBackground = new AngelCore::AngelGUI::GUI2D::GUISprite();
//	 m_mapBackground->Initialize();
//
//	 m_mapMarker = new AngelCore::AngelGUI::GUI2D::GUISprite();
//	 m_mapMarker->Initialize();
//	 m_mapMarker->SetBitmapPath(L"dot.png");*/
//
//	/* TwAddVarRW(bar, "angle", TW_TYPE_FLOAT,
//	 	&m_mapMarker->m_rotationAngle, "");
//
//	 TwAddVarRW(bar, "pos x", TW_TYPE_FLOAT,
//		 &m_mapMarker->m_position->m_x, "");
//
//	 TwAddVarRW(bar, "pos y", TW_TYPE_FLOAT,
//		 &m_mapMarker->m_position->m_y, "");
//
//	 TwAddVarRW(bar, "scale x", TW_TYPE_FLOAT,
//		 &m_mapMarker->m_scale->m_x, "Min=-100 Max=100 Step=0.01 ");
//
//	 TwAddVarRW(bar, "scale y", TW_TYPE_FLOAT,
//		 &m_mapMarker->m_scale->m_y, "Min=-100 Max=100 Step=0.01");*/
//
//	 //-23->24 Z
//	 //-3=rect.bottom; 3=ret.top;
//	 //-24->16 X
//	 //2->rect.rght; -2=rect.left;
//	 // 24  462.5
//	 //	-24 587.5
//	 //3.125
//	 //center 525
//
//	 //-23 662.5
//	 //15.7 787.5
//	 //3.94
//	 //center 725
//	 //16.31 23.7
//
//	 //25 18
//	 //575 475
//
//
//
//	 m_widthBounds.x = 16.31;//max
//	 m_widthBounds.y = -23;//min
//
//	 m_heightBounds.x = 25.0;//max
//	 m_heightBounds.y = -18.0f;//min
//
//
//
//
//	
//	/* m_mapMarker->SetScale(0.5, 0.5);
//	 m_mapBackground->SetRectangle(D2D1::RectF(650, 450, 800, 600));*/
//	/* m_mapCenterX = (m_mapBackground->GetRect().right-
//		 (m_mapMarker->GetScale()->GetX()*(m_mapMarker->GetRect().right- m_mapMarker->GetRect().left)))
//		 - (m_mapBackground->GetRect().left +
//			 (m_mapMarker->GetScale()->GetX()*(m_mapMarker->GetRect().right - m_mapMarker->GetRect().left)));
//
//	 float dividerX = (m_mapBackground->GetRect().right -
//		 (m_mapMarker->GetScale()->GetX()*(m_mapMarker->GetRect().right - m_mapMarker->GetRect().left))/2)
//		 - (m_mapBackground->GetRect().left +
//			 (m_mapMarker->GetScale()->GetX()*(m_mapMarker->GetRect().right - m_mapMarker->GetRect().left))/2);
//
//	 float dividerY = (m_mapBackground->GetRect().bottom -
//		 (m_mapMarker->GetScale()->GetY()*(m_mapMarker->GetRect().bottom - m_mapMarker->GetRect().top))/2)
//		 - (m_mapBackground->GetRect().top +
//			 (m_mapMarker->GetScale()->GetY()*(m_mapMarker->GetRect().bottom - m_mapMarker->GetRect().top))/2);
//
//	 m_yInMapSpace = dividerY / (std::abs(m_heightBounds.x) + std::abs(m_heightBounds.y));
//	 m_xInMapSpace =  dividerX / (std::abs(m_widthBounds.x) + std::abs(m_widthBounds.y));
//	 
//	 m_mapCenterY = (m_mapBackground->GetRect().bottom -
//		 (m_mapMarker->GetScale()->GetY()*(m_mapMarker->GetRect().bottom - m_mapMarker->GetRect().top)))
//		 - (m_mapBackground->GetRect().top +
//			 (m_mapMarker->GetScale()->GetY()*(m_mapMarker->GetRect().bottom - m_mapMarker->GetRect().top)));
//	
//	 AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetGUIManager()->AddGUI2DItem(m_mapBackground);
//	 AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetGUIManager()->AddGUI2DItem(m_mapMarker);*/
//}
//
//void Map2D::Render()
//{
//	__super::Render();
//}
