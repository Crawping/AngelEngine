#pragma once

#include<typeinfo>
#include<memory>

#include"SubSystemsResource.h"

#include"../AngelWorldSystem/Public/ActorFactory.h"

#include<thread>

namespace AngelCore
{
	namespace AngelSubSystem
	{

#pragma region Forward Declaration
		class PhysicManager;
		class FileSystemManager;
		class RenderManager;
		AngelCore::AngelWorldSystem::ActorFactory;
		class TimeManager;
		class InputManager;
		class ScriptManager;
		class WorldManager;
		class LevelManager;
		class MediaManager;
		class VRManager;
#pragma endregion

#pragma region Typdefs
		using PhysicManagerStrongPtr = std::shared_ptr<PhysicManager>;
		using FileSystemManagerStrongPtr = std::shared_ptr<FileSystemManager>;
		using RenderManagerStrongPtr = std::shared_ptr<RenderManager>;
		using ActorFactoryStrongPtr = std::shared_ptr<AngelCore::AngelWorldSystem::ActorFactory>;
		using TimeManagerStrongPtr = std::shared_ptr<TimeManager>;
		using TimeManagerPtr = TimeManager *;
		using PhysicManagerPtr = PhysicManager*;
		using FileSystemManagerPtr = FileSystemManager*;
		using RenderManagerPtr = RenderManager*;
		using ActorFactoryPtr = AngelCore::AngelWorldSystem::ActorFactory*;
		using InputManagerPtr = InputManager*;
		using ScriptManagerStrongPtr = std::shared_ptr<ScriptManager>;
		using ScriptManagerPtr = ScriptManager*;
		using WorldManagerStrongPtr = std::shared_ptr<WorldManager>;
		using WorldManagerPtr = WorldManager*;
		using InputManagerStrongPtr = std::shared_ptr<InputManager>;
		using LevelManagerStrongPtr = std::shared_ptr<LevelManager>;
		using LevelManagerPtr = LevelManager*;
		using MediaManagerPtr = MediaManager*;
		using VRManagerPtr = VRManager*;
#pragma endregion

		class SubSystemManager
		{
			friend class ScriptManager;
		public:
			bool StartUpEngineSubSystem();
			bool ShutDownEngineSubSystem();
			static SubSystemManager* GetInstance();

		public:

#pragma region Getters
			RenderManagerPtr GetRenderManager()
			{
				return this->m_renderManager;
			}

			PhysicManagerPtr GetPhysicManager()
			{
				return this->m_physicManager.get();
			}

			FileSystemManagerPtr GetFileSystemManager()
			{
				return this->m_fileSystemManager.get();
			}

			AngelCore::AngelWorldSystem::ActorFactory* GetActorFactory()
			{
				return this->m_actorFactory;
			}

			TimeManagerPtr GetTimeManager()
			{
				return this->m_timeManager;
			}

			InputManagerPtr GetInputManager()
			{
				return this->m_inputManager.get();
			}

			ScriptManagerPtr GetScriptManager()
			{
				return this->m_scriptManager.get();
			}

			WorldManagerPtr GetWorldManager()
			{
				return this->m_worldManager;
			}

			LevelManagerPtr GetLevelManager()
			{
				return this->m_levelManager;
			}
			MediaManagerPtr GetMediaManager()
			{
				return m_mediaManager;
			}
			VRManagerPtr GetVRManager()
			{
				return m_vrManager;
			}

			bool GetInitialize() const { return m_initialized; }

#pragma endregion

#pragma region Setters
			void SetInitialize(bool b)  {  m_initialized = b; }
#pragma endregion


		private:

			PhysicManagerStrongPtr			m_physicManager;
			FileSystemManagerStrongPtr		m_fileSystemManager;
			RenderManagerPtr				m_renderManager;
			ActorFactoryPtr					m_actorFactory;
			TimeManagerPtr					m_timeManager;
			InputManagerStrongPtr			m_inputManager;
			ScriptManagerStrongPtr			m_scriptManager;
			WorldManagerPtr					m_worldManager;
			LevelManagerPtr					m_levelManager;
			MediaManagerPtr					m_mediaManager;
			VRManagerPtr					m_vrManager;
			bool							m_initialized;

		private:

			SubSystemManager();
			static SubSystemManager *m_sSubSysteManager;
		};
	}
}
