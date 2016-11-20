#pragma once

#include"../../../AngelTools/AngelDataStructures/Public/ALinkedList.h"
#include"../../../AngelTools/AngelDataStructures/Public/ALink.h"
#include"../../../AngelMemory/Public/StackAllocator.h"
#include"add_on\scripthandle\scripthandle.h"
#include"../AngelComponent/Public/Component.h"
#include"../../Public/TimeManager.h"
#include<memory>
#include<vector>
#include<Windows.h>

#define __ACTOR

namespace AngelCore
{

	namespace AngelWorldSystem
	{
		class IComponent;

		using ActorID = uint32_t;

		class Transform;
		class MeshRenderer;
		class RigidBody;
	
		class Actor : public AngelTools::AngelDataStructures::ALink<Actor>, public AngelWorldSystem::Component
		{
		public:
			Actor(const Actor &that);
			Actor();
			~Actor();

		public:
			virtual void Initialize();

			virtual void Update(const AngelCore::AngelSubSystem::TimeManager & timer);

			virtual void Render();

			virtual void RenderGeometry();

		public:
			///SCRIPT
			int AddRef();
			int Release();
			asILockableSharedBool *GetWeakRefFlag();
			void Send(std::string &msg, std::string &otherName);
			void Send(std::string &msg, Actor &otherName);

			void StartScript();

			void UpdateScript();

			void OnCollide(Actor* other);

			void AttachScript(const char * scriptName);

			///

		public:

			//Use only for registring in angel script.
			void AddComponent(const std::string &componentName);
			template<typename T>
			T * GetComponent()
			{
				for (unsigned int i = 0; i < this->m_components.size(); i++)
				{
					const char *name1 = typeid(T*).name();
					T * data = dynamic_cast<T*>(this->m_components.at(i));
					if (!data) continue;
					const char *name2 = typeid(data).name();
					if (std::strcmp(name1, name2) == 0)
					{
						return data;
					}
				}
				return nullptr;
			}
		public:

			bool operator ==(const Actor &rhs);
			const bool operator ==(const Actor &rhs) const;
			bool operator !=(const Actor &rhs);
			const bool operator !=(const Actor &rhs) const;

		public:

#pragma region Setter

			void SetActive(bool _active);
			void SetActive(bool &_active);

			void SetName(const char * _name)	{ this->m_name = _name; }

			void SetTag(const char *_tag)		{ this->m_tag = _tag; }

			void SetID(ActorID id)				{ this->m_id = id; }

			void SetScriptInitialized(bool b)	{ this->m_initializeScript = b; }
			//for script
			void SetHidden(bool &b)				{ this->m_hidden = b; }

			void SetHidden(bool b)				{ this->m_hidden = b; }

#pragma endregion

		public:

#pragma region Getter

			bool GetActive() const				{ return this->m_active; }

			const char * GetName() 				{ return this->m_name; }

			std::string GetName() const			{ return this->m_name; }

			const char * GetTag() 			{ return this->m_tag; }

			std::string	 GetTag() const { return this->m_tag; }


			ActorID GetID() const				{ return this->m_id; }

			bool GetScriptInitialized() const	{ return m_initializeScript; }

			bool GetHidden()					{ return m_hidden; }

#pragma endregion

		private:

			bool								m_active;
			const char*							m_name;
			const char*							m_tag;
			ActorID								m_id;
			std::vector<IComponent*>			m_components;
			AngelMemory::StackAllocator *		m_componentMemory;
			bool								m_hidden;
		private:

			///SCRIPT
			asIScriptObject *					m_controller;
			asILockableSharedBool *				m_weakRefFlag;
			unsigned int						m_refCount;
			bool								m_initializeScript;
			///

		};
	}
}
