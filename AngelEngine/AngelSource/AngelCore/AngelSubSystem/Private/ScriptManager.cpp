#include "../Public/ScriptManager.h"

#include "../../../AngelCore/AngelSubSystem/AngelWorldSystem/Public/ActorFactory.h"

#ifndef __ACTOR
#define __ACTOR
#include "../AngelWorldSystem/Public/Actor.h"
#endif

#include <iostream>  // cout
#include <stdio.h>  // fopen, fclose
#include <string.h> // strcmp
#include <assert.h>
#include "add_on\scriptstdstring\scriptstdstring.h"
#include "add_on\scriptbuilder\scriptbuilder.h"
#include "add_on\weakref\weakref.h"
#include<Source\Include.h>

#ifndef __TRANSFORM
#include"../../AngelSubSystem/AngelWorldSystem/AngelComponent/Public/Transform.h"
#endif

#ifndef __RIGIDBODY
#include"../../AngelSubSystem/AngelWorldSystem/AngelComponent/Public/RigidBody.h"
#endif

#ifndef __AUDIO_HANDLER
#include"../../AngelSubSystem/AngelWorldSystem/AngelComponent/Public/AudioHandler.h"
#endif

#include"../../AngelSubSystem/AngelRendering/Public/BaseCamera.h"
#include"../AngelWorldSystem/AngelComponent/Public/VideoRenderer.h"
#include"../AngelWorldSystem/AngelComponent/Public/CharacterController.h"
#include"../AngelWorldSystem/AngelComponent/Public/LightRenderer.h"

#ifndef __PARTICLE_RENDERER
#define __PARTICLE_RENDERER
#include"../AngelWorldSystem/AngelComponent/Public/ParticleRenderer.h"
#endif

#include"../../AngelSubSystem/Public/SubSystemManager.h"
#include"../../AngelSubSystem/Public/LevelManager.h"
#include"../../AngelSubSystem/Public/RenderManager.h"
#include"../../AngelSubSystem/Public/WorldManager.h"

#include"add_on\debugger\debugger.h"
#include"../Public/InputManager.h"

using namespace std;

AngelCore::AngelSubSystem::ScriptManager::ScriptManager()
{
	engine = 0;
	hasCompileErrors = false;
}

//---- Global Script Functions
void Print(std::string&);
void Print(float&);
DirectX::XMFLOAT4X4 GetRotationMatrix(float &pitch, float &yaw, float &roll);
DirectX::XMFLOAT4X4 MultiplyMatrix(DirectX::XMFLOAT4X4 &m1, DirectX::XMFLOAT4X4 &m2);
Vector3 TransformCoord(Vector3 &vec, DirectX::XMFLOAT4X4 &mat);
Vector3 TransformCoordNormal(Vector3 &vec, DirectX::XMFLOAT4X4 &mat);
Vector3 Cross(Vector3 &vec1, Vector3 &vec2);
float	ToRadian(float &degree);
AngelGUISystem::GUI2D::AGButton * InstantiateButton();
AngelGUISystem::GUI2D::AGText * InstantiateText();
AngelGUISystem::GUI2D::AGCheckBox * InstantiateCheckBox();
AngelGUISystem::GUI2D::AGTab * InstantiateTab();
AngelGUISystem::GUI2D::AGTabGroup * InstantiateTabGroup();
AngelGUISystem::GUI3D::AGText * Instantiate3DText();
AngelGUISystem::GUI3D::AGButton * Instantiate3DButton();
std::string ToString(float &data);
std::string ToString(int &data);
float GetWindowHeight();
float GetWindowWidth();
void Quit();
float GetRandomNumber();
float GetRandomNumber(float &a, float &b);

//---------------------------

AngelCore::AngelSubSystem::ScriptManager::~ScriptManager()
{

}

bool AngelCore::AngelSubSystem::ScriptManager::ShutDown()
{
	for (unsigned int n = 0; n < controllers.size(); n++)
		delete controllers[n];

	for (unsigned int n = 0; n < contexts.size(); n++)
		contexts[n]->Release();

	//if (engine)
		//engine->ShutDownAndRelease();
	return true;
}

asIScriptFunction *callback = 0;
void SetCallback(asIScriptFunction *cb)
{
	// Release the previous callback, if any
	if (callback)
		callback->Release();
	// Store the received handle for later use
	callback = cb;
	//callback->
	// Find the cached onThink method id 
	//SController *ctrl = reinterpret_cast<SController*>(object->GetObjectType()->GetUserData());

	// Call the method using the shared context
	//if (ctrl->Update != 0)
	{
		asIScriptContext *ctx = 
			AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->PrepareContextFromPool(callback);
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->ExecuteCall(ctx);
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetScriptManager()->ReturnContextToPool(ctx);

	}
	// Do not release the received script function 
	// until it won't be used any more
}

bool AngelCore::AngelSubSystem::ScriptManager::StartUp(...)
{
	int r;

	engine = asCreateScriptEngine();

	// Set the message callback to print the human readable messages that the engine gives in case of errors
	r = engine->SetMessageCallback(asMETHOD(AngelCore::AngelSubSystem::ScriptManager, MessageCallback), this, asCALL_THISCALL); assert(r >= 0);

	// Register the string type
	RegisterStdString(engine);

	// Register the generic handle type, called 'ref' in the script
	RegisterScriptHandle(engine);

	// Register the weak ref template type
	RegisterScriptWeakRef(engine);

	//-------- Register Types
	RegisterType<AngelCore::AngelWorldSystem::Transform>("Transform");
	RegisterType<AngelCore::AngelWorldSystem::RigidBody>("RigidBody");
	RegisterType<AngelCore::AngelWorldSystem::MeshRenderer>("MeshRenderer");
	RegisterType<AngelCore::AngelWorldSystem::AudioHandler>("AudioHandler");
	RegisterType<AngelCore::AngelWorldSystem::VideoRenderer>("VideoRenderer");
	RegisterType<AngelCore::AngelWorldSystem::CharacterController>("CharacterController");
	RegisterType<AngelCore::AngelWorldSystem::ParticleRenderer>("ParticleRenderer");
	RegisterType<AngelCore::AngelWorldSystem::LightRenderer>("LightRenderer");
	RegisterType<Vector3>("Vector3");
	RegisterType<Vector2>("Vector2");
	RegisterType<Vector4>("Vector4");
	RegisterType<DirectX::XMFLOAT4X4>("Matrix");
	//----------------------------------------

	r = engine->RegisterObjectType("Actor", 0, asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Actor", asBEHAVE_ADDREF, "void f()", 
		asMETHOD(AngelCore::AngelWorldSystem::Actor, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Actor", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelCore::AngelWorldSystem::Actor, Release), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Actor", asBEHAVE_GET_WEAKREF_FLAG, "int &f()",
		asMETHOD(AngelCore::AngelWorldSystem::Actor, GetWeakRefFlag), asCALL_THISCALL); assert(r >= 0);
	
	assert(r >= 0);

	r = engine->RegisterObjectType("InputManager", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	// Register the game manager's methods
	r = engine->RegisterGlobalProperty("InputManager inputManager",
		AngelCore::AngelSubSystem::inputManager); assert(r >= 0);

	r = engine->RegisterObjectType("MainCamera", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	
	r = engine->RegisterGlobalProperty("MainCamera mainCamera",
		AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera); assert(r >= 0);
	r = engine->RegisterObjectType("ActorFactory", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	
	r = engine->RegisterGlobalProperty("ActorFactory actorFactory",
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->m_actorFactory); assert(r >= 0);

	r = engine->RegisterObjectType("LevelManager", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	
	r = engine->RegisterGlobalProperty("LevelManager levelManager",
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->m_levelManager); assert(r >= 0);

	r = engine->RegisterObjectType("TimeManager", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);

	r = engine->RegisterGlobalProperty("TimeManager timeManager",
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->m_timeManager); assert(r >= 0);

	r = engine->RegisterObjectType("RenderManager", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	
	r = engine->RegisterGlobalProperty("RenderManager renderManager",
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->m_renderManager); assert(r >= 0);

	r = engine->RegisterObjectType("WorldManager", 0, asOBJ_REF | asOBJ_NOHANDLE); assert(r >= 0);
	
	r = engine->RegisterGlobalProperty("WorldManager worldManager",
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->m_worldManager); assert(r >= 0);

	r = engine->RegisterObjectType("GUIButton",
		sizeof(AngelGUISystem::GUI2D::AGButton), asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUIButton", asBEHAVE_ADDREF, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGButton, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUIButton", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGButton, Release), asCALL_THISCALL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterObjectType("GUIText",
		sizeof(AngelGUISystem::GUI2D::AGText), asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUIText", asBEHAVE_ADDREF, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGText, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUIText", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGText, Release), asCALL_THISCALL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterObjectType("GUICheckBox",
		sizeof(AngelGUISystem::GUI2D::AGCheckBox), asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUICheckBox", asBEHAVE_ADDREF, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGCheckBox, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUICheckBox", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGCheckBox, Release), asCALL_THISCALL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterObjectType("GUITab",
		sizeof(AngelGUISystem::GUI2D::AGTab), asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUITab", asBEHAVE_ADDREF, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGTab, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUITab", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGTab, Release), asCALL_THISCALL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterObjectType("GUITabGroup",
		sizeof(AngelGUISystem::GUI2D::AGTabGroup), asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUITabGroup", asBEHAVE_ADDREF, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGTabGroup, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUITabGroup", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelGUISystem::GUI2D::AGTabGroup, Release), asCALL_THISCALL); assert(r >= 0);
	assert(r >= 0);


	r = engine->RegisterObjectType("GUI3DText",
		sizeof(AngelGUISystem::GUI3D::AGText), asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUI3DText", asBEHAVE_ADDREF, "void f()",
		asMETHOD(AngelGUISystem::GUI3D::AGText, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUI3DText", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelGUISystem::GUI3D::AGText, Release), asCALL_THISCALL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterObjectType("GUI3DButton",
		sizeof(AngelGUISystem::GUI3D::AGButton), asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUI3DButton", asBEHAVE_ADDREF, "void f()",
		asMETHOD(AngelGUISystem::GUI3D::AGButton, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUI3DButton", asBEHAVE_RELEASE, "void f()",
		asMETHOD(AngelGUISystem::GUI3D::AGButton, Release), asCALL_THISCALL); assert(r >= 0);
	assert(r >= 0);


	//------ Register Properties
	r = engine->RegisterObjectProperty("Vector3", "float x", asOFFSET(Vector3, m_x));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Vector3", "float y", asOFFSET(Vector3, m_y));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Vector3", "float z", asOFFSET(Vector3, m_z));

	engine->RegisterObjectMethod("Vector3", "Vector3 &opSub(const Vector3 &in)", 
		asMETHODPR(Vector3, operator-, (const Vector3&), Vector3&), asCALL_THISCALL);

	engine->RegisterObjectMethod("Vector3", "Vector3 &opAdd(const Vector3 &in)",
		asMETHODPR(Vector3, operator+, (const Vector3&), Vector3&), asCALL_THISCALL);

	engine->RegisterObjectMethod("Vector3", "Vector3 Multiply(float &in)",
		asMETHOD(Vector3, Multiply), asCALL_THISCALL);


	r = engine->RegisterObjectProperty("Vector2", "float x", asOFFSET(Vector2, m_x));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Vector2", "float y", asOFFSET(Vector2, m_y));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Vector4", "float x", asOFFSET(Vector4, m_x));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Vector4", "float y", asOFFSET(Vector4, m_y));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Vector4", "float z", asOFFSET(Vector4, m_z));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Vector4", "float w", asOFFSET(Vector4, m_w));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Matrix", "float M11",
		asOFFSET(DirectX::XMFLOAT4X4,_11));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M12",
		asOFFSET(DirectX::XMFLOAT4X4, _12));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M13", 
		asOFFSET(DirectX::XMFLOAT4X4, _13));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M14", 
		asOFFSET(DirectX::XMFLOAT4X4,_14));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Matrix", "float M21",
		asOFFSET(DirectX::XMFLOAT4X4, _21));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M22",
		asOFFSET(DirectX::XMFLOAT4X4, _22));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M23",
		asOFFSET(DirectX::XMFLOAT4X4, _23));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M24",
		asOFFSET(DirectX::XMFLOAT4X4, _24));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Matrix", "float M31",
		asOFFSET(DirectX::XMFLOAT4X4, _31));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M32",
		asOFFSET(DirectX::XMFLOAT4X4, _32));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M33",
		asOFFSET(DirectX::XMFLOAT4X4, _33));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M34",
		asOFFSET(DirectX::XMFLOAT4X4, _34));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Matrix", "float M41",
		asOFFSET(DirectX::XMFLOAT4X4, _41));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M42",
		asOFFSET(DirectX::XMFLOAT4X4, _42));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M43",
		asOFFSET(DirectX::XMFLOAT4X4, _43));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Matrix", "float M44",
		asOFFSET(DirectX::XMFLOAT4X4, _44));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Transform", "Vector3 &position",
		asOFFSET(AngelCore::AngelWorldSystem::Transform, 
			m_position));

	r = engine->RegisterObjectProperty("Transform", "Vector3 &rotation", 
		asOFFSET(AngelCore::AngelWorldSystem::Transform,
		m_rotation));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Transform", "Vector3 &scalling",
		asOFFSET(AngelCore::AngelWorldSystem::Transform,
		m_scalling));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Actor", "Transform &transform", 
		asOFFSET(AngelCore::AngelWorldSystem::Actor,m_transform));
	assert(r >= 0);
	r = engine->RegisterObjectProperty("Actor", "RigidBody &rigidBody",
		asOFFSET(AngelCore::AngelWorldSystem::Actor, m_rigidBody));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Actor", "VideoRenderer &videoRenderer",
		asOFFSET(AngelCore::AngelWorldSystem::Actor, m_videoRenderer));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Actor", "MeshRenderer &meshRenderer",
		asOFFSET(AngelCore::AngelWorldSystem::Actor, m_meshRenderer));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Actor", "AudioHandler &audioHandler",
		asOFFSET(AngelCore::AngelWorldSystem::Actor, m_audioHandler));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Actor", "CharacterController &characterController",
		asOFFSET(AngelCore::AngelWorldSystem::Actor, m_characterController));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Actor", "ParticleRenderer &particleRenderer",
		asOFFSET(AngelCore::AngelWorldSystem::Actor, m_particleRenderer));
	assert(r >= 0);

	r = engine->RegisterObjectProperty("Actor", "LightRenderer &lightRenderer",
		asOFFSET(AngelCore::AngelWorldSystem::Actor, m_lightRenderer));
	assert(r >= 0);

	//-----------------------------------------

	r = engine->RegisterObjectMethod("RigidBody",
		"void ApplyForce(float &in,float &in,float &in)",
		asMETHOD(AngelCore::AngelWorldSystem::RigidBody, ApplyForce),
		asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("RigidBody",
		"void ApplyImpluse(float &in,float &in,float &in)",
		asMETHOD(AngelCore::AngelWorldSystem::RigidBody, ApplyImpluse),
		asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("RigidBody",
		"void ApplyTorque(float &in,float &in,float &in)",
		asMETHOD(AngelCore::AngelWorldSystem::RigidBody, ApplyTorque),
		asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("RigidBody",
		"void ApplyTorqueImpluse(float &in,float &in,float &in)",
		asMETHOD(AngelCore::AngelWorldSystem::RigidBody, ApplyTorqueImpluse),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("RigidBody",
		"void SetPosition(Vector3 &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::RigidBody,
			SetPosition,(Vector3&),void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("RigidBody",
		"void SetPosition(float &in,float &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::RigidBody, SetPosition
			,(float &,float &,float&),void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("RigidBody",
		"void SetAngularVelocity(Vector3 &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::RigidBody, 
			SetAngularVelocity,(Vector3&),void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("RigidBody",
		"void SetAngularVelocity(float &in,float &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::RigidBody,
			SetAngularVelocity, (float&, float&, float&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("RigidBody",
		"void SetLinearVelocity(Vector3 &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::RigidBody,
			SetLinearVelocity, (Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("RigidBody",
		"void SetLinearVelocity(float &in,float &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::RigidBody,
			SetLinearVelocity, (float&, float&, float&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("RigidBody",
		"void SetCollisionFlag(int &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::RigidBody,
			SetCollisionFlag, (int&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("VideoRenderer",
		"void Load(string &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::VideoRenderer,
			Load, (string&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("VideoRenderer",
		"void Set2DRendering(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::VideoRenderer,
			Set2DRendering, (bool&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("VideoRenderer",
		"void Set3DRendering(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::VideoRenderer,
			Set3DRendering, (bool&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("VideoRenderer",
		"void SetRectangle(float &in,float &in,float &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::VideoRenderer,
			SetRectangle, (float&, float&, float&, float&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("VideoRenderer",
		"void SetBillboard(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::VideoRenderer,
			SetBillboard, (bool&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MeshRenderer",
		"void SetDiffuseColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::MeshRenderer,
			SetDiffuseColor, (float&, float&, float&, float&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MeshRenderer",
		"void SetAmbientColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::MeshRenderer,
			SetAmbientColor, (float&, float&, float&, float&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MeshRenderer",
		"void SetAnimationClipIndex(int &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::MeshRenderer,
			SetAnimationClipIndex, (int &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MeshRenderer",
		"void SetAnimationClipEndTime(int &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::MeshRenderer,
			SetAnimationClipEndTime, (int &,float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MeshRenderer",
		"void SetAnimationClipTimeScale(int &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::MeshRenderer,
			SetAnimationClipTimeScale, (int &, float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MeshRenderer",
		"void DisableAnimationClipEndTime(int &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::MeshRenderer,
			DisableAnimationClipEndTime, (int &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("CharacterController",
		"void MoveForward()",
		asMETHODPR(AngelCore::AngelWorldSystem::CharacterController,
			MoveForward, (), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("CharacterController",
		"void MoveBackward()",
		asMETHODPR(AngelCore::AngelWorldSystem::CharacterController,
			MoveBackward, (), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("CharacterController",
		"void MoveRight()",
		asMETHODPR(AngelCore::AngelWorldSystem::CharacterController,
			MoveRight, (), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("CharacterController",
		"void MoveLeft()",
		asMETHODPR(AngelCore::AngelWorldSystem::CharacterController,
			MoveLeft, (), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("CharacterController",
		"void SetMoveSpeed(float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::CharacterController,
			SetMoveSpeed, (float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("CharacterController",
		"void SetHeightOffset(float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::CharacterController,
			SetHeightOffset, (float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("CharacterController",
		"void SetLocalScale(float &in,float &in,float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::CharacterController,
			SetLocalScale, (float &, float &, float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetLifeTime(float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetLifeTime, (float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetVelocity(Vector3 &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetVelocity, (Vector3 &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetEmitRate(float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetEmitRate, (float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetParticleMinSize(float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetParticleMinSize, (float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetParticleMaxSize(float &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetParticleMaxSize, (float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetRandomInXDirection(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetRandomInXDirection, (bool &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetRandomInYDirection(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetRandomInYDirection, (bool &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetRandomInZDirection(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetRandomInZDirection, (bool &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void SetActive(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			SetActive, (bool &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("ParticleRenderer",
		"void LoadParticleTexture(string &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::ParticleRenderer,
			LoadParticleTexture, (std::string &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("LightRenderer",
		"void SetCastShadow(int &in) const",
		asMETHODPR(AngelCore::AngelWorldSystem::LightRenderer,
			SetCastShadow, (int &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("LightRenderer",
		"void SetColor(float &in,float &in,float &in,float &in) const",
		asMETHODPR(AngelCore::AngelWorldSystem::LightRenderer,
			SetColor, (float &,float &,float &,float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("LightRenderer",
		"void SetType(int &in) const",
		asMETHODPR(AngelCore::AngelWorldSystem::LightRenderer,
			SetType, (int &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("LightRenderer",
		"void SetEnable(bool &in) const",
		asMETHODPR(AngelCore::AngelWorldSystem::LightRenderer,
			SetEnable, (bool &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("LightRenderer",
		"void SetIntensity(float &in) const",
		asMETHODPR(AngelCore::AngelWorldSystem::LightRenderer,
			SetIntensity, (float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Vector3",
		"Vector3 Normalize()",
		asMETHOD(Vector3, Normalize),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("InputManager",
		"bool isKeyPressed(string &in)",
		asMETHODPR(AngelCore::AngelSubSystem::InputManager, IsKeyPressed, (std::string&), bool),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("InputManager",
		"bool isKeyDown(string &in)",
		asMETHODPR(AngelCore::AngelSubSystem::InputManager, IsKeyDown, (std::string&), bool),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("InputManager",
		"bool isKeyUp(string &in)",
		asMETHODPR(AngelCore::AngelSubSystem::InputManager, IsKeyUp, (std::string&), bool),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("InputManager",
		"bool isKeyHeld(string &in)",
		asMETHODPR(AngelCore::AngelSubSystem::InputManager, IsKeyHeld, (std::string&), bool),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("InputManager",
		"Vector2 GetMousePosition()",
		asMETHODPR(AngelCore::AngelSubSystem::InputManager, GetMousePosition, () const, Vector2),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("InputManager",
		"bool IsRightMouseDown()",
		asMETHODPR(AngelCore::AngelSubSystem::InputManager, IsRightMouseClicked, (), bool),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("InputManager",
		"bool IsLeftMouseDown()",
		asMETHODPR(AngelCore::AngelSubSystem::InputManager, IsLeftMouseClicked, (), bool),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void SetCameraRight(Vector3 &in)",
		asMETHODPR(BaseCamera, SetCameraRight, (Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetCameraPosition()",
		asMETHODPR(BaseCamera, GetCameraPosition, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetCameraDirection()",
		asMETHODPR(BaseCamera, GetCameraDirection, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetCameraTarget()",
		asMETHODPR(BaseCamera, GetCameraTarget, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetCameraUp()",
		asMETHODPR(BaseCamera, GetCameraUp, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetDefaultForward()",
		asMETHODPR(BaseCamera, GetDefaultForward, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetCameraForward()",
		asMETHODPR(BaseCamera, GetCameraForward, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetCameraRight()",
		asMETHODPR(BaseCamera, GetCameraRight, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"Vector3 GetDefaultRight()",
		asMETHODPR(BaseCamera, GetDefaultRight, ()const, Vector3),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void SetCameraPosition(Vector3 &in)",
		asMETHODPR(BaseCamera, SetCameraPosition, (Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void SetCameraForward(Vector3 &in)",
		asMETHODPR(BaseCamera, SetCameraForward, (Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void SetCameraUp(Vector3 &in)",
		asMETHODPR(BaseCamera, SetCameraUp, (Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void SetCameraDirection(Vector3 &in)",
		asMETHODPR(BaseCamera, SetCameraDirection, (Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void SetCameraTarget(Vector3 &in)",
		asMETHODPR(BaseCamera, SetCameraTarget, (Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void UpdateViewMatrix(Vector3 &in,Vector3 &in,Vector3 &in)",
		asMETHODPR(BaseCamera, UpdateView,(Vector3&, Vector3&, Vector3&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("MainCamera",
		"void UpdateProjection(float &in)",
		asMETHODPR(BaseCamera, UpdateProjection, (float&), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Actor",
		"void AddComponent(string &in)",
		asMETHOD(AngelCore::AngelWorldSystem::Actor, AddComponent), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Actor",
		"string GetName() const",
		asMETHODPR(AngelCore::AngelWorldSystem::Actor, GetName,() const,std::string), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Actor",
		"string GetTag() const",
		asMETHODPR(AngelCore::AngelWorldSystem::Actor, GetTag, () const, std::string), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Actor",
		"bool GetActive() const",
		asMETHODPR(AngelCore::AngelWorldSystem::Actor, GetActive, () const, bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Actor",
		"void SetName(string &in)",
		asMETHOD(AngelCore::AngelWorldSystem::Actor, SetName), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Actor",
		"void SetActive(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::Actor, SetActive,(bool &),void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("Actor",
		"void SetTag(string &in)",
		asMETHOD(AngelCore::AngelWorldSystem::Actor, SetTag), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"Actor", "void Send(string &in,string &in)", 
		asMETHODPR(AngelCore::AngelWorldSystem::Actor,
			Send,(std::string &, std::string &),void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"Actor", "void Send(string &in,const Actor & in)",
		asMETHODPR(AngelCore::AngelWorldSystem::Actor,
			Send, (std::string &, AngelCore::AngelWorldSystem::Actor &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"Actor", "void SetHidden(bool &in)",
		asMETHODPR(AngelCore::AngelWorldSystem::Actor, SetHidden
			,(bool &),void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"Actor", "bool GetHidden()",
		asMETHODPR(AngelCore::AngelWorldSystem::Actor, GetHidden
			, (), bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"ActorFactory", "Actor @GetActorByName(string &in)",
		asMETHOD(AngelCore::AngelWorldSystem::ActorFactory,
			GetActorByName), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"ActorFactory", "Actor @GetActorByTag(string &in)",
		asMETHOD(AngelCore::AngelWorldSystem::ActorFactory,
			GetActorByTag), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"LevelManager", "void LoadLevel(string &in)",
		asMETHODPR(AngelCore::AngelSubSystem::LevelManager,
			LoadLevel,(std::string &),bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void Play(bool &in,int &in)",
		asMETHOD(AngelCore::AngelWorldSystem::AudioHandler,
			Play), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void Pause(int &in)",
		asMETHOD(
			AngelCore::AngelWorldSystem::AudioHandler,
			Pause), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void Resume(int &in)",
		asMETHOD(
			AngelCore::AngelWorldSystem::AudioHandler,
			Resume), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void Stop(int &in)",
		asMETHOD(
			AngelCore::AngelWorldSystem::AudioHandler,
			Stop), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void SetPan(float &in,int &in)",
		asMETHOD(
			AngelCore::AngelWorldSystem::AudioHandler,
			SetPan), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void SetPitch(float &in,int &in)",
		asMETHOD(
			AngelCore::AngelWorldSystem::AudioHandler,
			SetPitch), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void SetVolume(float &in,int &in)",
		asMETHOD(
			AngelCore::AngelWorldSystem::AudioHandler,
			SetVolume), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"AudioHandler", "void Load(string &in)",
		asMETHOD(
			AngelCore::AngelWorldSystem::AudioHandler,
			Load), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"TimeManager", "float DeltaTime()",
		asMETHOD(
			AngelCore::AngelSubSystem::TimeManager,
			GetElapsedSeconds), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"TimeManager", "float FramePerSecond()",
		asMETHOD(
			AngelCore::AngelSubSystem::TimeManager,
			GetFramesPerSecond), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"TimeManager", "float TotalTime()",
		asMETHOD(
			AngelCore::AngelSubSystem::TimeManager,
			GetTotalSeconds), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"TimeManager", "void SetPause(bool &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::TimeManager,
			SetPause,(bool&),void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"TimeManager", "bool GetPause()",
		asMETHODPR(
			AngelCore::AngelSubSystem::TimeManager,
			GetPause, ()const , bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"RenderManager", "void SetPostProcessMotionBlur(bool &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::RenderManager,
			SetMotionBlurPostProcess, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"RenderManager", "void SetPostProcessBloom(bool &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::RenderManager,
			SetBloomPostProcess, (bool &), void), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod(
		"RenderManager", "void SetPostProcessDepthOfField(bool &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::RenderManager,
			SetDepthOfDieldPostProcess, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"WorldManager", "void SetDynamicWeather(bool &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::WorldManager,
			SetDynamicWeather, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"WorldManager", "void SetDynamicClouds(bool &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::WorldManager,
			SetDynamicClouds, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"WorldManager", "void LoadStaticSkyMap(string &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::WorldManager,
			LoadStaticSkyMap, (string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"WorldManager", "int GetCurrentTime()",
		asMETHODPR(
			AngelCore::AngelSubSystem::WorldManager,
			GetCurrentTime,  ()const, int), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"WorldManager", "void SetTimeScale(float &in)",
		asMETHODPR(
			AngelCore::AngelSubSystem::WorldManager,
			SetTimeScale, (float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"WorldManager", "float GetTimeScale()",
		asMETHODPR(
			AngelCore::AngelSubSystem::WorldManager,
			GetTimeScale, ()const, float), asCALL_THISCALL); assert(r >= 0);

	// Register a simple funcdef for the callback
	engine->RegisterFuncdef("void CallbackFunc()");

	r = engine->RegisterObjectMethod(
		"GUIButton", "void Initialize()",
		asMETHOD(
			AngelGUISystem::GUI2D::AGButton,
			Initialize), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void RegisterFunction(CallbackFunc @cb)",
		asMETHOD(
			AngelGUISystem::GUI2D::AGButton,
			RegisterCallBackFunctionScript), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetPosition(float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetPosition,(float &,float &),void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetBitmapPath(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetBitmapPath, (std::string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetPosition(float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetPosition, (float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetScale(float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetScale, (float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetText(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetText,(std::string&),void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetRounded(bool &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetRounded, (bool&), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetActive(bool &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetActive, (bool&), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetActive(bool &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetActive, (bool&), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetRoundedRadius(float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetRoundedRadius, (float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetTextFontSize(float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetTextFontSize, (float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetRectangle(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetRectangle, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetHoverColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetOnHoverColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetDefaultColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetDefaultColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetClickColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetOnClickColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIButton", "void SetTextColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGButton,
			SetTextColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetText(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetText, (string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetEditable(bool &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetEditable, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetRectangle(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetRectangle, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetTextColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetTextColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetAlignmentDirection(int &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetReadingDirection, (int &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetFontSize(float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetFontSize, (float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetFittingMode(int &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetFittingMode, (int &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetBackgroundColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetBackgroundColor, (float &, float &, float &, float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUIText", "void SetCarretColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGText,
			SetCarretColor, (float &, float &, float &, float &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "void SetText(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			SetText, (string &), void),
		asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "void SetPosition(float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			SetPosition, (float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "void SetRectangle(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			SetRectangle, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "void SetBackgroundColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			SetBackgroundColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "void SetHoverColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			SetOnHoverColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "void SetClickColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			SetOnClickColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "void SetFilledColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			SetFilledColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUICheckBox", "bool GetChecked()",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGCheckBox,
			GetChecked, ()const, bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void SetHeaderText(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			SetText, (string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetHeaderText(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetTabHeaderText, (string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void SetBackgroundColor(float &in, float &in, float &in, float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			SetBackgroundColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void SetTabHeaderTextColor(float &in, float &in, float &in, float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			SetTabHeaderTextColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetTabHeaderTextColor(float &in, float &in, float &in, float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetTabHeaderTextColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void AddItem(GUIButton @button)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			AddItem, (AngelGUISystem::GUI2D::IGUI2D *), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void AddItem(GUIText @text)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			AddItem, (AngelGUISystem::GUI2D::IGUI2D *), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void AddItem(GUICheckBox @checkbox)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			AddItem, (AngelGUISystem::GUI2D::IGUI2D *), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void SetPosition(float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			SetPosition, (float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITab", "void SetRectangle(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTab,
			SetRectangle, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetPosition(float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetPosition, (float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetRectangle(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetRectangle, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void AddTab(GUITab @tab)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			AddTab, (AngelGUISystem::GUI2D::AGTab *), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetTabsDefaultColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetTabsDefaultColor, 
			(float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetTabsHoverColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetTabsHoverColor,
			(float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetTabsClickColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetTabsClickColor,
			(float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "void SetTabsActiveColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			SetTabsActiveColor,
			(float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "float GetPositionX()",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			GetPositionX,
			(), float), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUITabGroup", "float GetPositionY()",
		asMETHODPR(
			AngelGUISystem::GUI2D::AGTabGroup,
			GetPositionY,
			(), float), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetText(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetText, (std::string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetBackgroundColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetCurrentColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetDefaultColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetDefaultColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetBillboard(bool &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetBillboard, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "bool GetBillboard()",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			GetBillboard, ()const, bool), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetTextColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetTextColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetCaretColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetCaretColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetPosition(float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetPosition, (float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetRotation(float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetRotation, (float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetScale(float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetScale, (float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetFontSize(float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetFontSize, (float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DText", "void SetEditable(bool &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGText,
			SetEditable, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetText(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetText, (std::string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetTextColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetTextColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);


	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetTextFontSize(float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetTextFontSize, (float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetHoverColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetOnHoverColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetDefaultColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetDefaultColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetClickColor(float &in,float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetOnClickColor, (float &, float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetBitmapPath(string &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetBitmapPath, (std::string &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void RegisterFunction(CallbackFunc @cb)",
		asMETHOD(
			AngelGUISystem::GUI3D::AGButton,
			RegisterCallBackFunctionScript), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetPosition(float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetPosition, (float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetRotation(float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetRotation, (float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetScale(float &in,float &in,float &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetScale, (float &, float &, float &), void), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod(
		"GUI3DButton", "void SetBillboard(bool &in)",
		asMETHODPR(
			AngelGUISystem::GUI3D::AGButton,
			SetBillboard, (bool &), void), asCALL_THISCALL); assert(r >= 0);

	//------- Register Global Functions
	r = engine->RegisterGlobalFunction("void DebugString(float &in)",
		asFUNCTIONPR(Print, (float&), void), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction("void DebugString(string &in)",
		asFUNCTIONPR(Print, (string&), void), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"Matrix GetRotationMatrix(float &in,float &in,float &in)",
		asFUNCTION(GetRotationMatrix), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"Matrix MultiplyMatrix(Matrix &in,Matrix &in)",
		asFUNCTION(MultiplyMatrix), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"Vector3 TransformCoord(Vector3 &in,Matrix &in)",
		asFUNCTION(TransformCoord), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"Vector3 TransformCoordNormal(Vector3 &in,Matrix &in)",
		asFUNCTION(TransformCoordNormal), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"Vector3 Cross(Vector3 &in,Vector3 &in)",
		asFUNCTION(Cross), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"float ToRadian(float &in)",
		asFUNCTION(ToRadian), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"GUIButton @CreateButton()",
		asFUNCTION(InstantiateButton), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"GUIText @CreateText()",
		asFUNCTION(InstantiateText), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"GUICheckBox @CreateCheckBox()",
		asFUNCTION(InstantiateCheckBox), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"GUITab @CreateTab()",
		asFUNCTION(InstantiateTab), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"GUITabGroup @CreateTabGroup()",
		asFUNCTION(InstantiateTabGroup), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"GUI3DText @Create3DText()",
		asFUNCTION(Instantiate3DText), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"GUI3DButton @Create3DButton()",
		asFUNCTION(Instantiate3DButton), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"string ToString(float &in)",
		asFUNCTIONPR(ToString,(float &),std::string), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"string ToString(int &in)",
		asFUNCTIONPR(ToString, (int &), std::string), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"float GetRandomNumber()",
		asFUNCTIONPR(GetRandomNumber, (), float), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"float GetRandomNumber(float &in,float &in)",
		asFUNCTIONPR(GetRandomNumber, (float &a,float &b), float), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"float WindowHeight()",
		asFUNCTION(GetWindowHeight), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"float WindowWidth()",
		asFUNCTION(GetWindowWidth), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	r = engine->RegisterGlobalFunction(
		"void Quit()",
		asFUNCTION(Quit), asCALL_CDECL); assert(r >= 0);
	assert(r >= 0);

	
	//---------------------------------

	r = engine->RegisterInterface("IController"); assert(r >= 0);

	// The game engine will determine the class that represents the controller
	// by checking if the class implements the IController interface. No methods
	// are registered for this interface, as the script shouldn't be required to
	// implement the methods. This will allow the game engine to avoid calling
	// methods that doesn't do anything, thus improving performance.
	r = engine->RegisterInterface("Component"); assert(r >= 0);
	assert(r >= 0);

	return true;
}

void AngelCore::AngelSubSystem::ScriptManager::MessageCallback(const asSMessageInfo &msg)
{
	const char *type = "ERR ";
	if (msg.type == asMSGTYPE_WARNING)
		type = "WARN  ";
	else if (msg.type == asMSGTYPE_INFORMATION)
		type = "INFO  ";
	std::string err = type;
	err += msg.section;
	err += " (";
	err += std::to_string(msg.row);
	err += ", ";
	err += to_string(msg.col);
	err += ") : ";
	err += type;
	err += " : ";
	err += msg.message;
	err += "\n";
	MessageBoxA(NULL, err.c_str(), "Script error", MB_ERROR);
	//terminate();
	if (msg.type == asMSGTYPE_ERROR)
		hasCompileErrors = true;
}

AngelCore::AngelSubSystem::ScriptManager::SController * AngelCore::AngelSubSystem::ScriptManager::LoadScript
	(const string &script)
{
	int r;
	std::wstring scriptName(script.begin(), script.end());
	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	path += L"\\AngelData\\Script\\";
	path += scriptName;
	string scriptNameString(path.begin(), path.end());
	// Find the cached controller
	for (unsigned int n = 0; n < controllers.size(); n++)
	{
		if (controllers[n]->module == scriptNameString)
			return controllers[n];
	}

	// No controller, check if the script has already been loaded
	asIScriptModule *mod = engine->GetModule(scriptNameString.c_str(), asGM_ONLY_IF_EXISTS);
	if (mod)
	{
		// We've already attempted loading the script before, but there is no controller
		return 0;
	}

	// Compile the script into the module
	CScriptBuilder builder;
	r = builder.StartNewModule(engine, scriptNameString.c_str());
	if (r < 0)
		return 0;

	// If the script file doesn't exist, then there is no script controller for this type
	FILE *f;
	if ((f = fopen((scriptNameString + ".as").c_str(), "r")) == 0)
		return 0;
	fclose(f);

	// Let the builder load the script, and do the necessary pre-processing (include files, etc)
	r = builder.AddSectionFromFile((scriptNameString + ".as").c_str());
	if (r < 0)
		return 0;

	r = builder.BuildModule();
	if (r < 0)
		return 0;

	// Cache the functions and methods that will be used
	SController *ctrl = new SController;
	controllers.push_back(ctrl);
	ctrl->module = scriptNameString;

	// Find the class that implements the IController interface
	mod = engine->GetModule(scriptNameString.c_str(), asGM_ONLY_IF_EXISTS);
	asITypeInfo *type = 0;
	int tc = mod->GetObjectTypeCount();
	for (int n = 0; n < tc; n++)
	{
		bool found = false;
		type = mod->GetObjectTypeByIndex(n);
		int ic = type->GetInterfaceCount();
		for (int i = 0; i < ic; i++)
		{
			if (strcmp(type->GetInterface(i)->GetName(), "IController") == 0)
			{
				found = true;
				break;
			}
		}

		if (found == true)
		{
			ctrl->type = type;
			break;
		}
	}

	if (ctrl->type == 0)
	{
		cout << "Couldn't find the controller class for the type '" << scriptNameString << "'" << endl;
		controllers.pop_back();
		delete ctrl;
		return 0;
	}

	// Find the factory function
	// The game engine will pass in the owning CGameObj to the controller for storage
	string s = string(type->GetName()) + "@ " + string(type->GetName()) + "(Actor @)";
	ctrl->factoryFunc = type->GetFactoryByDecl(s.c_str());
	if (ctrl->factoryFunc == 0)
	{
		cout << "Couldn't find the appropriate factory for the type '" << scriptNameString << "'" << endl;
		controllers.pop_back();
		delete ctrl;
		return 0;
	}

	// Find the optional event handlers
	ctrl->Update = type->GetMethodByDecl("void Update()");
	ctrl->Start = type->GetMethodByDecl("void Start()");
	ctrl->OnCollide = type->GetMethodByDecl("void OnCollide(const Actor &sender)");
	ctrl->onMessage = type->GetMethodByDecl("void OnMessage(string, const Actor &)");

	// Add the cache as user data to the type for quick access
	type->SetUserData(ctrl);

	return ctrl;
}

asIScriptObject *AngelCore::AngelSubSystem::ScriptManager::CreateController(const string &script, AngelCore::AngelWorldSystem::Actor *gameObj)
{
	int r;
	asIScriptObject *obj = 0;

	SController *ctrl = LoadScript(script);
	if (ctrl == 0) return 0;

	// Create the object using the factory function
	asIScriptContext *ctx = PrepareContextFromPool(ctrl->factoryFunc);

	// Pass the object pointer to the script function. With this call the 
	// context will automatically increase the reference count for the object.
	ctx->SetArgObject(0, gameObj);

	// Make the call and take care of any errors that may happen
	r = ExecuteCall(ctx);
	if (r == asEXECUTION_FINISHED)
	{
		// Get the newly created object
		obj = *((asIScriptObject**)ctx->GetAddressOfReturnValue());

		// Since a reference will be kept to this object 
		// it is necessary to increase the ref count
		obj->AddRef();
	}

	// Return the context to the pool so it can be reused
	ReturnContextToPool(ctx);

	return obj;
}
void AngelCore::AngelSubSystem::ScriptManager::Update(asIScriptObject *object)
{
	// Find the cached onThink method id 
		SController *ctrl = reinterpret_cast<SController*>(object->GetObjectType()->GetUserData());

		// Call the method using the shared context
		if (ctrl->Update != 0)
		{
			asIScriptContext *ctx = PrepareContextFromPool(ctrl->Update);
			ctx->SetObject(object);
			ExecuteCall(ctx);
			ReturnContextToPool(ctx);
		}
}

void AngelCore::AngelSubSystem::ScriptManager::Start(asIScriptObject *object)
{
	// Find the cached onThink method id 
	SController *ctrl = reinterpret_cast<SController*>(object->GetObjectType()->GetUserData());

	// Call the method using the shared context
	if (ctrl->Start != 0)
	{
		asIScriptContext *ctx = PrepareContextFromPool(ctrl->Start);
		ctx->SetObject(object);
		ExecuteCall(ctx);
		ReturnContextToPool(ctx);
	}
}

void AngelCore::AngelSubSystem::ScriptManager::OnCollide(asIScriptObject *object,
	AngelCore::AngelWorldSystem::Actor *caller)
{
	// Find the cached onMessage method id
	SController *ctrl = reinterpret_cast<SController*>(object->GetObjectType()->GetUserData());

	// Call the method using the shared context
	if (ctrl->OnCollide != 0)
	{
		asIScriptContext *ctx = PrepareContextFromPool(ctrl->OnCollide);
		ctx->SetObject(object);
		ctx->SetArgObject(0, caller);
		ExecuteCall(ctx);
		ReturnContextToPool(ctx);
	}
}

void AngelCore::AngelSubSystem::ScriptManager::OnSendMessage(asIScriptObject *object, 
	std::string &msg, AngelCore::AngelWorldSystem::Actor *caller)
{
	// Find the cached onMessage method id
	SController *ctrl = reinterpret_cast<SController*>(object->GetObjectType()->GetUserData());

	// Call the method using the shared context
	if (ctrl->onMessage != 0)
	{
		asIScriptContext *ctx = PrepareContextFromPool(ctrl->onMessage);
		ctx->SetObject(object);
		ctx->SetArgObject(0, &msg);
		ctx->SetArgObject(1, caller);
		ExecuteCall(ctx);
		ReturnContextToPool(ctx);
	}
}

int AngelCore::AngelSubSystem::ScriptManager::ExecuteCall(asIScriptContext *ctx)
{
	int r = ctx->Execute();
	if (r != asEXECUTION_FINISHED)
	{
		if (r == asEXECUTION_EXCEPTION)
		{
			cout << "Exception: " << ctx->GetExceptionString() << endl;
			cout << "Function: " << ctx->GetExceptionFunction()->GetDeclaration() << endl;
			cout << "Line: " << ctx->GetExceptionLineNumber() << endl;

			// It is possible to print more information about the location of the 
			// exception, for example the call stack, values of variables, etc if 
			// that is of interest.
		}
	}

	return r;
}

asIScriptContext * AngelCore::AngelSubSystem::ScriptManager::PrepareContextFromPool(asIScriptFunction *func)
{
	asIScriptContext *ctx = 0;
	if (contexts.size())
	{
		ctx = *contexts.rbegin();
		contexts.pop_back();
	}
	else
		ctx = engine->CreateContext();

	int r = ctx->Prepare(func); assert(r >= 0);

	return ctx;
}

void AngelCore::AngelSubSystem::ScriptManager::ReturnContextToPool(asIScriptContext *ctx)
{
	contexts.push_back(ctx);

	// Unprepare the context to free any objects that might be held
	// as we don't know when the context will be used again.
	ctx->Unprepare();
}

//------ Global functions implementation
void Print(string &str)
{
	OutputDebugStringA(str.c_str());
}

void Print(float &str)
{
	OutputDebugStringA(std::to_string(str).c_str());
}

DirectX::XMFLOAT4X4 GetRotationMatrix(float &pitch, float &yaw, float &roll)
{
	DirectX::XMFLOAT4X4 ret;
	DirectX::XMStoreFloat4x4(&ret, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
	return ret;
}


DirectX::XMFLOAT4X4 MultiplyMatrix(DirectX::XMFLOAT4X4 &m1, DirectX::XMFLOAT4X4 &m2)
{
	DirectX::XMFLOAT4X4 ret;
	DirectX::XMStoreFloat4x4(&ret,DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m1)
		, DirectX::XMLoadFloat4x4(&m2)));
	return ret;
}

Vector3 TransformCoord(Vector3 &vec,DirectX::XMFLOAT4X4 &mat)
{
	DirectX::XMVECTOR tmp =
		DirectX::XMVector3TransformCoord(vec.GetXMVECTOR(), DirectX::XMLoadFloat4x4(&mat));
	Vector3 ret(tmp);
	return ret;
}

Vector3 TransformCoordNormal(Vector3 &vec, DirectX::XMFLOAT4X4 &mat)
{
	DirectX::XMVECTOR tmp =
		DirectX::XMVector3TransformNormal(vec.GetXMVECTOR(), DirectX::XMLoadFloat4x4(&mat));
	Vector3 ret(tmp);
	return ret;
}

Vector3 Cross(Vector3 &vec1, Vector3 &vec2)
{
	return DirectX::XMVector3Cross(vec1.GetXMVECTOR(), vec2.GetXMVECTOR());
}

float  ToRadian(float &degree)
{
	return DirectX::XMConvertToRadians(degree);
}

AngelGUISystem::GUI2D::AGButton * InstantiateButton()
{
	AngelGUISystem::GUI2D::AGButton * b = new AngelGUISystem::GUI2D::AGButton();
	b->Initialize();
	return b;
}

AngelGUISystem::GUI2D::AGText * InstantiateText()
{
	AngelGUISystem::GUI2D::AGText * b = new AngelGUISystem::GUI2D::AGText();
	b->Initialize();
	return b;
}

AngelGUISystem::GUI2D::AGCheckBox * InstantiateCheckBox()
{
	AngelGUISystem::GUI2D::AGCheckBox * b = new AngelGUISystem::GUI2D::AGCheckBox();
	b->Initialize();
	return b;
}


AngelGUISystem::GUI3D::AGText * Instantiate3DText()
{
	AngelGUISystem::GUI3D::AGText * b = new AngelGUISystem::GUI3D::AGText();
	b->Initialize();
	return b;
}

AngelGUISystem::GUI3D::AGButton * Instantiate3DButton()
{
	AngelGUISystem::GUI3D::AGButton * b = new AngelGUISystem::GUI3D::AGButton();
	b->Initialize();
	return b;
}


AngelGUISystem::GUI2D::AGTab * InstantiateTab()
{
	AngelGUISystem::GUI2D::AGTab * b = new AngelGUISystem::GUI2D::AGTab();
	b->Initialize();
	return b;
}


AngelGUISystem::GUI2D::AGTabGroup * InstantiateTabGroup()
{
	AngelGUISystem::GUI2D::AGTabGroup * b = new AngelGUISystem::GUI2D::AGTabGroup();
	b->Initialize();
	return b;
}

std::string ToString(float &data)
{
	return std::to_string(data);
}

std::string ToString(int &data)
{
	return std::to_string(data);
}

float GetWindowHeight()
{
	return AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
}
float GetWindowWidth()
{
	return AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();
}

void Quit()
{
	PostQuitMessage(0);
}

float GetRandomNumber()
{
	return RandF();
}
float GetRandomNumber(float &a, float &b)
{
	float ret;
	ret = RandF(a, b);
	return ret;
}
//------------------------------

