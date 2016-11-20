#pragma once
#include"IComponent.h"

#ifndef _SCRIPT_MANAGER
#define _SCRIPT_MANAGER
#include"../../../Public/ScriptManager.h"
#endif

#ifndef __MEDIA_MANAGER
#define __MEDIA_MANAGER
#include"../../../Public/MediaManager.h"
#endif

#include"../../../AngelRendering/Public/Shader.h"
#include<Source\Include.h>

#define __VIDEO_RENDERER

namespace AngelCore
{
	class AngelSubSystem::ScriptManager;

	namespace AngelWorldSystem
	{
		class VideoRenderer : public IComponent, public AngelCore::AngelSubSystem::MediaEngineNotifyCallback
		{
			friend class AngelSubSystem::ScriptManager;
			friend class Actor;
			friend class MeshRenderer;

			struct VertexTranslation
			{
				DirectX::XMFLOAT4X4 vertexPositive1Positive1Tranlation;
				DirectX::XMFLOAT4X4 vertexNegative1Negative1Tranlation;
				DirectX::XMFLOAT4X4 vertexNegative1Positive1Tranlation;
				DirectX::XMFLOAT4X4 vertexPositive1Negative1Tranlation;
			};

		public:

			VideoRenderer();
			VideoRenderer(const VideoRenderer &that);
			VideoRenderer(VideoRenderer &&that);
			virtual ~VideoRenderer();

			VideoRenderer &operator =(const VideoRenderer &that);
			VideoRenderer &operator =(VideoRenderer &&that);

		public:

			virtual void Initialize(Actor* _owner, ...) override;

			virtual void Update(const AngelSubSystem::TimeManager &timer) override;

			virtual void Render() override;

			virtual void OnMediaEngineEvent(DWORD meEvent) override;

		public:
			bool IsPlaying() const { return m_play; }
			
			void Load(const wchar_t * name);
			void Load(std::string &name);

			void SetRectangle(D2D1_RECT_F rect);
			void SetRectangle(float &l,float &t,float &r,float &b);
			void Set2DRendering(bool b) { this->m_2dRendering = b; }
			void Set2DRendering(bool &b) { if (b == true) m_2dRendering = true; else m_2dRendering = false; }
			void Set3DRendering(bool b) { this->m_3dRendering = b; }
			void Set3DRendering(bool &b) { if (b == true) m_3dRendering = true; else m_3dRendering = false; }
			void SetBillboard(bool b) { if (m_sprite) { if (b == true) m_sprite->SetBillboard(true); else  m_sprite->SetBillboard(false); } }
			void SetBillboard(bool &b) { if (m_sprite) { if(b==true) m_sprite->SetBillboard(true); else  m_sprite->SetBillboard(false);} }

		private:
			Microsoft::WRL::ComPtr<IMFMediaEngine>              m_MediaEngine;
			Microsoft::WRL::ComPtr<IMFMediaEngineEx>            m_EngineEx;
			bool												m_play;
			std::wstring										m_path;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>								m_textureDst;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>					m_srv;
			UINT									m_indicesSizeImage;
			UINT									m_strideImage;
			UINT									m_offsetImage;
			AngelRendering::Shader*					m_shaderImage;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_vertexBufferImage;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_indexBufferImage;
			bool									m_buffersInitialized;
			ID3D11Texture2D				*			m_renderTargetViewTexture;
			ID3D11RenderTargetView		*			m_renderTargetView;
			ID3D11ShaderResourceView	*			m_srvRtv;
			CBuffer<VertexTranslation>				m_cbVertexTranslation;
			AngelGUISystem::GUI3D::AGSprite *		m_sprite;
			bool									m_2dRendering;
			bool									m_3dRendering;
		};
	}
}