#pragma once

#include"IComponent.h"

#include"../../../../AngelTools/AngelMathHelper/Public/Vector.h"
#include"MeshRenderer.h"

#ifndef _SCRIPT_MANAGER
#include"../../../Public/ScriptManager.h"
#endif

#define __TRANSFORM

namespace AngelCore
{
	class AngelSubSystem::ScriptManager;

	namespace AngelWorldSystem
	{
		class Transform : public IComponent
		{
			friend class AngelSubSystem::ScriptManager;
			friend class Actor;
			friend class MeshRenderer;

		public:

			Transform();
			Transform(const Transform &that);
			Transform(Transform &&that);
			virtual ~Transform();

			Transform &operator =(const Transform &that);
			Transform &operator =(Transform &&that);

			Vector3 * GetPosition()					  { return this->m_position; }
			Vector3 * GetRotation()					  { return this->m_rotation; }
			Vector3 * GetScalling()					  { return this->m_scalling; }
			DirectX::XMMATRIX	GetWorldMatrix()	  { return DirectX::XMLoadFloat4x4(&this->m_worldMatrix4x4); }
			DirectX::XMMATRIX   GetRotationMatrix()	  { return DirectX::XMLoadFloat4x4(&this->m_rotationMatrix4x4); }
			DirectX::XMMATRIX   GetScallingMatrix()	  { return DirectX::XMLoadFloat4x4(&this->m_scallingMatrix4x4); }
			DirectX::XMMATRIX   GetTranslationMatrix(){ return DirectX::XMLoadFloat4x4(&this->m_translationMatrix4x4); }

		public:

			virtual void Initialize(Actor* _owner, ...) override;

			virtual void Update(const AngelSubSystem::TimeManager &timer) override;

			virtual void Render() override;

		private:

			Vector3 *															m_position;
			Vector3 *															m_Lposition;
			Vector3 *															m_scalling;
			Vector3 *															m_rotation;
			DirectX::XMFLOAT4X4													m_worldMatrix4x4;
			DirectX::XMFLOAT4X4													m_translationMatrix4x4;
			DirectX::XMFLOAT4X4													m_rotationMatrix4x4;
			DirectX::XMFLOAT4X4													m_scallingMatrix4x4;
			float																DEGREETORADIAN;
		};
	}
}