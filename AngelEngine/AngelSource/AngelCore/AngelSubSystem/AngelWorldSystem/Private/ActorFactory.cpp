#include"../Public/ActorFactory.h"
#include"../Public/Actor.h"
#include"../Public/ActorsHolder.h"
#include<string>
#include"../../../AngelGameObjects/Public/MainCamera.h"
#include"../AngelComponent/Public/Transform.h"
#include"../AngelComponent/Public/MeshRenderer.h"
#include"../AngelComponent/Public/ParticleRenderer.h"
#include"../AngelComponent/Public/RigidBody.h"
#include"../AngelComponent/Public/AudioHandler.h"
#include"../AngelComponent/Public/LightRenderer.h"
#include"../AngelComponent/Public/VideoRenderer.h"
#include"../AngelComponent/Public/CharacterController.h"
#include"tinyxml.h"

bool AngelCore::AngelWorldSystem::ActorFactory::InstantiateActor(TiXmlElement * node)
{
	using namespace std;
	Actor * actor = new Actor();
	actor->Initialize();

	actor->SetName(node->Attribute("Name"));
	actor->SetTag(node->Attribute("Tag"));
	node = node->FirstChild()->ToElement();
	

	for (node; node; node = node->NextSiblingElement())
	{
		std::string s = node->Value();
		if (strcmp(node->Value(), "Script") == 0)
		{
			std::string scriptName = node->Attribute("Name");
			actor->AttachScript(scriptName.c_str());
		}
		if (_strcmpi("Transform", node->Value()) == 0)
		{
			actor->AddComponent("Transform");
			actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->Initialize(actor);
			TiXmlElement * PElemChild = node->FirstChildElement();
			{
				double * data = new double[3];
				PElemChild->QueryDoubleAttribute("x", &data[0]);
				PElemChild->QueryDoubleAttribute("y", &data[1]);
				PElemChild->QueryDoubleAttribute("z", &data[2]);

				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetX(data[0]);
				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetY(data[1]);
				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->SetZ(data[2]);

				PElemChild = PElemChild->NextSiblingElement();

				PElemChild->QueryDoubleAttribute("x", &data[0]);
				PElemChild->QueryDoubleAttribute("y", &data[1]);
				PElemChild->QueryDoubleAttribute("z", &data[2]);

				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->SetX(data[0]);
				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->SetY(data[1]);
				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->SetZ(data[2]);

				PElemChild = PElemChild->NextSiblingElement();

				PElemChild->QueryDoubleAttribute("x", &data[0]);
				PElemChild->QueryDoubleAttribute("y", &data[1]);
				PElemChild->QueryDoubleAttribute("z", &data[2]);

				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetX(DirectX::XMConvertToRadians(data[0]));
				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetY(DirectX::XMConvertToRadians(data[1]));
				actor->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->SetZ(DirectX::XMConvertToRadians(data[2]));
				delete[] data;
			}
		}
		if (_strcmpi("MeshRenderer", node->Value()) == 0)
		{
			actor->AddComponent("MeshRenderer");
			TiXmlElement * PElemChild = node->FirstChildElement();
			
			std::string meshName = PElemChild->Attribute("MeshName");
			bool skining = false;
			PElemChild->QueryBoolAttribute("Skinning", &skining);
			int type = 0;
			PElemChild->QueryIntAttribute("MaterialType", &type);
			actor->GetComponent<AngelCore::AngelWorldSystem::MeshRenderer>()->Initialize(actor,meshName.c_str(), skining, type);
		}
		if (_strcmpi("AudioHandler", node->Value()) == 0)
		{
			actor->AddComponent("AudioHandler");
			actor->GetComponent<AngelCore::AngelWorldSystem::AudioHandler>()->Initialize(actor);
		}
		if (_strcmpi("VideoRenderer", node->Value()) == 0)
		{
			actor->AddComponent("VideoRenderer");
			actor->GetComponent<AngelCore::AngelWorldSystem::VideoRenderer>()->Initialize(actor);
		}
		if (_strcmpi("CharacterController", node->Value()) == 0)
		{
			actor->AddComponent("CharacterController");
			actor->GetComponent<AngelCore::AngelWorldSystem::CharacterController>()->Initialize(actor);
		}
		if (_strcmpi("ParticleRenderer", node->Value()) == 0)
		{
			actor->AddComponent("ParticleRenderer");
			actor->GetComponent<AngelCore::AngelWorldSystem::ParticleRenderer>()->Initialize(actor);
		}
		if (_strcmpi("RigidBody", node->Value()) == 0)
		{
			actor->AddComponent("RigidBody");
			int shapeType = 0;
			TiXmlElement * PElemChild = node->FirstChildElement();
			PElemChild->QueryIntAttribute("ShapeType", &shapeType);
			float data = 0;
			PElemChild->QueryFloatAttribute("ShapeRadius", &data);
			actor->GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->Initialize(actor,
				AngelCore::AngelWorldSystem::RigidBody::COLLISION_SHAPE(shapeType), data);
			PElemChild->QueryFloatAttribute("Mass", &data);
			actor->GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetMass(data);
			float * res = new float();
			PElemChild->QueryFloatAttribute("Restitution", res);
			actor->GetComponent<AngelCore::AngelWorldSystem::RigidBody>()->SetRestitution(*res);
		}
		if (_strcmpi("LightRenderer", node->Value()) == 0)
		{
			actor->AddComponent("LightRenderer");
			actor->GetComponent<AngelCore::AngelWorldSystem::LightRenderer>()->Initialize(actor);
			TiXmlElement * PElemChild = node->FirstChildElement();
			int data = 0;
			PElemChild->QueryIntAttribute("Type", &data);
			const int type = data;
			actor->GetComponent<AngelCore::AngelWorldSystem::LightRenderer>()->SetType(type);
			PElemChild->QueryIntAttribute("CastShadow", &data);
			const int castShadow = data;
			actor->GetComponent<AngelCore::AngelWorldSystem::LightRenderer>()->SetCastShadow(castShadow);
			PElemChild = PElemChild->NextSiblingElement();
			float * color = new float[4];
			PElemChild->QueryFloatAttribute("x", &color[0]);
			PElemChild->QueryFloatAttribute("y", &color[1]);
			PElemChild->QueryFloatAttribute("z", &color[2]);
			PElemChild->QueryFloatAttribute("w", &color[3]);
			actor->GetComponent<AngelCore::AngelWorldSystem::LightRenderer>()->
				SetColor
				(DirectX::XMFLOAT4(color[0], color[1], color[2], color[3]));
		}
	}

	m_actorsHolder->InsertActor(actor);

	return true;
}

bool AngelCore::AngelWorldSystem::ActorFactory::InstantiateActor()
{
	/*AngelCore::AngelWorldSystem::Actor * a = new AngelCore::AngelWorldSystem::Actor();
	a->AddComponent("transform");
	a->GetComponent<AngelCore::AngelWorldSystem::Transform>()->Initialize(a);
	a->Initialize();
	a->AddComponent("ParticleRenderer");
	a->GetComponent<AngelCore::AngelWorldSystem::ParticleRenderer>()->Initialize(a);
	m_actorsHolder->InsertActor(a);*/

	return true;
}

bool AngelCore::AngelWorldSystem::ActorFactory::StartUp(...)
{
	this->m_actorsHolder = ActorsHolder::GetInstance();

	if (!this->m_actorsHolder->Initialize())
		return false;

	this->m_memoryPool = new AngelMemory::MemoryPool();

	this->m_memoryPool->Init(sizeof(Actor), 100000 * sizeof(Actor));
	this->m_pause = false;
	return true;
}

bool AngelCore::AngelWorldSystem::ActorFactory::ShutDown()
{
	delete this->m_actorsHolder;
	this->m_memoryPool->FreeAll();

	delete this->m_memoryPool;


	return true;
}

bool AngelCore::AngelWorldSystem::ActorFactory::UnloadActors()
{
	return m_actorsHolder->UnloadActors();
}

void AngelCore::AngelWorldSystem::ActorFactory::Update(const AngelCore::AngelSubSystem::TimeManager & timer)
{

	if (!m_pause)
		this->m_actorsHolder->Update(timer);
}

void AngelCore::AngelWorldSystem::ActorFactory::Render()
{
	this->m_actorsHolder->Render();
}

void AngelCore::AngelWorldSystem::ActorFactory::RenderGeometry()
{
	this->m_actorsHolder->RenderGeometry();
}

AngelCore::AngelWorldSystem::Actor* AngelCore::AngelWorldSystem::ActorFactory::GetActorByName(std::string &name)
{
	return this->m_actorsHolder->FindActorWithName(name.c_str());
}

AngelCore::AngelWorldSystem::Actor* AngelCore::AngelWorldSystem::ActorFactory::GetActorByTag(std::string &name)
{
	return this->m_actorsHolder->FindActorWithTag(name.c_str());
}