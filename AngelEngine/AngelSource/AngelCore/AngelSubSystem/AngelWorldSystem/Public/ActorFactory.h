#pragma once

#include"../../../AngelSubSystem/Public/SubSystemBase.h"

#include"../../../AngelMemory/Public/MemoryPool.h"

#include"../../Public/TimeManager.h"

#include"../../AngelRendering/Public/RenderTexture.h"

#include<tinyxml.h>

namespace AngelCore
{
	namespace AngelWorldSystem
	{
#pragma region Forward Declaration
		class Actor;
		class ActorsHolder;
#pragma endregion

		class ActorFactory : public AngelSubSystem::ISubSystem
		{
		public:
			bool InstantiateActor();
			bool InstantiateActor(TiXmlElement * node);

			bool StartUp(...) override;

			bool ShutDown() override;
			bool UnloadActors();

		public:

			void Update(const AngelCore::AngelSubSystem::TimeManager & timer);

			Actor * GetActorByName(std::string &name);
			Actor * GetActorByTag(std::string &name);

			void Render();
			void RenderGeometry();

		public:
			void SetPause(bool _b)						{ this->m_pause = _b; }

			bool GetPause()								{ return this->m_pause; }

			ActorsHolder * GetActors()					{ return m_actorsHolder; }

			template<typename T>
			std::vector<Actor*>	GetActorWithComponent()
			{
				std::vector<Actor*> ret;
				std::vector<Actor*> allActors = m_actorsHolder->GetActors();
				for (int i = 0;i < allActors.size();i++)
				{
					if (allActors[i]->GetComponent<T>() != nullptr)
					{
						ret.push_back(allActors[i]);
					}
				}
				return ret;
			}

		private:
			ActorsHolder *								m_actorsHolder;
			AngelMemory::MemoryPool	*					m_memoryPool;
			bool										m_pause;

		};
	}
}