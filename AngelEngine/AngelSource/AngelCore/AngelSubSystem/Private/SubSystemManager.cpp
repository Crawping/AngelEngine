#include"../Public/SubSystemManager.h"
#include"../AngelLog/Public/AngelLog.h"
#include"../Public/PhysicManager.h"
#include"../Public/FileSystemManager.h"
#include"../Public/RenderManager.h"
#include"../Public/TimeManager.h"
#include"../Public/InputManager.h"
#include"../Public/ScriptManager.h"
#include"../Public/LevelManager.h"
#include"../Public/WorldManager.h"
#include"../Public/MediaManager.h"
#include"../Public/VRManager.h"
#include<assert.h>
#include<AntTweakBar.h>
#include<Source\Include.h>

AngelCore::AngelSubSystem::SubSystemManager * AngelCore::AngelSubSystem::SubSystemManager::m_sSubSysteManager = nullptr;

bool AngelCore::AngelSubSystem::SubSystemManager::StartUpEngineSubSystem()
{

	this->m_fileSystemManager = std::make_shared<FileSystemManager>();
	if (this->m_fileSystemManager)
	{
		/*auto func = std::bind(&FileSystemManager::StartUp, this, std::placeholders::_1);
		auto f = std::async(std::launch::async, &FileSystemManager::StartUp, &m_fileSystemManager);*/
		if (!this->m_fileSystemManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up FileSystemManager.");
			return false;
		}

		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("File System Manager Started.");
	}

	this->m_renderManager = new RenderManager();

	if (this->m_renderManager)
	{
		if (!this->m_renderManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up RenderManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Render Manager Started.");
	}

	if (!AngelGUISystem::AngelGUI::StartUp
		(AngelCore::AngelSubSystemResources::WindowProperties::GetHWND()
			, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get(),
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext.Get(),
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChain.Get()))
	{
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up AngelGUISystem.");
		return false;
	}

	AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("AngelGUISystem Started.");

	if (!TwInit(TW_DIRECT3D11, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get()))
	{
		MessageBoxA(NULL, TwGetLastError(), "AntTweakBar initialization failed", MB_OK | MB_ICONERROR);
	}


	this->m_physicManager = std::make_shared<PhysicManager>();
	if (this->m_physicManager)
	{
		if (!this->m_physicManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up PhysicManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("Physic Manager Started.");
	}

	this->m_actorFactory = new AngelCore::AngelWorldSystem::ActorFactory();
	if (this->m_actorFactory)
	{
		if (!this->m_actorFactory->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up ActorFactory.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("ActorFactory Started.");
	}

	this->m_inputManager = std::make_shared<InputManager>();
	if (this->m_inputManager)
	{
		if (!this->m_inputManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up InputManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("InputManager Started.");

		AngelSubSystem::inputManager = this->m_inputManager.get();
	}

	this->m_timeManager = new TimeManager();
	if (this->m_timeManager)
	{
		if (!this->m_timeManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up TimeManager.");
			return false;
		}
		this->m_timeManager->SetFixedTimeStep(false);
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("TimeManager Started.");
	}

	this->m_levelManager = new LevelManager();
	if (this->m_levelManager)
	{
		if (!this->m_levelManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up LevelManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("LevelManager Started.");
	}

	this->m_worldManager = new WorldManager();
	if (this->m_worldManager)
	{
		if (!this->m_worldManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up WorldManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("WorldManager Started.");
	}

	this->m_scriptManager = std::make_shared<ScriptManager>();
	if (this->m_scriptManager)
	{
		if (!this->m_scriptManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up ScriptManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("ScriptManager Started.");
	}

	

	AngelCore::AngelSubSystemResources::AudioEngineProperties::aduioManager = new DirectX::AudioManager();
	if (!AngelCore::AngelSubSystemResources::AudioEngineProperties::aduioManager)
	{
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up AudioManager.");
		return false;
	}

	this->m_mediaManager = new MediaManager();
	if (this->m_mediaManager)
	{
		if (!this->m_mediaManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up MediaManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("MediaManager Started.");
	}

	this->m_vrManager = new VRManager();
	if (this->m_vrManager)
	{
		if (!this->m_vrManager->StartUp())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Start Up VRManager.");
			return false;
		}
		AngelCore::AngelSubSystem::AngelLog::WriteLogMessage("VRManager Started.");
	}

	m_initialized = true;

	return true;
}

bool AngelCore::AngelSubSystem::SubSystemManager::ShutDownEngineSubSystem()
{
	if (m_physicManager)
	{
		{
			if (!m_physicManager->ShutDown())
			{
				AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed ShutDown PhysicManager.");
				return false;
			}
		}
		this->m_physicManager.reset();
	}
	if (this->m_renderManager)
	{
		if (!m_renderManager->ShutDown())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed ShutDown RenderManager.");
			return false;
		}
		delete m_renderManager;
	}

	if (this->m_actorFactory)
	{

		if (!this->m_actorFactory->ShutDown())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed ShutDown ActorFactory.");
			return false;
		}

		delete m_actorFactory;
	}

	if (this->m_timeManager)
	{
		if (!this->m_timeManager->ShutDown())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed ShutDown TimeManager.");
			return false;
		}
	}

	if (this->m_inputManager)
	{
		if (!this->m_inputManager->ShutDown())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed ShutDown InputManager.");
			return false;
		}
		this->m_inputManager.reset();
	}

	AngelGUISystem::AngelGUI::ShutDown();

	if (this->m_scriptManager)
	{
		if (!this->m_scriptManager->ShutDown())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed ShutDown ScriptManager.");
			return false;
		}
		this->m_scriptManager.reset();
	}

	if (this->m_worldManager)
	{
		if (!this->m_worldManager->ShutDown())
		{
			AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed ShutDown WorldManager.");
			return false;
		}
		delete m_worldManager;
	}
	return true;
}

AngelCore::AngelSubSystem::SubSystemManager * AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
{
	if (!m_sSubSysteManager)
	{
		m_sSubSysteManager = new SubSystemManager();
		assert(m_sSubSysteManager);
		return m_sSubSysteManager;
	}
	return m_sSubSysteManager;
}

AngelCore::AngelSubSystem::SubSystemManager::SubSystemManager() : m_scriptManager(nullptr)
, m_actorFactory(nullptr)
, m_fileSystemManager(nullptr)
, m_inputManager(nullptr)
, m_physicManager(nullptr)
, m_renderManager(nullptr)
, m_timeManager(nullptr)
,m_worldManager(nullptr)
,m_initialized(false)
{

}