#include"../Public/CharacterController.h"
#include"../Public/Transform.h"
#include"../Public/RigidBody.h"//GetRigidBody
#include"../../Public/Actor.h"//GetOnwer
#include"../../../Public/SubSystemManager.h"//GetPhysicManager
#include"../../../Public/PhysicManager.h"//GetDynamicWorld
#include"../../../Public/InputManager.h"//GetDynamicWorld
#include "btBulletDynamicsCommon.h"


AngelCore::AngelWorldSystem::CharacterController::CharacterController()
{
	m_walkDirection = Vector3(0, 0, 0);
	m_walkVelocity = Vector3(0, 0, 0);
	m_walkSpeed = Vector3(0, 0, 0);
	m_moveForward = false;
	m_moveBackward = false;
	m_moveRight = false;
	m_moveLeft = false;
}

AngelCore::AngelWorldSystem::CharacterController::CharacterController(const CharacterController &that)
{
	
}

AngelCore::AngelWorldSystem::CharacterController::CharacterController(CharacterController &&that)
{
	
}

AngelCore::AngelWorldSystem::CharacterController & AngelCore::AngelWorldSystem::CharacterController::operator=(CharacterController &&that)
{
	

	return *this;
}

AngelCore::AngelWorldSystem::CharacterController & AngelCore::AngelWorldSystem::CharacterController::operator=(const CharacterController &that)
{
	

	return *this;
}

AngelCore::AngelWorldSystem::CharacterController::~CharacterController()
{
	
}

void AngelCore::AngelWorldSystem::CharacterController::Initialize(Actor* _owner, ...)
{
	m_owner = _owner;
	btTransform startTransform;
	if (GetOwner())
	{
		//Set start transform for physics
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(GetOwner()->GetComponent<Transform>()
			->GetPosition()->GetX(),
			GetOwner()->GetComponent<Transform>()
			->GetPosition()->GetY(),
			GetOwner()->GetComponent<Transform>()
			->GetPosition()->GetZ()));

		btQuaternion quat;
		quat.setEuler(GetOwner()->GetComponent<Transform>()
			->GetRotation()->GetY(),
			GetOwner()->GetComponent<Transform>()
			->GetRotation()->GetX(),
			GetOwner()->GetComponent<Transform>()
			->GetRotation()->GetZ());
		startTransform.setRotation(quat);
	}
	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);

	AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()
		->GetOverlappingPairCache()
		->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btScalar characterHeight = 6.15f;
	btScalar characterWidth = 1.75f;
	m_capsule = new btCapsuleShape(characterWidth, characterHeight);
	//m_capsule->setLocalScaling(btVector3(0, 0, 0));
	m_ghostObject->setCollisionShape(m_capsule);
	m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT
		| btCollisionObject::CF_KINEMATIC_OBJECT);

	btScalar stepHeight = btScalar(0.85f);
	m_character = new btKinematicCharacterController(m_ghostObject, m_capsule, stepHeight);
	AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()->GetDynamicWorld()->
		addCollisionObject
		(m_ghostObject, btBroadphaseProxy::CharacterFilter,
			btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()->AddCharacter(m_character);

	AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()->GetDynamicWorld()
		->getBroadphase()->getOverlappingPairCache()->
		cleanProxyFromPairs(m_ghostObject->getBroadphaseHandle(), 
			AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()->GetDynamicWorld()->
			getDispatcher());
	m_character->reset(AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()->GetDynamicWorld());
	m_ghostObject->setUserPointer(_owner);
	m_moveSpeed = 0.1f;
	m_heightOffset = 0.0f;
}

void AngelCore::AngelWorldSystem::CharacterController::Update(const AngelSubSystem::TimeManager &timer)
{
	btTransform xform;
	xform = m_ghostObject->getWorldTransform();
	btVector3 upDir = xform.getBasis()[1];
	btVector3 strafeDir = xform.getBasis()[0];
	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.normalize();
	upDir.normalize();
	strafeDir.normalize();
	
	btVector3 walkDirection = btVector3(
		0, 0
		, 0.0);
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY();
	btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
	btQuaternion quat;
	quat.setEuler(
		-GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY()
		, 0, 0);
	orn = btMatrix3x3(quat);
	m_ghostObject->getWorldTransform().setBasis(orn);

	if (m_moveForward)
	{
		walkDirection += forwardDir;
	}
	if (m_moveBackward)
	{
		walkDirection -= forwardDir;
	}
	if (m_moveRight)
	{
		walkDirection += strafeDir;
	}
	if (m_moveLeft)
	{
		walkDirection -= strafeDir;
	}
	m_character->setWalkDirection(walkDirection*m_moveSpeed);

	//rest values
	m_moveForward = false;
	m_moveBackward = false;
	m_moveLeft = false;
	m_moveRight = false;
}

void AngelCore::AngelWorldSystem::CharacterController::Render()
{

}

void AngelCore::AngelWorldSystem::CharacterController::SetLocalScale(float &x, float &y, float &z)
{
	m_character->getGhostObject()->getCollisionShape()->setLocalScaling(btVector3(x, y, z));
}