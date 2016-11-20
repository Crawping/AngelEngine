#pragma once

namespace AngelCore
{
	namespace AngelWorldSystem
	{

#pragma region Forward Declaration
		class Transform;
		class MeshRenderer;
		class SpriteRenderer;
		class RigidBody;
		class AudioHandler;
		class LightRenderer;
		class VideoRenderer;
		class CharacterController;
		class ParticleRenderer;

#pragma endregion

		class Component
		{
		public:
			Transform *						m_transform;
			MeshRenderer *					m_meshRenderer;
			RigidBody*						m_rigidBody;
			AudioHandler *					m_audioHandler;
			LightRenderer *					m_lightRenderer;
			VideoRenderer*					m_videoRenderer;
			CharacterController *			m_characterController;
			ParticleRenderer *				m_particleRenderer;
		};
	}
}
