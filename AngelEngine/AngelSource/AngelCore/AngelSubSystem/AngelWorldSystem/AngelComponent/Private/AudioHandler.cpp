#include"../Public/AudioHandler.h"
#include"../../Public/Actor.h"//GetOnwer
#include"../../../Public/FileSystemManager.h"


AngelCore::AngelWorldSystem::AudioHandler::AudioHandler()
{

}


AngelCore::AngelWorldSystem::AudioHandler::AudioHandler(AudioHandler &&that)
{
	
}

AngelCore::AngelWorldSystem::AudioHandler::AudioHandler(const AudioHandler &that)
{

}

AngelCore::AngelWorldSystem::AudioHandler & AngelCore::AngelWorldSystem::AudioHandler::operator=(AudioHandler &&that)
{
	return *this;
}

AngelCore::AngelWorldSystem::AudioHandler & AngelCore::AngelWorldSystem::AudioHandler::operator=(const AudioHandler &that)
{
	return *this;
}

AngelCore::AngelWorldSystem::AudioHandler::~AudioHandler()
{
	for (unsigned int i = 0;i < m_audioClips.size();i++)
	{
		m_audioClips[i]->soundEffect.reset();
		m_audioClips[i]->soundEffectInstance.reset();
	}
}

void AngelCore::AngelWorldSystem::AudioHandler::Initialize(Actor* _owner, ...)
{
	SetOwner(_owner);
}

void AngelCore::AngelWorldSystem::AudioHandler::Update(const AngelSubSystem::TimeManager &timer)
{

}

void AngelCore::AngelWorldSystem::AudioHandler::Render()
{

}

void AngelCore::AngelWorldSystem::AudioHandler::Play(bool &loop, int &index)
{
	if (m_audioClips[index]->soundEffectInstance->GetState() == DirectX::SoundState::PLAYING)
	{
		Stop(index);
	}
	m_audioClips[index]->soundEffectInstance->Play(loop);
	
	m_audioClips[index]->paused = false;
}

void AngelCore::AngelWorldSystem::AudioHandler::Pause(int &index)
{
	m_audioClips[index]->soundEffectInstance->Pause();
	m_audioClips[index]->paused = true;
}

void AngelCore::AngelWorldSystem::AudioHandler::Resume(int &index)
{
	if(m_audioClips[index]->paused)
		m_audioClips[index]->soundEffectInstance->Resume();

	m_audioClips[index]->paused = false;
}

void AngelCore::AngelWorldSystem::AudioHandler::Stop(int &index)
{
	m_audioClips[index]->soundEffectInstance->Stop();
}

void AngelCore::AngelWorldSystem::AudioHandler::SetPan(float &pan, int &index)
{
	m_audioClips[index]->soundEffectInstance->SetPan(pan);
}

void AngelCore::AngelWorldSystem::AudioHandler::SetPitch(float &pitch, int &index)
{
	m_audioClips[index]->soundEffectInstance->SetPitch(pitch);
}

void AngelCore::AngelWorldSystem::AudioHandler::SetVolume(float &vol,int &index)
{
	m_audioClips[index]->soundEffectInstance->SetVolume(vol);
}

void AngelCore::AngelWorldSystem::AudioHandler::Load(std::string &name)
{
	std::wstring nameWstr(name.begin(), name.end());
	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	path += L"\\AngelData\\Sound\\";
	path += nameWstr;
	AudioClip * clip = new AudioClip();
	clip->name = name.c_str();
	clip->soundEffect.reset(new DirectX::SoundEffect
		(AngelCore::AngelSubSystemResources::AudioEngineProperties::aduioManager,
			path.c_str()));
	clip->soundEffectInstance = clip->soundEffect->CreateInstance();
	m_audioClips.push_back(clip);
}