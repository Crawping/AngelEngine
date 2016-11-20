#include"../Public/FileSystemManager.h"

#include<iostream>
#include<fstream>
#include<istream>
#include<string>

#include"../AngelLog/Public/AngelLog.h"

bool AngelCore::AngelSubSystem::FileSystemManager::StartUp(...)
{
	if (!InitLogFile())
	{
		return false;
	}
	return true;
}

bool AngelCore::AngelSubSystem::FileSystemManager::ShutDown()
{
	return true;
}

bool AngelCore::AngelSubSystem::FileSystemManager::InitLogFile()
{
	std::string fileName = "AngelEngineLog.txt";
	std::fstream file(fileName.c_str(),std::ios::out);

	if (!file || !file.is_open() || file.bad())
		return false;

	file << "Date:" << AngelCore::AngelSubSystem::AngelLog::GetCurrentDateTime() << "\nAngel Engine Log Started \n\n";
	file.close();

	return true;
}