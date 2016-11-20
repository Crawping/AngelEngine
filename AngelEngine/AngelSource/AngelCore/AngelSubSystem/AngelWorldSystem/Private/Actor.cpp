#include"../Public/Actor.h"
#include<Windows.h>
#include"../AngelComponent/Public/IComponent.h"
#include"../../Public/SubSystemManager.h"
#include"../../Public/ScriptManager.h"
#include"../AngelComponent/Public/Transform.h"
#include"../AngelComponent/Public/AudioHandler.h"
#include"../AngelComponent/Public/RigidBody.h"
#include"../AngelComponent/Public/LightRenderer.h"
#include"../AngelComponent/Public/VideoRenderer.h"
#include"../AngelComponent/Public/ParticleRenderer.h"
#include"../AngelComponent/Public/CharacterController.h"


AngelCore::AngelWorldSystem::Actor::Actor() : m_name(""),
m_tag(""), m_active(true), m_controller(nullptr), m_initializeScript(false) , m_hidden(false) ,m_weakRefFlag(nullptr)

{
	m_refCount = 1;
	//this->m_components = new AngelTools::AngelDataStructures::AILinkedList<IComponent>();
	//this->m_componentMemory = new AngelMemory::StackAllocator();
}

AngelCore::AngelWorldSystem::Actor::~Actor()
{
	for (unsigned int i = 0;i < m_components.size();i++)
	{
		delete m_components[i];
	}
	if (m_controller)
		m_controller->Release();
	//delete this->m_components;
	//this->m_componentMemory->FreeAll();
	//delete this->m_componentMemory;
}

void AngelCore::AngelWorldSystem::Actor::Initialize()
{
	m_controller = nullptr;
}
void AngelCore::AngelWorldSystem::Actor::Update(const AngelCore::AngelSubSystem::TimeManager & timer)
{
	/*AngelTools::AngelDataStructures::AILinkedList<IComponent>::Iterator  beg = this->m_components->Begin();
	AngelTools::AngelDataStructures::AILinkedList<IComponent>::Iterator  end = this->m_components->End();
	while (beg != end)
	{
		beg->Update(timer);
		beg++;
	}
	end->Update(timer);*/
	for (unsigned int i = 0; i < m_components.size();i++)
	{
		m_components[i]->Update(timer);
	}
}

int AngelCore::AngelWorldSystem::Actor::AddRef()
{
	return ++m_refCount;
}

int AngelCore::AngelWorldSystem::Actor::Release()
{
	if (--m_refCount == 0)
	{
		return 0;
	}
	return m_refCount;
}

void AngelCore::AngelWorldSystem::Actor::UpdateScript()
{
	// Call the script controller's OnThink method
	if (m_controller)
	{
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->Update(m_controller);
		asUINT c = m_controller->GetPropertyCount();
		asITypeInfo *t =  m_controller->GetObjectType();
	}
		
}

void AngelCore::AngelWorldSystem::Actor::OnCollide(AngelCore::AngelWorldSystem::Actor*other)
{
	if (m_controller)
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->OnCollide(
			m_controller,other);
}

void AngelCore::AngelWorldSystem::Actor::StartScript()
{
	// Call the script controller's OnThink method
	if (m_controller)
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->Start(m_controller);
}

void AngelCore::AngelWorldSystem::Actor::Send(std::string &msg, std::string &otherName)
{
	Actor * other
		= AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetActorFactory()->
		GetActorByName(otherName);
	if (other && other->m_controller)
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()
		->OnSendMessage(other->m_controller, msg, this);
}

void AngelCore::AngelWorldSystem::Actor::Send(std::string &msg, AngelCore::AngelWorldSystem::Actor& other)
{
	if (&other && other.m_controller)
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()
		->OnSendMessage(other.m_controller, msg, this);
}


void AngelCore::AngelWorldSystem::Actor::AttachScript(const char * scriptName)
{
	std::string scriptNameString = scriptName;
	this->m_controller =
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->CreateController(
		scriptNameString, this);

}

asILockableSharedBool * AngelCore::AngelWorldSystem::Actor::GetWeakRefFlag()
{
	if (!m_weakRefFlag)
		m_weakRefFlag = asCreateLockableSharedBool();

	return m_weakRefFlag;
}

void AngelCore::AngelWorldSystem::Actor::Render()
{
	/*AngelTools::AngelDataStructures::AILinkedList<IComponent>::Iterator  beg = this->m_components->Begin();
	AngelTools::AngelDataStructures::AILinkedList<IComponent>::Iterator  end = this->m_components->End();
	while (beg != end)
	{
		beg->Render();
		beg++;
	}
	end->Render();*/
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->Render();
	}
}

void AngelCore::AngelWorldSystem::Actor::RenderGeometry()
{
	/*AngelTools::AngelDataStructures::AILinkedList<IComponent>::Iterator  beg = this->m_components->Begin();
	AngelTools::AngelDataStructures::AILinkedList<IComponent>::Iterator  end = this->m_components->End();
	while (beg != end)
	{
	beg->Render();
	beg++;
	}
	end->Render();*/
	if (GetComponent<MeshRenderer>() != nullptr)
	{
		GetComponent<MeshRenderer>()->RenderDepth();
	}
}

bool AngelCore::AngelWorldSystem::Actor::operator==(const Actor &that)
{
	if (this->m_id == that.m_id)
		return true;
	return false;
}

const bool AngelCore::AngelWorldSystem::Actor::operator==(const Actor &that) const
{
	if (this->m_id == that.m_id)
		return true;
	return false;
}
bool AngelCore::AngelWorldSystem::Actor::operator!=(const Actor &that)
{
	if (this->m_id != that.m_id)
		return true;
	return false;
}
const bool AngelCore::AngelWorldSystem::Actor::operator!=(const Actor &that) const
{
	if (this->m_id != that.m_id)
		return true;
	return false;
}

AngelCore::AngelWorldSystem::Actor::Actor(const Actor &that)
{
	this->m_name = that.m_name;
	this->m_next = that.m_next;
	this->m_prev = that.m_prev;
	this->m_active = that.m_active;
	this->m_tag = that.m_tag;
}

void AngelCore::AngelWorldSystem::Actor::AddComponent(const std::string &componentName)
{
	if (_strcmpi(componentName.c_str(), "Transform") == 0)
	{
		m_transform = new Transform();
		m_components.push_back(m_transform);
	}

	else if (_strcmpi(componentName.c_str(), "MeshRenderer") == 0)
	{
		m_meshRenderer = new MeshRenderer();
		m_components.push_back(m_meshRenderer);
	}

	else if (_strcmpi(componentName.c_str(), "RigidBody") == 0)
	{
		m_rigidBody = new RigidBody();
		m_components.push_back(m_rigidBody);
	}
	else if (_strcmpi(componentName.c_str(), "AudioHandler") == 0)
	{
		m_audioHandler = new AudioHandler();
		m_components.push_back(m_audioHandler);
	}
	else if (_strcmpi(componentName.c_str(), "LightRenderer") == 0)
	{
		m_lightRenderer = new LightRenderer();
		m_components.push_back(m_lightRenderer);
	}
	else if (_strcmpi(componentName.c_str(), "ParticleRenderer") == 0)
	{
		m_particleRenderer = new ParticleRenderer();
		m_components.push_back(m_particleRenderer);
	}
	else if (_strcmpi(componentName.c_str(), "VideoRenderer") == 0)
	{
		m_videoRenderer = new VideoRenderer();
		m_components.push_back(m_videoRenderer);
	}
	else if (_strcmpi(componentName.c_str(), "CharacterController") == 0)
	{
		m_characterController = new CharacterController();
		m_components.push_back(m_characterController);
	}
}

void AngelCore::AngelWorldSystem::Actor::SetActive(bool b)
{
	if (b == true)
	{
		m_active = true;
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetActive(true);
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetCollisionFlag(btCollisionObject::CF_CHARACTER_OBJECT);
	}
	else
	{
		m_active = false;
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetActive(false);
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

void AngelCore::AngelWorldSystem::Actor::SetActive(bool& b)
{
	if (b == true)
	{
		m_active = true;
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetActive(true);
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetCollisionFlag(btCollisionObject::CF_CHARACTER_OBJECT);
	}
	else
	{
		m_active = false;
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetActive(false);
		GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetCollisionFlag(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}
