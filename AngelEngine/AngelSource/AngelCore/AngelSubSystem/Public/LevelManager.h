#pragma once
#include"SubSystemBase.h"

namespace AngelCore
{
	namespace AngelSubSystem
	{
		class LevelManager : public AngelSubSystem::ISubSystem
		{
		public:
			bool StartUp(...) override;
			bool ShutDown() override;

		public:
			void SetDefaultLevel(std::string  levelName) { m_defaultLevelName = levelName.c_str(); }
			bool LoadLevel(std::string levelName);
			bool LoadLevel(std::string &levelName);
			void UnloadCurrentLevel();

		private:
			const char * m_defaultLevelName;
		};
	}
}