#include"../Public/ActorsHolder.h"
#include"../Public/Actor.h"
#include<vector>
#include<assert.h>
#include"../../Public/InputManager.h"
#include"../../AngelWorldSystem/AngelComponent/Public/MeshRenderer.h"


AngelCore::AngelWorldSystem::ActorsHolder* AngelCore::AngelWorldSystem::ActorsHolder::GetInstance()
{
	static ActorsHolder* instance;
	if (!instance)
	{
		instance = new ActorsHolder();
		assert(instance);
		return instance;
	}

	return nullptr;
}

AngelCore::AngelWorldSystem::ActorsHolder::ActorsHolder()
{

}

bool AngelCore::AngelWorldSystem::ActorsHolder::Initialize()
{
	return true;
}

void AngelCore::AngelWorldSystem::ActorsHolder::Update(const AngelCore::AngelSubSystem::TimeManager & timer)
{
	/*AngelTools::AngelDataStructures::ve<Actor>::Iterator beg = this->m_linkedListActors->Begin();
	AngelTools::AngelDataStructures::AILinkedList<Actor>::Iterator  end = this->m_linkedListActors->End();
	while (beg != end)
	{
		beg->Update(timer);
		beg++;
	}
	end->Update(timer);*/
	for (unsigned int i = 0;i < m_linkedListActors.size();i++)
	{
		if (!m_linkedListActors[i]->GetActive()) continue;
		if (!m_linkedListActors[i]->GetScriptInitialized())
		{
			
			m_linkedListActors[i]->StartScript();
			m_linkedListActors[i]->SetScriptInitialized(true);
			m_linkedListActors[i]->UpdateScript();
			m_linkedListActors[i]->Update(timer);
		}
		else
		{
			m_linkedListActors[i]->UpdateScript();
			m_linkedListActors[i]->Update(timer);
		}
	}
}

void AngelCore::AngelWorldSystem::ActorsHolder::Render()
{
	/*AngelTools::AngelDataStructures::AILinkedList<Actor>::Iterator beg = this->m_linkedListActors->Begin();
	AngelTools::AngelDataStructures::AILinkedList<Actor>::Iterator  end = this->m_linkedListActors->End();
	while (beg != end)
	{
		beg->Render();
		beg++;
	}
	end->Render();*/

	for (auto const & member : m_linkedListActors)
	{
		if (!member->GetActive()) continue;
		member->Render();
	}
}

void AngelCore::AngelWorldSystem::ActorsHolder::RenderGeometry()
{
	/*AngelTools::AngelDataStructures::AILinkedList<Actor>::Iterator beg = this->m_linkedListActors->Begin();
	AngelTools::AngelDataStructures::AILinkedList<Actor>::Iterator  end = this->m_linkedListActors->End();
	while (beg != end)
	{
	beg->Render();
	beg++;
	}
	end->Render();*/

	for (auto const & member : m_linkedListActors)
	{
		if (!member->GetActive()) continue;
		if(member->GetComponent<AngelCore::AngelWorldSystem::MeshRenderer>())
			member->RenderGeometry();
	}
}

void AngelCore::AngelWorldSystem::ActorsHolder::InsertActor(AngelWorldSystem::Actor* _actor)
{
	this->m_linkedListActors.push_back(_actor);
}

AngelCore::AngelWorldSystem::Actor* AngelCore::AngelWorldSystem::ActorsHolder::FindActorWithName(const char * name)
{
	for (unsigned int i = 0;i < m_linkedListActors.size();i++)
	{
		if (strcmp(m_linkedListActors[i]->GetName(), name) == 0)
		{
			return m_linkedListActors[i];
		}
	}
	return nullptr;
}

AngelCore::AngelWorldSystem::Actor* AngelCore::AngelWorldSystem::ActorsHolder::FindActorWithTag(const char * name)
{
	for (unsigned int i = 0;i < m_linkedListActors.size();i++)
	{
		if (strcmp(m_linkedListActors[i]->GetTag(), name) == 0)
		{
			return m_linkedListActors[i];
		}
	}
	return nullptr;
}

AngelCore::AngelWorldSystem::ActorsHolder::~ActorsHolder()
{
	for (unsigned int i = 0;i < m_linkedListActors.size();i++)
	{
		delete m_linkedListActors[i];
	}
}

bool AngelCore::AngelWorldSystem::ActorsHolder::UnloadActors()
{
	for (unsigned int i = 0;i < m_linkedListActors.size();i++)
	{
		delete m_linkedListActors[i];
	}
	m_linkedListActors.clear();
	return true;
}
