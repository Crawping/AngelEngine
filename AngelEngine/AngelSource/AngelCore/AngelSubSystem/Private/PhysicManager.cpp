#include"../Public/PhysicManager.h"
#include"../../AngelSubSystem/Public/TimeManager.h"
#include"../../AngelSubSystem/Public/SubSystemManager.h"
#include"../../AngelSubSystem/AngelWorldSystem/Public/Actor.h"
#include<assert.h>

btAxisSweep3* sweepBP;
bool AngelCore::AngelSubSystem::PhysicManager::StartUp(...)
{
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btVector3 worldMin(-10, -10, -10);
	btVector3 worldMax(10, 10, 10);
	sweepBP = new btAxisSweep3(worldMin, worldMax);
	m_overlappingPairCache = sweepBP;
	this->m_dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
		m_overlappingPairCache, solver, collisionConfiguration);
	this->m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
	return true;
}

bool AngelCore::AngelSubSystem::PhysicManager::ShutDown()
{
	Clear();

	//delete dynamics world
	delete m_dynamicsWorld;

	//delete broadphase
	delete m_overlappingPairCache;
	return true;
}

void AngelCore::AngelSubSystem::PhysicManager::Clear()
{
	int i;
	for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (unsigned int j = 0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	for (unsigned int i = 0;i < m_characters.size();i++)
	{
		m_dynamicsWorld->removeAction(m_characters[i]);
		delete m_characters[i];
	}
	m_characters.clear();
}

void AngelCore::AngelSubSystem::PhysicManager::Update()
{
	this->m_dynamicsWorld->stepSimulation
		(AngelCore::AngelSubSystem::
			SubSystemManager::GetInstance()->
			GetTimeManager()->GetElapsedSeconds()
		, 10);
	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold = 
			m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		btRigidBody* body1 = btRigidBody::upcast(obA);
		btRigidBody* body2 = btRigidBody::upcast(obB);

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f && body1 && body2)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
				AngelCore::AngelWorldSystem::Actor * actor1 = 
					(AngelCore::AngelWorldSystem::Actor *)
					body1->getUserPointer();
				AngelCore::AngelWorldSystem::Actor * actor2 = 
					(AngelCore::AngelWorldSystem::Actor *)
					body2->getUserPointer();
				actor1->OnCollide(actor2);
				actor2->OnCollide(actor1);
			}
		}
	}
}