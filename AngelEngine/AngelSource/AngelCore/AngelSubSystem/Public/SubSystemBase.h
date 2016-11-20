#pragma once

#include"SubSystemsResource.h"

#include"../../AngelTools/AngelGlobalTools.h"

namespace AngelCore
{
	namespace AngelSubSystem
	{
		class ISubSystem
		{

		public:
			virtual bool StartUp(...) = 0;
			virtual bool ShutDown() = 0;
		};
	}
}
