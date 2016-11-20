#pragma once
#include"SubSystemBase.h"
#include"btBulletCollisionCommon.h"
#include"btBulletDynamicsCommon.h"
#include<memory>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include"BulletDynamics\Character\btKinematicCharacterController.h"

namespace AngelCore
{
	namespace AngelSubSystem
	{
		class PhysicManager : public ISubSystem
		{
		public:

			bool StartUp(...) override;
			bool ShutDown() override;

			void Update();

			void Clear();

			btDiscreteDynamicsWorld* GetDynamicWorld()
			{
			return this->m_dynamicsWorld;
			}

			btAlignedObjectArray<btCollisionShape*> & GetCollisionShapeList()
			{
			return this->m_collisionShapes;
			}

			void AddRigidBody(btRigidBody * rigid)
			{
				this->m_dynamicsWorld->addRigidBody(rigid);
			}

			void AddCharacter(btKinematicCharacterController * character)
			{
				m_dynamicsWorld->addAction(character);
				m_characters.push_back(character);
			}

			btBroadphaseInterface *GetOverlappingPairCache() const { return m_overlappingPairCache; }

			private:
			btDiscreteDynamicsWorld*						m_dynamicsWorld;
			btAlignedObjectArray<btCollisionShape*>			m_collisionShapes;
			btCollisionWorld *								m_collisionWorld;
			btBroadphaseInterface*							m_overlappingPairCache;
			std::vector<btKinematicCharacterController * >  m_characters;
		};
	}
}
