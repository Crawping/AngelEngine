#include"../Public/RigidBody.h"
#include"../Public/MeshRenderer.h"
#include"../../../Public/SubSystemManager.h"//Pointer to physic manager
#include"../../../Public/PhysicManager.h"//Pointer to physic manager
#include"../../../Public/InputManager.h"//for testing
#include"../../Public/Actor.h"
#include"../Public/Component.h"
#include"../Public/Transform.h"
#include"../Public/Model.h"
#include"../Public/MeshRenderer.h"
#include"BulletCollision\CollisionShapes\btShapeHull.h"
#include"BulletCollision\Gimpact\btGImpactShape.h"

AngelCore::AngelWorldSystem::RigidBody::RigidBody()
{

}

AngelCore::AngelWorldSystem::RigidBody::RigidBody(const RigidBody &that)
{
	
}


AngelCore::AngelWorldSystem::RigidBody::RigidBody(RigidBody &&that)
{
	
}

AngelCore::AngelWorldSystem::RigidBody & AngelCore::AngelWorldSystem::RigidBody::operator=(RigidBody &&that)
{
	return *this;
}

AngelCore::AngelWorldSystem::RigidBody & AngelCore::AngelWorldSystem::RigidBody::operator=(const RigidBody &that)
{
	return *this;
}

AngelCore::AngelWorldSystem::RigidBody::~RigidBody()
{
	
}

void AngelCore::AngelWorldSystem::RigidBody::Initialize(Actor* _owner, ...)
{
	va_list args;
	va_start(args, _owner);
	SetOwner(_owner);
	COLLISION_SHAPE shape = va_arg(args, COLLISION_SHAPE);
	double size = va_arg(args, double);
	m_shape = shape;

	assert(GetOwner()->GetComponent<MeshRenderer>());
	std::vector<ModelMesh*> vec =  GetOwner()->GetComponent<MeshRenderer>()->GetAllMeshes();
	m_meshRigidData.resize(vec.size());
	if (shape == BOX)
	{
		//Set default initial state
		for (unsigned int i = 0;i < vec.size();i++)
		{
			m_meshRigidData[i].collisionShape = new btBoxShape(btVector3(20,0.05,20));
			
		}
	}
	if (shape == SPHERE)
	{
		//Set default initial state
		for (unsigned int i = 0;i < vec.size();i++)
		{
			m_meshRigidData[i].collisionShape = new btSphereShape(size);
		}
	}
	if (shape == CONVEX)
	{
		
		//Set default initial state
		for (unsigned int i = 0;i < vec.size();i++)
		{
			btScalar * points = new btScalar[vec[i]->pointsSize*3];
			btConvexHullShape *tmp = new btConvexHullShape();
			for (unsigned int j = 0;j < vec[i]->pointsSize;j++)
			{
				points[j * 3 + 0] = vec[i]->points[j].x;
				points[j * 3 + 1] = vec[i]->points[j].y;
				points[j * 3 + 2] = vec[i]->points[j].z;
			}
			btTriangleIndexVertexArray * data =
				new btTriangleIndexVertexArray(vec[i]->indicesSize / 3,
					(int*)vec[i]->indciesInt.data(), 3 * sizeof(int)
					, vec[i]->pointsSize, points, 3 * sizeof(btScalar));
			btBvhTriangleMeshShape * d = new btBvhTriangleMeshShape(data, false, false);
			d->buildOptimizedBvh();
			d->usesQuantizedAabbCompression();
			m_meshRigidData[i].collisionShape = d;
			m_meshRigidData[i].collisionShape->setMargin(0.5f);
			m_debugCollisionShape = new AngelRendering::DebugCollisionShape();
			m_debugCollisionShape->Create();
		}
	}

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
	else
	{
		startTransform.setOrigin(btVector3(0, 0, 0));
		startTransform.setRotation(btQuaternion(0, 0, 0, 0));
	}

	btScalar	mass(1.f);

	//suppose shape is dynamic.
	btVector3 localInertia(0, 0, 0);
	for (unsigned int i = 0;i < vec.size();i++)
	{
		//m_meshRigidData[i].collisionShape->calculateLocalInertia(mass, localInertia);
		m_meshRigidData[i].collisionShape->setLocalScaling(btVector3(
			GetOwner()->GetComponent<Transform>()
			->GetScalling()->GetX(),
			GetOwner()->GetComponent<Transform>()
			->GetScalling()->GetY(),
			GetOwner()->GetComponent<Transform>()
			->GetScalling()->GetZ()));

		m_meshRigidData[i].motionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_meshRigidData[i].motionState
			, m_meshRigidData[i].collisionShape, localInertia);
		m_meshRigidData[i].rigidBody = new btRigidBody(rbInfo);
		m_meshRigidData[i].rigidBody->setUserPointer(_owner);
		
		m_meshRigidData[i].isDynamic = true;
		m_meshRigidData[i].index = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->
			GetPhysicManager()->GetDynamicWorld()->getNumCollisionObjects();
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->
			GetPhysicManager()->AddRigidBody(m_meshRigidData[i].rigidBody);
		//SetCollisionFlag(btCollisionObject::CF_STATIC_OBJECT);
	}
}

void AngelCore::AngelWorldSystem::RigidBody::Update(const AngelSubSystem::TimeManager &timer)
{	
	
}

void AngelCore::AngelWorldSystem::RigidBody::Render()
{

}

void AngelCore::AngelWorldSystem::RigidBody::SetMass(float mass)
{
	for (auto & member : m_meshRigidData)
	{
		btVector3 interia = btVector3(0, 0, 0);
		if(m_shape==0 || m_shape==1)
			member.collisionShape->calculateLocalInertia(mass, interia);
		member.rigidBody->setMassProps(mass, interia);
		
	}
}

void AngelCore::AngelWorldSystem::RigidBody::ApplyForce(float& x, float &y, float &z)
{
	for (auto & member : m_meshRigidData)
	{
		member.rigidBody->activate(true);
		member.rigidBody->applyCentralForce
			(btVector3(x, y, z));
	
	}
}

void AngelCore::AngelWorldSystem::RigidBody::ApplyImpluse(float& x, float &y, float &z)
{
	for (auto & member : m_meshRigidData)
	{
		member.rigidBody->activate(true);
		member.rigidBody->applyCentralImpulse
			(btVector3(x, y, z));
	}
}

void AngelCore::AngelWorldSystem::RigidBody::ApplyTorque(float& x, float &y, float &z)
{
	for (auto & member : m_meshRigidData)
	{
		member.rigidBody->activate(true);
		member.rigidBody->applyTorque(btVector3(x, y, z));
	}
}

void AngelCore::AngelWorldSystem::RigidBody::ApplyTorqueImpluse(float& x, float &y, float &z)
{
	for (auto & member : m_meshRigidData)
	{
		member.rigidBody->activate(true);
		member.rigidBody->applyTorqueImpulse(btVector3(x, y, z));
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetRestitution(float& restitution)
{
	for (auto & member : m_meshRigidData)
	{
		member.rigidBody->activate(true);
		member.rigidBody->setRestitution(restitution);
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetPosition(Vector3 &pos)
{
	
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
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
	transformation.setOrigin(btVector3(pos.GetX(), pos.GetY(), pos.GetZ()));
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetX(pos.GetX());
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetY(pos.GetY());
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetZ(pos.GetZ());
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setWorldTransform(transformation);
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetPosition(
	float &x,float &y,float &z)
{

	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
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
	transformation.setOrigin(btVector3(x, y, z));
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetX(x);
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetY(y);
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetZ(z);
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setWorldTransform(transformation);
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetRotation(Vector3 &rot)
{

	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
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
	btQuaternion quat;
	quat.setEuler(
		rot.GetX(),
		rot.GetY(),
		rot.GetZ());
	transformation.setRotation(quat);
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetX(rot.GetX());
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetY(rot.GetY());
	GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetZ(rot.GetZ());
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setWorldTransform(transformation);
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetLinearVelocity(Vector3 &vel)
{
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
	btRigidBody* body = btRigidBody::upcast(obj);
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setLinearVelocity(btVector3(vel.GetX(),vel.GetY(),vel.GetZ()));
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetLinearVelocity(float &x,float &y,float &z)
{
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
	btRigidBody* body = btRigidBody::upcast(obj);
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setLinearVelocity(btVector3(x,y,z));
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetAngularVelocity(Vector3 &vel)
{
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
	btRigidBody* body = btRigidBody::upcast(obj);
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setAngularVelocity(btVector3(vel.GetX(), vel.GetY(), vel.GetZ()));
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetAngularVelocity(float &x, float &y, float &z)
{
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
	btRigidBody* body = btRigidBody::upcast(obj);
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setAngularVelocity(btVector3(x, y, z));
	}
}
void AngelCore::AngelWorldSystem::RigidBody::SetActive(bool b)
{
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
	btRigidBody* body = btRigidBody::upcast(obj);
	for (auto & member : m_meshRigidData)
	{
		body->activate(b);
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetCollisionFlag(int &flag)
{
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
	btRigidBody* body = btRigidBody::upcast(obj);
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setCollisionFlags(flag);
	}
}

void AngelCore::AngelWorldSystem::RigidBody::SetCollisionFlag(int flag)
{
	btCollisionObject* obj = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetPhysicManager()->GetDynamicWorld()->getCollisionObjectArray()[
			m_meshRigidData[0].index];
	btRigidBody* body = btRigidBody::upcast(obj);
	for (auto & member : m_meshRigidData)
	{
		body->activate(true);
		body->setCollisionFlags(flag);
	}
}