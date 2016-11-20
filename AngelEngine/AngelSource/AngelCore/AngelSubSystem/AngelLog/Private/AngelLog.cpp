#include"../Public/AngelLog.h"
#include<ctime>

void AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(const char * message)
{
	std::string fileName = "AngelEngineLog.txt";
	std::fstream file(fileName.c_str(), std::ios::out | std::ios::app);

	if (!file || file.bad() || !file.is_open())
		return;
	

	file << "\n" << GetCurrentDateTime() <<"\t"<< message;
	file.close();
}

void AngelCore::AngelSubSystem::AngelLog::WriteWarningMessage(const char * message)
{
	std::string fileName = "AngelEngineLog.txt";
	std::fstream file(fileName.c_str(), std::ios::out | std::ios::app);

	if (!file || file.bad() || !file.is_open())
		return;

	file << "\n" << GetCurrentDateTime() <<"\tWARNING : "<< message;
	file.close();
}

void AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage(const char * message)
{
	std::string fileName = "AngelEngineLog.txt";
	std::fstream file(fileName.c_str(), std::ios::out | std::ios::app);

	if (!file || file.bad() || !file.is_open())
		return;

	file << "\n " << GetCurrentDateTime() << "\tERROR : " << message;
	file.close();
}

std::string  AngelCore::AngelSubSystem::AngelLog::GetCurrentDateTime()
{
	time_t t = time(0);
	struct tm tstruct;
	errno  =  localtime_s(&tstruct, &t);
	char  buf[80];
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}