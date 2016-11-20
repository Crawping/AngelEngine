#pragma once

#include"../../Public/Actor.h"

#include"../../../Public/TimeManager.h"

#include"../../../../AngelTools/AngelDataStructures/Public/ALinkedList.h"

namespace AngelCore
{
	namespace AngelWorldSystem
	{

		class IComponent : public AngelTools::AngelDataStructures::ALink<IComponent>
		{
		public:
			
			IComponent() { }
			virtual ~IComponent() {}
#pragma region Setter

			void SetActive(bool _active) { this->m_active = _active; }
			void SetActive(bool &_active) { if (_active == true) m_active = true; else m_active = false; }

			void SetName(const char * _name) { this->m_name = _name; }

#pragma endregion

		public:

#pragma region Getter

			bool GetActive() const { return this->m_active; }

			const char * GetName() const { return this->m_name; }

#pragma endregion

		public:

			virtual void Initialize(Actor * _owner,...) = 0;

			virtual void Update(const AngelSubSystem::TimeManager &timer) = 0;

			virtual void Render() = 0;

		protected:

			Actor* GetOwner()									{ return this->m_owner; }
			void SetOwner(Actor * _owner)						{ this->m_owner = _owner; }
			Actor *						m_owner;
			bool						m_active;

		private:
			bool						m_initialized;
			const char *				m_name;
		};
	}
}
