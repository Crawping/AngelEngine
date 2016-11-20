#pragma once

#include"IComponent.h"

#include"../../../../AngelTools/AngelMathHelper/Public/Vector.h"
#include"MeshRenderer.h"

#ifndef _SCRIPT_MANAGER
#include"../../../Public/ScriptManager.h"
#endif

#include"BulletDynamics\Character\btKinematicCharacterController.h"
#include"btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#define __CHARACTER_CONTROLLER

namespace AngelCore
{
	class AngelSubSystem::ScriptManager;

	namespace AngelWorldSystem
	{
		class CharacterController : public IComponent
		{
			friend class AngelSubSystem::ScriptManager;
			friend class Actor;
			friend class MeshRenderer;

		public:

			CharacterController();
			CharacterController(const CharacterController &that);
			CharacterController(CharacterController &&that);
			virtual ~CharacterController();

			CharacterController &operator =(const CharacterController &that);
			CharacterController &operator =(CharacterController &&that);

		public:

			virtual void Initialize(Actor* _owner, ...) override;

			virtual void Update(const AngelSubSystem::TimeManager &timer) override;

			virtual void Render() override;

		public:
			void MoveForward()				{ this->m_moveForward = true; }
			void MoveBackward()				{ this->m_moveBackward = true; };
			void MoveRight()				{ this->m_moveRight = true; }
			void MoveLeft()					{ this->m_moveLeft = true; };
			void SetMoveSpeed(float &speed) { m_moveSpeed = speed; }
			void SetLocalScale(float &x, float &y, float &z);
			void SetHeightOffset(float offset) { this->m_heightOffset = offset; }
			void SetHeightOffset(float &offset) { this->m_heightOffset = offset; }

			float GetHeightOffset() const { return m_heightOffset; }

		public:
			btPairCachingGhostObject* GetrGhostObject() const { return m_ghostObject; }
			btKinematicCharacterController * GetKinamticCharacter() const { return m_character; }

		private:
			btKinematicCharacterController* m_character;
			btPairCachingGhostObject*		m_ghostObject;
			Vector3							m_walkDirection;
			Vector3							m_walkVelocity;
			Vector3							m_walkSpeed;
			btScalar						m_moveSpeed;
			bool							m_moveForward;
			bool							m_moveBackward;
			bool							m_moveLeft;
			bool							m_moveRight;
			btConvexShape*					m_capsule;
			//for correction on ground.
			float							m_heightOffset;
		};
	}
}