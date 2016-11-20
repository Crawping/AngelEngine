// ***********************************************************************
// FileName         : SkyMap.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Loading and using skymap,also rendering a sphere  </summary>
// ***********************************************************************

#pragma once
#include"../../AngelRendering/Public/CBuffer.h"
#include"../../../AngelSubSystem/AngelWorldSystem/AngelComponent/Public/IComponent.h"
#include"Shader.h"
#include"../../../AngelTools/AngelGlobalTools.h"


struct WorldStruct
{
	DirectX::XMFLOAT4X4 WVP;
	DirectX::XMFLOAT4X4 World; 
};

struct WorldStructPlane
{
	DirectX::XMFLOAT4X4 WVP;
};

struct PlaneData
{
	float translation;
	float scale;
	float brightness;
	float padding;
};

struct cbPerFrameStruct
{
	float blendAmount;
	float alpha;
	int dynamicWeahter;
	int padd;
};

struct UVStruct
{
	DirectX::XMFLOAT3 uv;
	float padd;
};

struct SkyMapTexture
{
	const char * name;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};

struct GradiantBuffer
{
	DirectX::XMFLOAT4 apexColor;
	DirectX::XMFLOAT4 centerColor;
};




namespace AngelCore
{
	namespace AngelRendering
	{

		class Actor;

		class SkyMap 
		{
		public:
			enum SkyMapType
			{
				Static = 0,
				Dynamic
			};
			friend class Actor;
			friend class MeshRenderer;
		public:
			void Initialize();
			void Render() ;
			void Update();
			ID3D11ShaderResourceView* LoadSkyMapTexture(const char * name,int id);

		public:
			void SetRotationAxis(const DirectX::XMVECTOR axis)	{ this->m_rotationAxis = axis; }
			void SetRotationSpeed(float speed)					{ this->m_rotationSpeed = speed; }
			void SetAlpha(float alpha)							{ this->m_alpha = alpha; }
			void SetCurrentSkyMapTexture(ID3D11ShaderResourceView * srv) { this->m_shadeResourcecurrent = srv; }
			void SetNextTextureBlend(float blend, ID3D11ShaderResourceView * srv);
			void SetDynamicClouds(bool b) { this->m_dynamicCloud = b; }
			void ChangeCenterColor(DirectX::XMFLOAT4 newColor, float speed);
			void ChangeApexColor(DirectX::XMFLOAT4 newColor, float speed);

		private:
			struct ModelType
			{
				float x, y, z;
				float tu, tv;
				float nx, ny, nz;
			};

			struct SkyPlaneType
			{
				float x, y, z;
				float tu, tv;
			};

			struct VertexType
			{
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT2 texture;
			};


			bool InitializeSkyPlane(
				int skyPlaneResolution,
				float skyPlaneWidth, 
				float skyPlaneTop, 
				float skyPlaneBottom, 
				int textureRepeat);

			void RenderSkyPlane();



			Texture2D* LoadPlaneTexture(const wchar_t* textureName);
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer>				m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				m_indexBuffer;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shadeResourcecurrent;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shadeResourcenext;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		m_depthStencilState;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_texture2D;
			AngelCore::AngelRendering::Shader *					m_shader;
			AngelCore::AngelRendering::Shader *					m_Planeshader;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>		m_rasterizer;
			DirectX::XMFLOAT4X4									m_world;
			ModelType*											m_model;
			int													m_vertexCount;
			int													m_indexCount;
			float												m_alpha;
			DirectX::XMVECTOR									m_rotationAxis;
			float												m_rotationSpeed;
			float												m_curRotation;
			SkyMapType											m_type;
			bool												m_changeBlendAmount;
			float												m_blendAmountPerUpdate;
			float												m_curBlendAmount;
			SkyMapTexture										*m_skyMapTextures;
			CBuffer<WorldStruct>								m_worldBuffer;
			CBuffer<GradiantBuffer>								m_cbGradiant;
			CBuffer<cbPerFrameStruct>							m_cbPerFrame;
			CBuffer<UVStruct>									m_uvBuffer;
			DirectX::XMFLOAT4X4									wvp4x4;
			SkyPlaneType*										m_skyPlane;
			int													m_PlanevertexCount, m_PlaneindexCount;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				m_PlanevertexBuffer, m_PlaneindexBuffer;
			float												m_brightness;
			float												m_translationSpeed[4];
			float												m_textureTranslation[4];
			Texture2D *											m_cloudTexture1, *m_cloudTexture2;
			CBuffer<WorldStructPlane>							m_cbWorldPlane;
			CBuffer<PlaneData>									m_cbPlane;
			bool												m_dynamicCloud;
		};
	}
}