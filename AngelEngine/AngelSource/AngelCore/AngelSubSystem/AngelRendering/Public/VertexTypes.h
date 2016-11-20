// ***********************************************************************
// FileName         : VertexTypes.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> All vertex types for creating vertex buffers of models  </summary>
// ***********************************************************************
#pragma once


#define __VERTEX_TYPES
#include<DirectXMath.h>
#include<DirectXPackedVector.h>

enum VertexTypes : byte
{
	NOP,
	Position,
	PositionColor,
	PositionTexCoord,
	PositionTexCoordColor,
	PositionNormalTexture,
	PositionNormalTextureColor,
	PositionNormalTextureTangent,
	PositionNormalTextureTangentBinormal,
	PositionNormalTangentColorTexture,
	PositionNormalTangentTextureSkinned,
	Particle,
	SkyVertex,
	PlaneVertex,
	Terrain
};

namespace VertexTypesStruct
{
	using namespace DirectX;
	struct Position
	{
		Position(float _x, float _y, float _z) : pos(_x, _y, _z) {}
		XMFLOAT3 pos;
	};
	struct  PositionColor
	{
		XMFLOAT3 pos;
		XMFLOAT4 color;
	};
	struct PositionTexCoord
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};
	struct PositionTexCoordColor
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
		XMFLOAT4 color;
	};
	struct PositionNormalTexture
	{
		PositionNormalTexture(XMFLOAT3 _p, XMFLOAT3 _n, XMFLOAT2 _t)
		{
			Position = _p;
			Normal = _n;
			TexCoord = _t;
		}
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
	};
	struct PositionNormalTextureColor
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
		XMFLOAT3 Color;
	};
	struct Terrain
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
		XMFLOAT3 Color;
		FLOAT Layers[4];
		int TileNumber;
	};

	struct Instancing
	{
		XMFLOAT4X4 world;
	};

	struct VertexPositionNormalTextureTangent
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 Texcoord;
		XMFLOAT3 Tangent;
	};

	struct VertexPositionNormalTextureTangentBinormal
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 Texcoord;
		XMFLOAT3 Tangent;
		XMFLOAT3 Binormal;
	};


	struct VertexPositionNormalTangentColorTexture
	{
		XMFLOAT3	 Position;
		XMFLOAT3	 Normal;
		XMFLOAT4	 Tangent;
		UINT     Color;
		XMFLOAT2	 TexCoord;
	};

	struct VertexPositionNormalTangentTextureSkinned
	{
		XMFLOAT3  Position;
		XMFLOAT3 Normal;
		XMFLOAT4 Tangent;
		XMFLOAT2 TexCoord;
		UINT boneIndex[4];
		float boneWeight[4];
	};

	struct VertexParticel
	{
		XMFLOAT3 Position;
		XMFLOAT3 Velocity;
		float	 Life ;
		float    Size ;
	};

	struct SkyVertex
	{
		XMFLOAT3 pos;
	};

	struct PlaneVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};
}