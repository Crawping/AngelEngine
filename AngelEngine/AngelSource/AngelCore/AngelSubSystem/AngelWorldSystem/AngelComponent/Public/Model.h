//--------------------------------------------------------------------------------------
// File: Model.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <memory>
#include <functional>
#include <set>
#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#include <intsafe.h>
#pragma warning(pop)

#include <wrl\client.h>

#include"../../../AngelRendering/Public/Shader.h"
#include"../../../AngelRendering/Public/DebugCollisionShape.h"


// VS 2010 doesn't support explicit calling convention for std::function
#ifndef DIRECTX_STD_CALLCONV
#if defined(_MSC_VER) && (_MSC_VER < 1700)
#define DIRECTX_STD_CALLCONV
#else
#define DIRECTX_STD_CALLCONV __cdecl
#endif
#endif

struct ShaderMaterial
{
	DirectX::XMFLOAT4   GlobalAmbient;
	DirectX::XMFLOAT4   AmbientColor;
	DirectX::XMFLOAT3   EmissiveColor;
	int					instancing;
	DirectX::XMFLOAT4   DiffuseColor;
	DirectX::XMFLOAT3   SpecularColor;
	int					Skinning;
	float               SpecularPower;
	float				SpecularScale;
	float				AlphaThreshold;
	int					MaterialType;
};

struct cbPerObjectBuffer
{
	DirectX::XMFLOAT4X4 WVP;
	DirectX::XMFLOAT4X4 WVPLast;
	DirectX::XMFLOAT4X4 WorldView;
	DirectX::XMFLOAT4X4 WorldInvTranspose;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	DirectX::XMFLOAT4X4 ViewProjection;
};


const uint32_t NUM_BONE_INFLUENCES = 4;

struct SkinningVertex
{
	UINT boneIndex[NUM_BONE_INFLUENCES];
	float boneWeight[NUM_BONE_INFLUENCES];
};

struct MeshExtents
{
	float CenterX, CenterY, CenterZ;
	float Radius;

	float MinX, MinY, MinZ;
	float MaxX, MaxY, MaxZ;
};

struct Bone
{
	INT ParentIndex;
	DirectX::XMFLOAT4X4 InvBindPos;
	DirectX::XMFLOAT4X4 BindPos;
	DirectX::XMFLOAT4X4 LocalTransform;
};

struct Clip
{
	float StartTime;
	float EndTime;
	UINT  keys;
};

struct Keyframe
{
	UINT BoneIndex;
	float Time;
	DirectX::XMFLOAT4X4 Transform;
};

struct AnimationClip
{
	std::wstring name;
	std::vector<Keyframe*> keyFrames;
	float start;
	float end;
	UINT Keys;
	float elapsedTime;
	//for using Our ending.
	bool UseCodeEnding;
	float CodeEndingTime;
	//faster or slower playing of some animation clips.
	float timeScale;
};

struct Armature
{
	DirectX::XMFLOAT4X4 Bones[1024];
};
namespace AngelCore
{
	namespace AngelWorldSystem
	{
		class Transform;
		class Actor;
#if (DIRECTX_MATH_VERSION < 305) && !defined(XM_CALLCONV)
#define XM_CALLCONV __fastcall
		typedef const XMVECTOR& HXMVECTOR;
		typedef const XMMATRIX& FXMMATRIX;
#endif
		class ModelMesh;

		//----------------------------------------------------------------------------------
		// Each mesh part is a submesh with a single effect
		class ModelMeshPart
		{
		public:
			ModelMeshPart();
			virtual ~ModelMeshPart();

			uint32_t                                                indexCount;
			uint32_t                                                startIndex;
			uint32_t                                                vertexOffset;
			uint32_t                                                vertexStride;
			D3D_PRIMITIVE_TOPOLOGY                                  primitiveType;
			DXGI_FORMAT                                             indexFormat;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>               inputLayout;
			Microsoft::WRL::ComPtr<ID3D11Buffer>                    indexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>                    vertexBuffer;
			std::shared_ptr<std::vector<D3D11_INPUT_ELEMENT_DESC>>  vbDecl;
			AngelCore::AngelRendering::Shader *						shader;
			CBuffer<cbPerObjectBuffer>								m_wvpBuffer;
			CBuffer<ShaderMaterial>									m_material;
			ID3D11ShaderResourceView *								textureView[8];
			bool                                                    isAlpha;
			DirectX::BoundingOrientedBox                 boundingBox;
			AngelCore::AngelRendering::DebugCollisionShape *		collisionShape;
			bool													enableSkinning;
			bool													enableInstancing;

			typedef std::vector<std::unique_ptr<ModelMeshPart>> Collection;

			// Draw mesh part with custom effect
			void __cdecl Draw(_In_ ID3D11DeviceContext* deviceContext, _In_ ID3D11InputLayout* iinputLayout,
				AngelCore::AngelRendering::Shader * shader, bool depth
				, DirectX::CXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection);

			void __cdecl DrawInstanced
				(_In_ ID3D11DeviceContext* deviceContext,
					Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer, _In_ ID3D11InputLayout* iinputLayout,
				AngelCore::AngelRendering::Shader * shader, bool depth
				,DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, UINT instanceCount);

			// Create input layout for drawing with a custom effect.
		private:
			DirectX::XMFLOAT4X4 lWVP;

		};


		//----------------------------------------------------------------------------------
		// A mesh consists of one or more model mesh parts
		class ModelMesh
		{
		public:
			ModelMesh();
			virtual ~ModelMesh();

			DirectX::BoundingSphere              boundingSphere;
			DirectX::BoundingOrientedBox         boundingBox;
			ModelMeshPart::Collection   meshParts;
			std::wstring                name;
			bool                        ccw;
			bool                        pmalpha;
			std::vector<Bone*>			bones;
			std::vector<std::wstring>	boneNames;
			bool						enableSkinning;
			CBuffer<Armature>			armatureBuffer;
			std::vector<AnimationClip*>	animationClips;
			DirectX::XMFLOAT3*			points;
			const USHORT*				indices;
			std::vector<int>			indciesInt;
			UINT						pointsSize;
			UINT						indicesSize;
			bool						enableInstancing;
			int index;

			typedef std::vector<std::shared_ptr<ModelMesh>> Collection;

			// Draw the mesh
			void XM_CALLCONV Draw(_In_ ID3D11DeviceContext* deviceContext,
				DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection,
				AngelCore::AngelRendering::Shader * shader, std::wstring name
				, bool depth, AngelCore::AngelWorldSystem::Actor * owner);

			void XM_CALLCONV DrawInstanced(_In_ ID3D11DeviceContext* deviceContext, 
				Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer,
				DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection,
				AngelCore::AngelRendering::Shader * shader, bool updateAABB, std::wstring name
				, bool depth, UINT instanceCount);

			Transform * GetTransform() const		{ return m_transform; }

		public:
			void SetUseParentTransform(bool b) { this->m_useParentTransform = b; }

		private:
			bool		m_useParentTransform;
			Transform * m_transform;
			DirectX::XMFLOAT4X4 lWorld;
		};


		//----------------------------------------------------------------------------------
		// A model consists of one or more meshes
		class Model
		{
		public:
			Model() {}
			virtual ~Model();

			ModelMesh::Collection   meshes;
			std::wstring            name;

			// Draw all the meshes in the model
			void XM_CALLCONV Draw(_In_ ID3D11DeviceContext* deviceContext,
				DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection
				, AngelCore::AngelRendering::Shader * shader, bool depth, 
				AngelCore::AngelWorldSystem::Actor * owner);

			void XM_CALLCONV DrawInstanced(_In_ ID3D11DeviceContext* deviceContext,
				DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection
				, AngelCore::AngelRendering::Shader * shader, bool depth,UINT instanceCount);

			Microsoft::WRL::ComPtr<ID3D11Buffer> GetInstanceBuffer() { return m_instanceBuffer; }
			void SetInstanceBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> b) { this->m_instanceBuffer = b; }

		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;
		};
	}
}