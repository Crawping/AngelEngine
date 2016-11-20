#pragma once

#include<fstream>
#include<string>
#include<istream>

namespace AngelCore
{
	namespace AngelSubSystem
	{
		class AngelLog
		{
#pragma region Friend Classes
			friend class FileSystemManager;
#pragma endregion
		public:
			static void WriteLogMessage(const char * message);

			static void WriteWarningMessage(const char * message);

			static void WriteErrorMessage(const char * message);

		private:
			static std::string GetCurrentDateTime();
		};
	}
}