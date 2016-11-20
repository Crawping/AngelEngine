#include"../Public/LightRenderer.h"
#include"../Public/Transform.h"
#include"../../../Public/SubSystemManager.h"//GetPhysicManager
#include"../../../Public/PhysicManager.h"//GetDynamicWorld
#include"../../../Public/InputManager.h"//GetDynamicWorld
#include"../../../AngelRendering/Public/LightManager.h"

AngelCore::AngelWorldSystem::LightRenderer::LightRenderer()
{
	
}

AngelCore::AngelWorldSystem::LightRenderer::LightRenderer(const LightRenderer &that)
{
	
}

AngelCore::AngelWorldSystem::LightRenderer::LightRenderer(LightRenderer &&that)
{
	
}

AngelCore::AngelWorldSystem::LightRenderer & AngelCore::AngelWorldSystem::LightRenderer::operator=(LightRenderer &&that)
{
	return *this;
}

AngelCore::AngelWorldSystem::LightRenderer & AngelCore::AngelWorldSystem::LightRenderer::operator=(const LightRenderer &that)
{

	return *this;
}

AngelCore::AngelWorldSystem::LightRenderer::~LightRenderer()
{
	
}

void AngelCore::AngelWorldSystem::LightRenderer::Initialize(Actor* _owner, ...)
{
	m_PositionWS = DirectX::XMFLOAT3(0,1,0);
	m_castShadow = false;
	m_DirectionWS = DirectX::XMFLOAT4(1, 1,1,1);
	m_Color = DirectX::XMFLOAT4(0, 0, 0,0);
	m_SpotlightAngle = 0;
	m_Range = 50;
	m_Intensity = 1;
	m_Enabled = true;
	m_Type = 0;
	m_CameraPosition = DirectX::XMFLOAT3(0, 0, 0);
	m_index = AngelRendering::LightManager::GetInstance()->GetIndex();
	AngelRendering::LightManager::GetInstance()->AddLight(this);
	this->m_owner = _owner;
}

void AngelCore::AngelWorldSystem::LightRenderer::Update(const AngelSubSystem::TimeManager &timer)
{
	AngelRendering::LightManager::GetInstance()->UpdateLight(m_index, this);
	m_PositionWS = m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetXMFLOAT3();
	m_DirectionWS =
		DirectX::XMFLOAT4(
			m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX()
			, m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY()
			,m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetZ(),
			1.0f);

}

void AngelCore::AngelWorldSystem::LightRenderer::Render()
{

}