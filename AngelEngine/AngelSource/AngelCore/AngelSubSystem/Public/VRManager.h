#pragma once
#include<OVR_CAPI_D3D.h>
#include<memory>
#include"RenderManager.h"
#include<OVR\Extras\OVR_Math.h>
#include"InputManager.h"
#include"SubSystemBase.h"

const OVR::Vector3f	RightVector(1.0f, 0.0f, 0.0f);
const OVR::Vector3f	UpVector(0.0f, 1.0f, 0.0f);
const OVR::Vector3f	ForwardVector(0.0f, 0.0f, -1.0f); // -1 because HMD looks along -Z at identity orientation

const float		YawInitial = 0.0f;
const float		Sensitivity = 0.3f; // low sensitivity to ease people into it gently.
const float		MoveSpeed = 3.0f; // m/s

#define TEXTURE_COUNT 2

using namespace DirectX;
namespace AngelCore
{
	namespace AngelSubSystem
	{
		class VRCamera
		{
		public:
			XMVECTOR Pos;
			XMVECTOR Rot;
			OVR::Posef HeadPose;
			OVR::Anglef  BodyYaw;
			OVR::Vector3f    BodyPos;
			float cameraSpeed;
			VRCamera();
			VRCamera(XMVECTOR * pos, XMVECTOR * rot);
			XMMATRIX GetViewMatrix();
			static void* operator new(std::size_t size)
			{
				return _aligned_malloc(sizeof(VRCamera), __alignof(VRCamera));
			}

				static void operator delete(void* p)
			{
				_aligned_free(p);
			}
			
			OVR::Posef  VirtualWorldTransformfromRealPose(const OVR::Posef &sensorHeadPose);
			OVR::Anglef GetApparentBodyYaw();
			OVR::Matrix4f CalculateViewFromPose(const OVR::Posef& pose);
			OVR::Quatf GetOrientation(bool baseOnly = false);
			void HandleMovement(double dt, bool shiftDown);

		private:
			uint8_t     MoveForward;
			uint8_t     MoveBack;
			uint8_t     MoveLeft;
			uint8_t     MoveRight;
			OVR::Vector3f    GamepadMove, GamepadRotate;
			bool m_updateAABB;
			bool        bMotionRelativeToBody;
			float       ComfortTurnSnap;
			int sign = 1;
		};
		class VRManager : public ISubSystem
		{
		public:
			struct OculusTexture
			{
				ovrHmd                   hmd;
				ovrSwapTextureSet      * TextureSet;
				static const int         TextureCount = 2;
				ID3D11RenderTargetView * TexRtv[TextureCount];

				OculusTexture() :
					hmd(nullptr),
					TextureSet(nullptr)
				{
					TexRtv[0] = TexRtv[1] = nullptr;
				}

				bool Init(ovrHmd _hmd, int sizeW, int sizeH)
				{
					hmd = _hmd;

					D3D11_TEXTURE2D_DESC dsDesc;
					dsDesc.Width = sizeW;
					dsDesc.Height = sizeH;
					dsDesc.MipLevels = 1;
					dsDesc.ArraySize = 1;
					dsDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
					dsDesc.SampleDesc.Count = 1;   // No multi-sampling allowed
					dsDesc.SampleDesc.Quality = 0;
					dsDesc.Usage = D3D11_USAGE_DEFAULT;
					dsDesc.CPUAccessFlags = 0;
					dsDesc.MiscFlags = 0;
					dsDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

					ovrResult result = ovr_CreateSwapTextureSetD3D11(hmd,
						AngelSubSystemResources::GraphicDeviceResources::Device.Get()
						, &dsDesc, ovrSwapTextureSetD3D11_Typeless, &TextureSet);
					if (!OVR_SUCCESS(result))
					{
						MessageBox(NULL, L"Failed Create Occulus Texture", L"Failed", MB_OK);
						return false;
					}
					assert(TextureSet->TextureCount == TextureCount);

					for (int i = 0; i < TextureCount; ++i)
					{
						ovrD3D11Texture* tex = (ovrD3D11Texture*)&TextureSet->Textures[i];
						D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
						rtvd.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
						rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
						HR(AngelSubSystemResources::GraphicDeviceResources::Device
							->CreateRenderTargetView(tex->D3D11.pTexture, &rtvd, &TexRtv[i]));
					}

					return true;
				}

				~OculusTexture()
				{
					for (int i = 0; i < TextureCount; ++i)
					{
						TexRtv[i]->Release();
						TexRtv[i] = nullptr;
					}
					if (TextureSet)
					{
						ovr_DestroySwapTextureSet(hmd, TextureSet);
					}
				}

				void AdvanceToNextTexture()
				{
					TextureSet->CurrentIndex = (TextureSet->CurrentIndex + 1) % TextureSet->TextureCount;
				}
			};
			struct DepthBuffer
			{
				ID3D11DepthStencilView * TexDsv;

				DepthBuffer(ID3D11Device * Device, int sizeW, int sizeH, int sampleCount = 1)
				{
					DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					D3D11_TEXTURE2D_DESC dsDesc;
					dsDesc.Width = sizeW;
					dsDesc.Height = sizeH;
					dsDesc.MipLevels = 1;
					dsDesc.ArraySize = 1;
					dsDesc.Format = format;
					dsDesc.SampleDesc.Count = sampleCount;
					dsDesc.SampleDesc.Quality = 0;
					dsDesc.Usage = D3D11_USAGE_DEFAULT;
					dsDesc.CPUAccessFlags = 0;
					dsDesc.MiscFlags = 0;
					dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					ID3D11Texture2D * Tex;
					Device->CreateTexture2D(&dsDesc, NULL, &Tex);
					Device->CreateDepthStencilView(Tex, NULL, &TexDsv);
				}
				~DepthBuffer()
				{
					TexDsv->Release();
					TexDsv = nullptr;
				}
			};


		public:
			bool StartUp(...) override;
			bool ShutDown() override;
			~VRManager();
			void Render();
			

		public:
			void SetEnable(bool b) { this->m_enabled = b; }
			bool GetEnable() const { return m_enabled; }
			bool GetInitializedSucced() const { return m_initializedSucced; }
		public:
			ovrTexture     *	m_mirrorTexture;
			ID3D11Texture2D *   m_guiTexture;
			OculusTexture  *	m_pEyeRenderTexture[2];
			DepthBuffer    *	m_pEyeDepthBuffer[2];
			ovrHmd				m_HMD;
			ovrGraphicsLuid		m_luid;
			bool                m_key[256];
			ovrEyeRenderDesc	m_eyeRenderDesc[2];
			ovrRecti			m_eyeRenderViewport[2];
			ovrHmdDesc			m_hmdDesc;
			VRCamera         *	m_VRCamera;
			bool                HmdDisplayAcquired;
			bool				m_enabled;
			//Check wether the device intialized correctly for later use or not.
			bool				m_initializedSucced;
		};
	}
}