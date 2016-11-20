#pragma once
#include<DirectXMath.h>
#include<vector>
#include"CBuffer.h"
#include"../../AngelWorldSystem/AngelComponent/Public/MeshRenderer.h"
#include"../../AngelWorldSystem/AngelComponent/Public/LightRenderer.h"
#define MAX_LIGHT 8
namespace AngelCore
{
	namespace AngelRendering
	{
		class Shader;

		class LightManager
		{
		public:
			enum LIGHT_TYPE
			{
				POINT_LIGHT  = 0,
				SPOT_LIGHT, 
				DIRECTIONAL_LIGHT 
			};
			
		public:

			struct Light
			{
				/**
				* Position for point and spot lights (World space).
				*/
				DirectX::XMFLOAT3   PositionWS;
				int					castShadow;
				//--------------------------------------------------------------( 16 bytes )
				/**
				* Direction for spot and directional lights (World space).
				*/
				DirectX::XMFLOAT4   DirectionWS;
				//--------------------------------------------------------------( 16 bytes )
				/**
				* Color of the light. Diffuse and specular colors are not seperated.
				*/
				DirectX::XMFLOAT4   Color;
				//--------------------------------------------------------------( 16 bytes )
				/**
				* The half angle of the spotlight cone.
				*/
				float    SpotlightAngle;
				/**
				* The range of the light.
				*/
				float    Range;

				/**
				* The intensity of the light.
				*/
				float    Intensity;

				/**
				* Disable or enable the light.
				*/
				bool			Enabled;
				//--------------------------------------------------------------( 16 bytes )

				/**
				* The type of the light.
				*/
				unsigned int		Type;
				DirectX::XMFLOAT3  CameraPosition;
				//--------------------------------------------------------------( 16 bytes )
				//--------------------------------------------------------------( 16 * 5 = 90 bytes )

				Light(AngelCore::AngelWorldSystem::LightRenderer * l)
				{
					PositionWS = l->GetPosition();
					castShadow = l->GetCastShadow();
					Type = l->GetType();
					DirectionWS = l->GetDirection();
					Color = l->GetColor();
					SpotlightAngle = l->GetSpotLightAngle();
					Range = l->GetRange();
					Intensity = l->GetIntensity();
					Enabled = l->GetEnable();
				}

				Light() 
				{
					PositionWS = DirectX::XMFLOAT3(0, 0, 0);
					castShadow = false;
					DirectionWS = DirectX::XMFLOAT4(0, 0, 0, 0);
					Color = DirectX::XMFLOAT4(0, 0, 0, 0);
					SpotlightAngle = 0;
					Range = 50;
					Intensity = 1;
					Enabled = false;
					Type = 0;
					PositionWS = DirectX::XMFLOAT3(0, 0, 0);
				}
			};


		private:

			struct cbLight
			{
				Light l[MAX_LIGHT];
			};
		
		public:
			void AddLight(AngelCore::AngelWorldSystem::LightRenderer *l);
			void DeleteLight(Light *l);
			void UpdateLight(int index
				, AngelCore::AngelWorldSystem::LightRenderer *l);

			//Shadow Mapping
			void Initialize();
			void RenderLightsShadowMap();
			
			Light * GetLight(int i);
			void Update();
			static LightManager * GetInstance();

			CBuffer<cbLight> GetBuffer() { return m_cbLight; }

			ID3D11ShaderResourceView * GetSRV() { return m_SrvDsv.Get(); }
			
			ID3D11ShaderResourceView * GetSRVDirectionalLight() { return m_SrvDirectional.Get(); }

			DirectX::XMFLOAT4X4 * GetViews()			{ return m_views; }
			DirectX::XMFLOAT4X4 * GetProjections()		{ return m_projections; }

			UINT GetIndex() const						{ return m_lights.size(); }

			void ClearLights();

		private:
			LightManager() { m_cbLight.Load(); m_lightCount = 0; };
			LightManager(const LightManager & l) { };
			void BuildCubicShadowMap(float x, float y, float z);
			void BuildOrthographicShadowMap(int i);
			void BuildPerspectiveShadowMap(int i);

		private:
			static LightManager *										m_instance;
			std::vector<AngelCore::AngelWorldSystem::MeshRenderer *>	m_meshes;
			CBuffer<cbLight>											m_cbLight;
			std::vector<Light*>											m_lights;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>			m_SrvDsv;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>			m_SrvRtv;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>			m_SrvDirectional;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>			m_SrvSpot;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>				m_Dsv[6];
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>				m_DsvDirectional;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>				m_DsvSpot;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>				m_Rtv[6];
			DirectX::XMFLOAT4X4											m_views[6];
			DirectX::XMFLOAT4X4											m_projections[6];
			D3D11_VIEWPORT												m_viewPort;
			UINT m_lightCount;
		};
	}
}