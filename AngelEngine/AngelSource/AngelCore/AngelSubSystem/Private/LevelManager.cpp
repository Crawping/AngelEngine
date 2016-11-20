#include"../Public/LevelManager.h"
#include"../../AngelSubSystem/Public/SubSystemManager.h"
#include"../../AngelSubSystem/Public/FileSystemManager.h"
#include"../../AngelSubSystem/Public/PhysicManager.h"
#include"../../AngelSubSystem/Public/VRManager.h"
#include<assert.h>
#include<tinyxml.h>
#include<string>
#include<Source\Include.h>
#include"../AngelRendering/Public/LightManager.h"
#include"../AngelRendering/Public/TerrainManager.h"

bool AngelCore::AngelSubSystem::LevelManager::StartUp(...)
{
	
	return true;
}

bool AngelCore::AngelSubSystem::LevelManager::ShutDown()
{
	return true;
}

bool AngelCore::AngelSubSystem::LevelManager::LoadLevel(std::string levelName)
{
	UnloadCurrentLevel();
	TiXmlDocument *doc = new TiXmlDocument();
	doc->Clear();
	levelName += ".XML";
	std::wstring levelNameWstr(levelName.begin(), levelName.end());
	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	path += L"\\AngelData\\Level\\";
	path += levelNameWstr;

	std::string levelNameStr(path.begin(), path.end());
	doc->LoadFile(levelNameStr.c_str());
	TiXmlHandle hDoc(doc);
	TiXmlElement * root = hDoc.FirstChildElement().Element();
	TiXmlHandle hRoot(0);
	hRoot = TiXmlHandle(root);
	TiXmlElement* pElem = hRoot.FirstChild("Identifier").Element();
	TiXmlElement *startElem = new TiXmlElement("NULL");
	for (pElem; pElem; pElem = pElem->NextSiblingElement())
	{
		if (strcmp(pElem->Value(), "Actor") == 0)
		{
			if (!SubSystemManager::GetInstance
				()->GetActorFactory()->InstantiateActor(pElem))
			{
				std::string txt;
				txt += "Load Level ";
				txt + levelName.c_str();
				txt += "Failed";
				AngelLog::WriteErrorMessage(txt.c_str());
				return false;
			}
		}
	}
	std::string txt;
	txt += "Level ";
	txt + levelName.c_str();
	txt += "Loaded Successfully";
	AngelLog::WriteLogMessage(txt.c_str());
	SubSystemManager::GetInstance
		()->GetActorFactory()->InstantiateActor();
	return true;
}
bool AngelCore::AngelSubSystem::LevelManager::LoadLevel(std::string &levelName)
{
	UnloadCurrentLevel();
	TiXmlDocument *doc = new TiXmlDocument();
	doc->Clear();
	levelName += ".XML";
	std::wstring levelNameWstr(levelName.begin(), levelName.end());
	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	path += L"\\AngelData\\Level\\";
	path += levelNameWstr;

	//Customized code here just for samples and showing features.
	if (strcmpi("VirtualReality.XML", levelName.c_str()) == 0)
	{
		if (!AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->GetInitializedSucced())
		{
			MessageBox(NULL,
				L"It Seems You Don't Have Oculus Headset Connected To Computer,If You Want To Test VR Sample Download Oculus Runtime Sdk,After Instalation Open It,Goto->tools->service->configure And From Debug HMD Device Select DK2 And Click Ok. After That ReOpen Program To Test VR Sample.", L"How To Test VR Sample Without Headset", MB_ICONINFORMATION | MB_OK);
			std::string level = "sampleMain";
			LoadLevel("sampleMain");
			return true;
		}
		AngelGUISystem::AngelGUI::SetUsingAddtionalRenderTarget(true);
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->SetEnable(true);
	}

	else if (strcmpi("TerrainEditor.XML", levelName.c_str()) == 0)
	{
		if(!AngelRendering::TerrainManager::GetInstance()->GetInitialized())
		AngelRendering::TerrainManager::GetInstance()->Initialize();
		AngelRendering::TerrainManager::GetInstance()->SetEnable(true);
	}
	else
	{
		AngelRendering::TerrainManager::GetInstance()->SetEnable(false);
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetVRManager()->SetEnable(false);
		AngelGUISystem::AngelGUI::SetUsingAddtionalRenderTarget(false);
	}
		

	std::string levelNameStr(path.begin(), path.end());
	doc->LoadFile(levelNameStr.c_str());
	TiXmlHandle hDoc(doc);
	TiXmlElement * root = hDoc.FirstChildElement().Element();
	TiXmlHandle hRoot(0);
	hRoot = TiXmlHandle(root);
	TiXmlElement* pElem = hRoot.FirstChild("Identifier").Element();
	TiXmlElement *startElem = new TiXmlElement("NULL");
	for (pElem; pElem; pElem = pElem->NextSiblingElement())
	{
		if (strcmp(pElem->Value(), "Actor") == 0)
		{
			if (!SubSystemManager::GetInstance
				()->GetActorFactory()->InstantiateActor(pElem))
			{
				std::string txt;
				txt += "Load Level ";
				txt + levelName.c_str();
				txt += "Failed";
				AngelLog::WriteErrorMessage(txt.c_str());
				return false;
			}
		}
	}
	std::string txt;
	txt += "Level ";
	txt += levelName.c_str();
	txt += " Loaded Successfully";
	AngelLog::WriteLogMessage(txt.c_str());
	SubSystemManager::GetInstance
		()->GetActorFactory()->InstantiateActor();
	
	return true;
}

void AngelCore::AngelSubSystem::LevelManager::UnloadCurrentLevel()
{
	bool b = false;
	b = SubSystemManager::GetInstance
		()->GetActorFactory()->UnloadActors();
	AngelGUISystem::AngelGUI::Clear();
	AngelRendering::LightManager::GetInstance()->ClearLights();
	AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()->Clear();
	if (!b)
	{
		std::string txt;
		txt += "Unload Level Fails ";
		AngelLog::WriteErrorMessage(txt.c_str());
	}
	if (!b)
	{
		std::string txt;
		txt += "Unload Level Successfull ";
		AngelLog::WriteErrorMessage(txt.c_str());
	}
	
	//AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetPhysicManager()->StartUp();
}