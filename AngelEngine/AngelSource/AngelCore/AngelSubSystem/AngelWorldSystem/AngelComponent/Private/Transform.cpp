#include"../Public/Transform.h"
#include"../Public/RigidBody.h"//GetRigidBody
#include"../Public/CharacterController.h"//Getcharactercontroller
#include"../../Public/Actor.h"//GetOnwer
#include"../../../Public/SubSystemManager.h"//GetPhysicManager
#include"../../../Public/PhysicManager.h"//GetDynamicWorld
#include"../../../Public/InputManager.h"//GetDynamicWorld

AngelCore::AngelWorldSystem::Transform::Transform()
{
	this->m_position = new Vector3();
	this->m_Lposition = new Vector3();
	this->m_rotation = new Vector3();
	this->m_scalling = new Vector3(1,1,1);

	DirectX::XMStoreFloat4x4(&this->m_rotationMatrix4x4, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->m_scallingMatrix4x4, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->m_translationMatrix4x4, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->m_worldMatrix4x4, DirectX::XMMatrixIdentity());

	this->DEGREETORADIAN = 0.0174532925f;
}

AngelCore::AngelWorldSystem::Transform::Transform(const Transform &that)
{
	this->m_position = new Vector3();
	this->m_rotation = new Vector3();
	this->m_scalling = new Vector3();

	this->m_position = (that.m_position);
	this->m_rotation = (that.m_rotation);
	this->m_scalling = (that.m_scalling);

	this->DEGREETORADIAN = 0.0174532925f;
}

AngelCore::AngelWorldSystem::Transform::Transform(Transform &&that)
{
	this->m_position = new Vector3();
	this->m_rotation = new Vector3();
	this->m_scalling = new Vector3();

	this->m_position = std::move(that.m_position);
	this->m_rotation = std::move(that.m_rotation);
	this->m_scalling = std::move(that.m_scalling);

	this->DEGREETORADIAN = 0.0174532925f;
}

AngelCore::AngelWorldSystem::Transform & AngelCore::AngelWorldSystem::Transform::operator=(Transform &&that)
{
	this->m_position = new Vector3();
	this->m_rotation = new Vector3();
	this->m_scalling = new Vector3();

	this->m_position = std::move(that.m_position);
	this->m_rotation = std::move(that.m_rotation);
	this->m_scalling = std::move(that.m_scalling);

	this->DEGREETORADIAN = 0.0174532925f;

	return *this;
}

AngelCore::AngelWorldSystem::Transform & AngelCore::AngelWorldSystem::Transform::operator=(const Transform &that)
{
	this->m_position = new Vector3();
	this->m_rotation = new Vector3();
	this->m_scalling = new Vector3();

	this->m_position = (that.m_position);
	this->m_rotation = (that.m_rotation);
	this->m_scalling = (that.m_scalling);

	
	this->DEGREETORADIAN = 0.0174532925f;

	return *this;
}

AngelCore::AngelWorldSystem::Transform::~Transform()
{
	delete this->m_position;
	delete this->m_scalling;
	delete this->m_rotation;
}

void AngelCore::AngelWorldSystem::Transform::Initialize(Actor* _owner, ...)
{
	this->m_owner = _owner;
}

void AngelCore::AngelWorldSystem::Transform::Update(const AngelSubSystem::TimeManager &timer)
{
	using namespace DirectX;
	XMMATRIX rotation;
	if (GetOwner()->GetComponent<AngelCore::AngelWorldSystem::RigidBody>())
	{
		btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
			->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
				GetOwner()->GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->GetRigidBody()[0].index];

		
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform transformation;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(transformation);
		}
		else
		{
			transformation = obj->getWorldTransform();
		}
		//if (transformation.getOrigin().getX() - m_position->GetX() != 0)
		//{
		//	transformation.getOrigin().setX(transformation.getOrigin().getX() + (transformation.getOrigin().getX()
		//		- m_position->GetX()));
		//	body->setWorldTransform(transformation);
		//}
		//if (transformation.getOrigin().getZ() - m_position->GetZ() != 0)
		//{
		//	transformation.getOrigin().setZ(transformation.getOrigin().getZ() + (transformation.getOrigin().getZ()
		//		- m_position->GetZ()));
		//	body->setWorldTransform(transformation);
		//}
		//if (transformation.getOrigin().getY() - m_position->GetY() != 0)
		//{
		//	
		//	transformation.getOrigin().setY(transformation.getOrigin().getY() + (transformation.getOrigin().getY()
		//		- m_position->GetY()));
		//	btTransform transform = body->getCenterOfMassTransform();
		//	transform.setOrigin(btVector3(transformation.getOrigin().getX(),
		//		transformation.getOrigin().getY(),
		//		transformation.getOrigin().getZ()));
		//	body->setCenterOfMassTransform(transform);
		//	//body->setWorldTransform(transformation);
		//}
		//
		if (strcmp(GetOwner()->GetName(), "Ball") == 0)
		{
			/*OutputDebugStringA("\nPhysic Y:");
			OutputDebugStringA(std::to_string(transformation.getOrigin().getY()).c_str());
			OutputDebugStringA("\Rot x:");
			OutputDebugStringA(std::to_string(transformation.getRotation().getX()).c_str());
			OutputDebugStringA("\Rot Y:");
			OutputDebugStringA(std::to_string(transformation.getRotation().getY()).c_str());*/
		}
		
		/*transformation.setOrigin(btVector3(
			transformation.getOrigin().getX()+(transformation.getOrigin().getX()-m_position->GetX()),
			transformation.getOrigin().getY(),
			transformation.getOrigin().getZ()));*/
			//body->setWorldTransform(transformation);
		m_position->SetX(transformation.getOrigin().getX());
		m_position->SetY(transformation.getOrigin().getY());
		m_position->SetZ(transformation.getOrigin().getZ());

		float roll =atan2f(2 * transformation.getRotation().getY()*transformation.getRotation().getW() - 2 
			* transformation.getRotation().getX()*transformation.getRotation().getZ(),
			1 - 2 * transformation.getRotation().getY()*transformation.getRotation().getY()
			- 2 * transformation.getRotation().getZ()*transformation.getRotation().getZ());
		float pitch = atan2f(2 * transformation.getRotation().getX()*transformation.getRotation().getW() 
			- 2 * transformation.getRotation().getY()*transformation.getRotation().getZ(),
			1 - 2 * transformation.getRotation().getX()*transformation.getRotation().getX() - 2 
			* transformation.getRotation().getZ()*transformation.getRotation().getZ());
		float yaw = asinf(2 * transformation.getRotation().getX()*transformation.getRotation().getY() +
			2 * transformation.getRotation().getZ()*transformation.getRotation().getW());

		rotation = XMMatrixRotationQuaternion(DirectX::XMVectorSet(transformation.getRotation().getX(),
			transformation.getRotation().getY(), transformation.getRotation().getZ()
			, transformation.getRotation().getW()));
		DirectX::XMStoreFloat4x4(&m_rotationMatrix4x4, rotation);
	}
	if (GetOwner()->GetComponent<AngelCore::AngelWorldSystem::CharacterController>())
	{
		
		btTransform transformation;

		//look at the vehicle
		transformation = GetOwner()->GetComponent<AngelCore::AngelWorldSystem::CharacterController>()
			->GetrGhostObject()->getWorldTransform();
		int a = 5;
		m_position->SetX(transformation.getOrigin().getX());
		m_position->SetY(transformation.getOrigin().getY()+ 
			GetOwner()->GetComponent<AngelCore::AngelWorldSystem::CharacterController>()->GetHeightOffset());
		m_position->SetZ(transformation.getOrigin().getZ());

		/*float roll = atan2f(2 * transformation.getRotation().getY()*transformation.getRotation().getW() - 2
			* transformation.getRotation().getX()*transformation.getRotation().getZ(),
			1 - 2 * transformation.getRotation().getY()*transformation.getRotation().getY()
			- 2 * transformation.getRotation().getZ()*transformation.getRotation().getZ());
		float pitch = atan2f(2 * transformation.getRotation().getX()*transformation.getRotation().getW()
			- 2 * transformation.getRotation().getY()*transformation.getRotation().getZ(),
			1 - 2 * transformation.getRotation().getX()*transformation.getRotation().getX() - 2
			* transformation.getRotation().getZ()*transformation.getRotation().getZ());
		float yaw = asinf(2 * transformation.getRotation().getX()*transformation.getRotation().getY() +
			2 * transformation.getRotation().getZ()*transformation.getRotation().getW());

		rotation = XMMatrixRotationQuaternion(DirectX::XMVectorSet(transformation.getRotation().getX(),
			transformation.getRotation().getY(), transformation.getRotation().getZ()
			, transformation.getRotation().getW()));

		rotation = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		DirectX::XMStoreFloat4x4(&m_rotationMatrix4x4, rotation);*/
	}



	DirectX::XMStoreFloat4x4(&this->m_scallingMatrix4x4, DirectX::XMMatrixScaling(
		this->m_scalling->GetX(), this->m_scalling->GetY(), this->m_scalling->GetZ()));
	XMMATRIX scalling = DirectX::XMLoadFloat4x4(&this->m_scallingMatrix4x4);

	DirectX::XMStoreFloat4x4(&this->m_translationMatrix4x4, DirectX::XMMatrixTranslation(
		this->m_position->GetX(), this->m_position->GetY(), this->m_position->GetZ()));
	XMMATRIX translation = DirectX::XMLoadFloat4x4(&this->m_translationMatrix4x4);

	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(this->m_rotation->GetX());
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(this->m_rotation->GetY());
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(this->m_rotation->GetZ());

	if (!GetOwner()->GetComponent<AngelCore::AngelWorldSystem::RigidBody>()) 
	{
		DirectX::XMStoreFloat4x4(&this->m_rotationMatrix4x4, X*Y*Z);
		rotation = DirectX::XMLoadFloat4x4(&this->m_rotationMatrix4x4);
	}

	DirectX::XMStoreFloat4x4(&this->m_worldMatrix4x4, DirectX::XMMatrixMultiply(scalling, rotation));
	XMMATRIX world = DirectX::XMLoadFloat4x4(&this->m_worldMatrix4x4);

	DirectX::XMStoreFloat4x4(&this->m_worldMatrix4x4, DirectX::XMMatrixMultiply(world, translation));
}

void AngelCore::AngelWorldSystem::Transform::Render()
{
	
}