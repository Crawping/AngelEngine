#pragma once

#include"../../AngelSubSystem/AngelWorldSystem/Public/Actor.h"
#include <string>
#include <vector>
#include <angelscript.h>
#include "add_on\scripthandle\scripthandle.h"

#define _SCRIPT_MANAGER

template<typename T>
static void Constructor(void *thisPointer)
{
	new(thisPointer) T();
}

template<typename T>
static void Destructor(void *thisPointer)
{
	((T*)thisPointer)->~T();
}

template<typename T>
static void CopyConstructor(const T &other, void *thisPointer)
{
	new(thisPointer) T(other);
}

class AngelCore::AngelWorldSystem::Actor;


namespace AngelCore
{
	template<>
	class Vector<2>;
	template<>
	class Vector<3>;
	template<>
	class Vector<4>;
	namespace AngelSubSystem
	{

		class ScriptManager : public ISubSystem
		{
			friend class Vector<2>;
			friend class Vector<3>;
			friend class Vector<4>;
		private:

			struct SController
			{
				SController() : type(0), factoryFunc(0), Update(0), onMessage(0) , Start(0),OnCollide(0){}
				std::string        module;
				asITypeInfo    *type;
				asIScriptFunction *factoryFunc;
				asIScriptFunction *Update;
				asIScriptFunction *Start;
				asIScriptFunction *OnCollide;
				asIScriptFunction *onMessage;
			};

		public:
			ScriptManager();
			~ScriptManager();

			bool StartUp(...) override;
			bool ShutDown() override;

			asIScriptObject *CreateController(const std::string &type, AngelCore::AngelWorldSystem::Actor *obj);


			template<typename T>
			void RegisterType(const char * _name)
			{
				int r;
				r = engine->RegisterObjectType(_name, sizeof(T), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_C);
				r = this->engine->RegisterObjectBehaviour(_name,
					asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Constructor<T>), asCALL_CDECL_OBJLAST);
				r = this->engine->RegisterObjectBehaviour(_name,
					asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Destructor<T>), asCALL_CDECL_OBJLAST);

				std::string dataS = (std::string)"void f(const " + _name + (std::string)" &in)";
				this->engine->RegisterObjectBehaviour(_name, asBEHAVE_CONSTRUCT, dataS.c_str(), 
					asFUNCTION(CopyConstructor<T>), asCALL_CDECL_OBJLAST);
				dataS = _name + (std::string)" &opAssign(const " + _name + (std::string)"&in)";
				r = engine->RegisterObjectMethod(_name, dataS.c_str(), asMETHODPR(T, operator =,
					(const T&), T&), asCALL_THISCALL);
			}


			void RegisterMethod(const char * className, const char * functionName, const asSFuncPtr &func)
			{
				int r = this->engine->RegisterObjectMethod(className, functionName, func, asCALL_THISCALL);
				assert(r >= 0);
			}

			bool hasCompileErrors;

		public:
			void Update(asIScriptObject *object);
			void Start(asIScriptObject *object);
			void OnCollide(asIScriptObject *object,AngelCore::AngelWorldSystem::Actor *caller);
			void OnSendMessage(asIScriptObject *object, std::string &msg, AngelCore::AngelWorldSystem::Actor *caller);

		public:
			void MessageCallback(const asSMessageInfo &msg);
			asIScriptContext *PrepareContextFromPool(asIScriptFunction *func);
			void ReturnContextToPool(asIScriptContext *ctx);
			int ExecuteCall(asIScriptContext *ctx);
			SController *LoadScript(const std::string &scriptName);

		private:

			asIScriptEngine  *engine;

			// Our pool of script contexts. This is used to avoid allocating
			// the context objects all the time. The context objects are quite
			// heavy weight and should be shared between function calls.
			std::vector<asIScriptContext *> contexts;

			// This is the cache of function ids etc that we use to avoid having
			// to search for the function ids everytime we need to call a function.
			// The search is quite time consuming and should only be done once.
			std::vector<SController *> controllers;

		};
	}
}

