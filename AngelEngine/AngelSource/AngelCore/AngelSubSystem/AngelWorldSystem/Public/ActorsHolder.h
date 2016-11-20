#pragma once

#include"../../../AngelTools/AngelDataStructures/Public/ALink.h"
#include"../../../AngelTools/AngelDataStructures/Public/ALinkedList.h"
#include"../../Public/TimeManager.h"

#include"Actor.h"

namespace AngelCore
{
	namespace AngelWorldSystem
	{
#pragma region Forward Declaration
		class Actor;
#pragma endregion
		class ActorsHolder
		{

#pragma region Friends
			friend class ActorFactory;
#pragma endregion

		public:

			static ActorsHolder * GetInstance();

			bool Initialize();

			void Update(const AngelCore::AngelSubSystem::TimeManager & timer);
			
			void Render();
			void RenderGeometry();

			~ActorsHolder();

			bool UnloadActors();

		private:

			ActorsHolder();
			ActorsHolder(const ActorsHolder &that);
			ActorsHolder(ActorsHolder && that);

		private:
			std::vector<Actor*> 			m_linkedListActors;

		private:

			void InsertActor(AngelWorldSystem::Actor* _actor);
			std::vector<Actor*> GetActors() { return m_linkedListActors; }

			AngelWorldSystem::Actor* FindActorWithName(const char * name);
			AngelWorldSystem::Actor* FindActorWithTag(const char * name);
		};
	}
}