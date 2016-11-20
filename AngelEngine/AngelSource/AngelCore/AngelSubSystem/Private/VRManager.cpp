#include"stdafx.h"
#include"../Public/VRManager.h"
#include<assert.h>
#include"../Public/RenderManager.h"
#include"../Public/SubSystemManager.h"
#include"../Public/TimeManager.h"
#include"../Public/WorldManager.h"
#include"../AngelRendering/Public/PostProcess.h"
#include"../AngelRendering/Public/LightManager.h"
#include"../AngelWorldSystem/Public/ActorFactory.h"

#include<Source\Include.h>

#define MAX_YAW 3.0
#define MIN_YAW -3.0

using namespace DirectX;

AngelCore::AngelSubSystem::VRCamera::VRCamera() : MoveForward(0),
MoveBack(0),
MoveLeft(0),
MoveRight(0),
ComfortTurnSnap(-1.0f),
cameraSpeed(1.0f),
bMotionRelativeToBody(false)
{
	BodyPos.x = Pos.m128_f32[0];
	BodyPos.y = Pos.m128_f32[1];
	BodyPos.z = Pos.m128_f32[2];
};
AngelCore::AngelSubSystem::VRCamera::VRCamera
(XMVECTOR * pos, XMVECTOR * rot) : Pos(*pos), Rot(*rot), MoveForward(0),
MoveBack(0),
MoveLeft(0),
MoveRight(0),
ComfortTurnSnap(-1.0f),
bMotionRelativeToBody(false),
cameraSpeed(0.01f)
{
	BodyPos.x = Pos.m128_f32[0];
	BodyPos.y = Pos.m128_f32[1];
	BodyPos.z = Pos.m128_f32[2];
};
XMMATRIX AngelCore::AngelSubSystem::VRCamera::GetViewMatrix()
{
	XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), Rot);
	return(XMMatrixLookAtLH(Pos, XMVectorAdd(Pos, forward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), Rot)));
}

OVR::Posef AngelCore::AngelSubSystem::VRCamera::VirtualWorldTransformfromRealPose(const OVR::Posef &sensorHeadPose)
{
	using namespace OVR;
	Quatf baseQ = Quatf(Vector3f(0, 1, 0), GetApparentBodyYaw().Get());

	return Posef(baseQ * sensorHeadPose.Rotation,
		BodyPos + baseQ.Rotate(sensorHeadPose.Translation));
}

OVR::Anglef AngelCore::AngelSubSystem::VRCamera::GetApparentBodyYaw()
{
	float yawtmp = 0;
	HeadPose.Rotation.GetYawPitchRoll(&yawtmp, nullptr, nullptr);
	if (yawtmp*HeadPose.Rotation.y > 0)
		sign = 1;
	else
		sign = -1;

	float mul = -2.3f*sign;
	if (abs(HeadPose.Rotation.y) > 0.9f)
	{
		mul = -3.3f*sign;
	}

	OVR::Anglef yaw = BodyYaw + HeadPose.Rotation.y *mul;
	if (ComfortTurnSnap > 0.0f)
	{
		float yawR = yaw.Get();
		yawR *= 1.0f / ComfortTurnSnap;
		yawR = floorf(yawR + 0.5f);
		yawR *= ComfortTurnSnap;
		yaw.Set(yawR);
	}
	return yaw;
}

OVR::Matrix4f AngelCore::AngelSubSystem::VRCamera::CalculateViewFromPose(const OVR::Posef& pose)
{
	OVR::Posef worldPose = VirtualWorldTransformfromRealPose(pose);

	// Rotate and position View Camera
	OVR::Vector3f up = worldPose.Rotation.Rotate(UpVector);
	OVR::Vector3f forward = worldPose.Rotation.Rotate(ForwardVector);

	// Transform the position of the center eye in the real world (i.e. sitting in your chair)
	// into the frame of the player's virtual body.

	OVR::Vector3f viewPos = false ? BodyPos : worldPose.Translation;

	OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(viewPos, viewPos + forward, up);
	return view;
}

OVR::Quatf AngelCore::AngelSubSystem::VRCamera::GetOrientation(bool baseOnly)
{
	using namespace OVR;
	OVR::Quatf baseQ = Quatf(Vector3f(0, 1, 0), GetApparentBodyYaw().Get());
	return true ? baseQ : baseQ * HeadPose.Rotation;
}

void AngelCore::AngelSubSystem::VRCamera::HandleMovement(double dt, bool shiftDown)
{
	
}

AngelCore::AngelSubSystem::VRManager::~VRManager()
{

}

bool AngelCore::AngelSubSystem::VRManager::StartUp(...)
{
	m_enabled = false;
	m_initializedSucced = false;
	ovrResult result = ovr_Initialize(nullptr);
	if (!OVR_SUCCESS(result))
	{
		AngelLog::WriteWarningMessage("VR Headset Is Not Attached Use Debug Device To Render To Screen.");
		MessageBox(NULL,
			L"It Seems You Don't Have Oculus Headset Connected To Computer,If You Want To Test VR Sample Download Oculus Runtime Sdk,After Instalation Open It,Goto->tools->service->configure And From Debug HMD Device Select DK2 And Click Ok. After That ReOpen Program To Test VR Sample.", L"How To Test VR Sample Without Headset", MB_ICONINFORMATION | MB_OK);
		return true;
	}
	result = ovr_Create(&m_HMD, &m_luid);
	if (!OVR_SUCCESS(result))
	{
		AngelLog::WriteWarningMessage("VR Headset Is Not Attached Use Debug Device To Render To Screen.");
		MessageBox(NULL,
			L"It Seems You Don't Have Oculus Headset Connected To Computer,If You Want To Test VR Sample Download Oculus Runtime Sdk,After Instalation Open It,Goto->tools->service->configure And From Debug HMD Device Select DK2 And Click Ok. After That ReOpen Program To Test VR Sample.", L"How To Test VR Sample Without Headset", MB_ICONINFORMATION | MB_OK);
		return true;
	}

	m_hmdDesc = ovr_GetHmdDesc(m_HMD);

	AngelSubSystem::SubSystemManager::GetInstance()->GetRenderManager()->
		InitVR(m_hmdDesc.Resolution.w / 2, m_hmdDesc.Resolution.h / 2, reinterpret_cast<LUID*>(&m_luid));

	m_VRCamera = nullptr;
	m_VRCamera = new VRCamera(&XMVectorSet(0,
		0,
		0, 1), &DirectX::XMQuaternionRotationRollPitchYaw(0, 0, 0));

	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealSize = ovr_GetFovTextureSize(m_HMD, (ovrEyeType)eye, m_hmdDesc.DefaultEyeFov[eye], 1.0f);
		m_pEyeRenderTexture[eye] = new OculusTexture();

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		texDesc.Height = idealSize.h;
		texDesc.Width = idealSize.w;
		texDesc.MipLevels = 1;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
			->CreateTexture2D(&texDesc, NULL, &m_guiTexture));

		AngelRendering::PostProcess::GetInstance()->CreateVRGUITexture(idealSize.w, idealSize.h);

		if (!m_pEyeRenderTexture[eye]->Init(m_HMD, idealSize.w, idealSize.h))
		{
			MessageBox(NULL, L"Failed Creating OVR Texture", L"FAILED", MB_ERROR);
			return false;
		}
		m_pEyeDepthBuffer[eye] = new DepthBuffer
			(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get(), idealSize.w, idealSize.h);
		m_eyeRenderViewport[eye].Pos.x = 0;
		m_eyeRenderViewport[eye].Pos.y = 0;
		m_eyeRenderViewport[eye].Size = idealSize;
		if (!m_pEyeRenderTexture[eye]->TextureSet)
		{
			MessageBox(NULL, L"Failed Creating OVR Texture", L"FAILED", MB_ERROR);
			return false;
		}
	}

	D3D11_TEXTURE2D_DESC td = {};

	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	td.Width = m_hmdDesc.Resolution.w / 2;
	td.Height = m_hmdDesc.Resolution.h / 2;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.SampleDesc.Count = 1;
	td.MipLevels = 1;
	result = ovr_CreateMirrorTextureD3D11(m_HMD,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get(), &td, 0, &m_mirrorTexture);
	if (!OVR_SUCCESS(result))
	{
		MessageBox(NULL, L"Failed Creating Mirror Texture", L"FAILED", MB_ERROR);
		return false;
	}

	m_eyeRenderDesc[0] = ovr_GetRenderDesc(m_HMD, ovrEye_Left, m_hmdDesc.DefaultEyeFov[0]);
	m_eyeRenderDesc[1] = ovr_GetRenderDesc(m_HMD, ovrEye_Right, m_hmdDesc.DefaultEyeFov[1]);

	ovrD3D11Texture* tex =
		(ovrD3D11Texture*)m_mirrorTexture;

	AngelGUISystem::AngelGUI::CreateAdditionalRenderTarget
		(m_guiTexture);
	m_initializedSucced = true;
	return true;
}

bool AngelCore::AngelSubSystem::VRManager::ShutDown()
{
	delete m_pEyeRenderTexture[0];
	delete m_pEyeRenderTexture[1];
	delete m_pEyeDepthBuffer[0];
	delete m_pEyeDepthBuffer[1];
	delete m_VRCamera;
	return true;
}

void AngelCore::AngelSubSystem::VRManager::Render()
{
	{
		m_VRCamera->BodyYaw =
			(AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera
				->GetCameraDirection().y + DirectX::XMConvertToRadians(180));;
		m_VRCamera->Rot = XMQuaternionRotationRollPitchYaw(0,
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera
			->GetCameraDirection().y + DirectX::XMConvertToRadians(180), 0);

		m_VRCamera->Pos = DirectX::XMVectorSet(
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().x,
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().y,
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().z, 1.0f);
		m_VRCamera->HandleMovement(
			AngelSubSystem::SubSystemManager::GetInstance()->GetTimeManager()->GetElapsedSeconds(), false);

		ovrPosef         EyeRenderPose[2];
		ovrVector3f      HmdToEyeViewOffset[2] = { m_eyeRenderDesc[0].HmdToEyeViewOffset,
			m_eyeRenderDesc[1].HmdToEyeViewOffset };
		double frameTime = ovr_GetPredictedDisplayTime(m_HMD, 0);
		// Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
		double           sensorSampleTime = ovr_GetTimeInSeconds();

		ovrTrackingState hmdState = ovr_GetTrackingState(m_HMD, frameTime, ovrTrue);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, HmdToEyeViewOffset, EyeRenderPose);
		m_VRCamera->HeadPose = hmdState.HeadPose.ThePose;

		// Render Scene to Eye Buffers
		{
			for (int eye = 0; eye < 2; ++eye)
			{
				// Increment to use next texture, just before writing
				//Get the pose information in XM format
				XMVECTOR eyeQuat = XMVectorSet(EyeRenderPose[eye].Orientation.x, -EyeRenderPose[eye].Orientation.y,
					-EyeRenderPose[eye].Orientation.z, EyeRenderPose[eye].Orientation.w);
				XMVECTOR eyePos = XMVectorSet(EyeRenderPose[eye].Position.x, EyeRenderPose[eye].Position.y, EyeRenderPose[eye].Position.z, 0);

				// Get view and projection matrices for the Rift camera
				XMVECTOR CombinedPos = XMVectorAdd(m_VRCamera->Pos, XMVector3Rotate(eyePos, m_VRCamera->Rot));

				VRCamera finalCam(&CombinedPos, &(XMQuaternionMultiply(eyeQuat, m_VRCamera->Rot)));

				XMMATRIX view = finalCam.GetViewMatrix();
				ovrMatrix4f p = ovrMatrix4f_Projection(m_eyeRenderDesc[eye].Fov, 0.01f, 1000.0f, ovrProjection_RightHanded);
				XMMATRIX proj = XMMatrixSet(
					p.M[0][0], p.M[1][0], p.M[2][0], p.M[3][0],
					p.M[0][1], p.M[1][1], p.M[2][1], p.M[3][1],
					p.M[0][2], p.M[1][2], p.M[2][2], p.M[3][2],
					p.M[0][3], p.M[1][3], p.M[2][3], p.M[3][3]);

				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetView(view);
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetProjection(proj);

				AngelRendering::LightManager::GetInstance()->RenderLightsShadowMap();

				AngelRendering::PostProcess::GetInstance()->
					RenderCurrentFrameToTexture(AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D().Get());
				AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->
					GetActorFactory()->Render();
				AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->
					GetWorldManager()->Render();
				float blendFactor[] = { 0.75f, 0.0f, 0.75f, 0.0f };
				AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->OMSetBlendState
					(AngelSubSystemResources::GraphicDeviceResources::TransparentBlending.Get(), blendFactor, 0xffffffff);
				AngelGUISystem::AngelGUI::RenderGUI3D();

				AngelGUISystem::AngelGUI::RenderGUI2DUsingAdditionalRenderTarget();

				m_pEyeRenderTexture[eye]->AdvanceToNextTexture();

				// Clear and set up rendertarget
				int texIndex = m_pEyeRenderTexture[eye]->TextureSet->CurrentIndex;

				AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetRenderManager()
					->SetAndClearRenderTarget(m_pEyeRenderTexture[eye]->TexRtv[texIndex],
						m_pEyeDepthBuffer[eye]->TexDsv);

				AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext.Get()->CopyResource(
					AngelCore::AngelRendering::PostProcess::GetInstance()->GetGUITexture(),
					m_guiTexture);

				AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetRenderManager()
					->SetViewport((float)m_eyeRenderViewport[eye].Pos.x,
						(float)m_eyeRenderViewport[eye].Pos.y,
						(float)m_eyeRenderViewport[eye].Size.w, (float)m_eyeRenderViewport[eye].Size.h);

				AngelRendering::PostProcess::GetInstance()->ProcessTechniques();
				AngelRendering::PostProcess::GetInstance()->RenderFinalImageCombined();

				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->
					SetCameraPosition(m_VRCamera->Pos);

			}
		}

		// Initialize our single full screen Fov layer.
		ovrLayerEyeFov ld = {};
		ld.Header.Type = ovrLayerType_EyeFov;
		ld.Header.Flags = ovrLayerFlag_HighQuality;

		for (int eye = 0; eye < 2; ++eye)
		{
			ld.ColorTexture[eye] = m_pEyeRenderTexture[eye]->TextureSet;
			ld.Viewport[eye] = m_eyeRenderViewport[eye];
			ld.Fov[eye] = m_hmdDesc.DefaultEyeFov[eye];
			ld.RenderPose[eye] = EyeRenderPose[eye];
			ld.SensorSampleTime = sensorSampleTime;
		}

		ovrLayerHeader* layers = &ld.Header;
		ovrResult result = ovr_SubmitFrame(m_HMD, 0, nullptr, &layers, 1);
		// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
		if (!OVR_SUCCESS(result))
		{
			AngelLog::WriteErrorMessage("Submit VR Frame Failed");
		}
	}
	ovrD3D11Texture* tex = (ovrD3D11Texture*)m_mirrorTexture;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext.Get()->CopyResource(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::BackBufferVR.Get(), tex->D3D11.pTexture);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::SwapChainVR->Present(0, 0);
}