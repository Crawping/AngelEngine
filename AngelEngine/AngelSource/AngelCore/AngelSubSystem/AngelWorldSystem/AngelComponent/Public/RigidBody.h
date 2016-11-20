#pragma once
#include"IComponent.h"
#include"../../../../AngelTools/AngelMathHelper/Public/Vector.h"
#ifndef _SCRIPT_MANAGER
#define _SCRIPT_MANAGER
#include"../../../Public/ScriptManager.h"
#endif
#include<btBulletCollisionCommon.h>
#include<btBulletDynamicsCommon.h>
#include"../../../AngelRendering/Public/DebugCollisionShape.h"

#define __RIGIDBODY

namespace AngelCore
{
	class AngelSubSystem::ScriptManager;
	namespace AngelWorldSystem
	{
		class RigidBody : public IComponent
		{
			struct MeshRigidData
			{
				btRigidBody *			rigidBody;
				bool					isDynamic;
				btDefaultMotionState*	motionState;
				btCollisionShape *		collisionShape;
				int						index;
			};

			friend class AngelSubSystem::ScriptManager;
			friend class Actor;
			friend class MeshRenderer;

		public:

			enum COLLISION_SHAPE
			{
				BOX = 0,
				SPHERE,
				CONVEX
			};
		public:
			RigidBody();
			RigidBody(const RigidBody &that);
			RigidBody(RigidBody &&that);
			virtual ~RigidBody();
			RigidBody &operator =(const RigidBody &that);
			RigidBody &operator =(RigidBody &&that);

		public:

			virtual void Initialize(Actor* _owner, ...) override;
			virtual void Update(const AngelSubSystem::TimeManager &timer) override;
			virtual void Render() override;
			std::vector<MeshRigidData>  GetRigidBody() const		{ return m_meshRigidData; }


		public:
			void SetMass(float mass);
			void ApplyForce(float& x, float &y, float &z);
			void ApplyImpluse(float& x, float &y, float &z);
			void ApplyTorque(float& x, float &y, float &z);
			void ApplyTorqueImpluse(float& x, float &y, float &z);
			void SetRestitution(float &restitution);
			//if we use rigidbody we should use this function for adjust position.
			void SetPosition(Vector3 &pos);
			void SetPosition(float &x,float &y,float &z);
			void SetLinearVelocity(Vector3 &vec);
			void SetLinearVelocity(float &x,float &y,float &z);
			void SetAngularVelocity(Vector3 &vec);
			void SetAngularVelocity(float &x, float &y, float &z);
			//if we use rigidbody we should use this function for adjust rotation.
			void SetRotation(Vector3 &rot);
			//for register inside script
			void SetCollisionFlag(int &flag);
			void SetActive(bool b);

			int GetCollisionFlag() const;

			void SetCollisionFlag(int flag);

		private:

			std::vector<MeshRigidData>				m_meshRigidData;
			AngelRendering::DebugCollisionShape * m_debugCollisionShape;
			DirectX::BoundingOrientedBox			m_aabb;
			btBvhTriangleMeshShape *tmpdata;
			COLLISION_SHAPE				m_shape;
		};
	}
}