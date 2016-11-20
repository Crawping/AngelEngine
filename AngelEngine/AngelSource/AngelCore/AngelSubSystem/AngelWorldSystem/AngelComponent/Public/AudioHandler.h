#pragma once

#include"IComponent.h"

#ifndef _SCRIPT_MANAGER
#include"../../../Public/ScriptManager.h"
#endif

#define __AUDIO_HANDLER

namespace AngelCore
{
	class AngelSubSystem::ScriptManager;

	namespace AngelWorldSystem
	{
		class AudioHandler : public IComponent
		{
			struct AudioClip
			{
				AudioClip() : soundEffect(nullptr), soundEffectInstance(nullptr),paused(false) {}
				const char * name;
				std::unique_ptr<DirectX::SoundEffect>                   soundEffect;
				std::unique_ptr<DirectX::SoundEffectInstance>           soundEffectInstance;
				bool paused;
			};
			friend class AngelSubSystem::ScriptManager;
			friend class Actor;
			friend class MeshRenderer;

		public:

			AudioHandler();
			AudioHandler(const AudioHandler &that);
			AudioHandler(AudioHandler &&that);
			virtual ~AudioHandler();

			AudioHandler &operator =(const AudioHandler &that);
			AudioHandler &operator =(AudioHandler &&that);
		public:

			virtual void Initialize(Actor* _owner, ...) override;

			virtual void Update(const AngelSubSystem::TimeManager &timer) override;

			virtual void Render() override;

		public:
			void Load(std::string &name);
			void Play(bool &loop,int &index);
			void Pause(int &index);
			void Resume(int &index);
			void SetPan(float &pan, int &index);
			void SetPitch(float &pitch, int &index);
			void SetVolume(float &vol, int &index);
			void Stop(int &index);


		private:
			std::vector<AudioClip*>			m_audioClips;
		};
	}
}