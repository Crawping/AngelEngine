#ifndef MAX_LIGHT
#pragma message( "NUM_LIGHTS undefined. Default to 8.")
#define MAX_LIGHT 8 // should be defined by the application.
#endif

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
//--------------------- Vertex-Pixel Shader Inputs.
struct VS_IN_MESH
{
	float4 PosL : POSITION;
	float3 Normal   : NORMAL;
	float4 Tangent : TANGENT;
	float2 UV :		TEXCOORD0;
	uint4  Indices  : BLENDINDICES0;
	float4 Weights  : BLENDWEIGHT0;
	row_major float4x4 mTransform : mTransform;
};

struct PS_IN_MESH
{
	float3 positionVS   : TEXCOORD0;    // View space position.
	float2 texCoord     : TEXCOORD1;    // Texture coordinate
	float3 tangentVS    : TANGENT;      // View space tangent.
	float3 binormalVS   : BINORMAL;     // View space binormal.
	float3 normalVS     : NORMAL;       // View space normal.
	float4 position     : SV_POSITION;  // Clip space position.
	float4 shadowPosH	: TEXCOORD2;
	float2 velocity		: TEXCOORD3;
};

struct VS_IN_TERRAIN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 color : COLOR;
	float4 layers : LAYERS;
	int tileNumber : TILENUMBER;
};

struct PS_IN_TERRAIN
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 posW : POSITION;
	float4 shadowPosH	: TEXCOORD2;
	float3 color : COLOR;
	float4 layers : LAYERS;
	int tileNumber : TILENUMBER;

};


//------------------------------------------------------------

//-----------------------------------------------------Structs
struct Material
{
	float4  GlobalAmbient;
	//-------------------------- ( 16 bytes )
	float4  AmbientColor;
	//-------------------------- ( 16 bytes )
	float3  EmissiveColor;
	int		Instancing;
	//-------------------------- ( 16 bytes )
	float4  DiffuseColor;
	//-------------------------- ( 16 bytes )
	float3  SpecularColor;
	int		Skinning;
	//-------------------------- ( 16 bytes )
	float   SpecularPower;
	float   SpecularScale;
	float   AlphaThreshold;
	int		MaterialType;
	//-------------------------- ( 16 bytes )
};  //--------------------------- ( 16 * 6 = 96 bytes )

struct Light
{
	/**
	* Position for point and spot lights (World space).
	*/
	float3   PositionWS;
	int castShadow;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Direction for spot and directional lights (World space).
	*/
	float4   DirectionWS;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Color of the light. Diffuse and specular colors are not seperated.
	*/
	float4   Color;
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
	bool    Enabled;
	//--------------------------------------------------------------( 16 bytes )

	/**
	* The type of the light.
	*/
	uint    Type;
	float3  EyePosition;
	//--------------------------------------------------------------( 16 bytes )
	//--------------------------------------------------------------( 16 * 7 = 112 bytes )
};

struct PixelShaderOutput
{
	float4 color : SV_TARGET0;
	float4 velocityUV : SV_TARGET1;
	float4 ZBuffer : SV_TARGET2;
};
//----------------------------------------------------------------

//----------------------------------------Constant Buffers

cbuffer Material : register(b1)
{
	Material Mat;
};
cbuffer cbLight : register(b2)
{
	Light light[MAX_LIGHT];
};
cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
	float4x4 WVPLast;
	float4x4 World;
	float4x4 WorldInvTranspose;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;
	float4x4 ViewProjection;
};

cbuffer PerArmature : register(b3)
{
	float4x4 Bones[1024];
};

cbuffer TextureData : register(b4)
{
	float width;
	float height;
	int   numTileWidth;
	int   numTileHeight;
};

//-----------------------------------------------------

//-------------------------------------- Textures
Texture2D	MeshTexture[8]		:		register(t0);
TextureCube cubeShadowMap		:		register(t8);
Texture2D	shadowMap			:		register(t9);
Texture2D   terrainTexture[4]	:		register(t10);
Texture2D   fullTexture			:		register(t15);
//---------------------------------------

//---------------------------------------Samplers
SamplerState LinearRepeatSampler     :	register(s0);
SamplerState LinearClampSampler      :	register(s1);
SamplerState shadowMapSampler :			register(s2);
SamplerComparisonState SamplerClamp :	register(s3);
//--------------------------------------

//----------------------------------Lighting
float3 ExpandNormal(float3 normal)
{
	return normal*2.0f - 1.0f;
}

float4 DoNormalMapping(float3x3 TBN, Texture2D tex, sampler s, float2 uv)
{
	float3 normal = tex.Sample(s, uv).xyz;
	normal = ExpandNormal(normal);

	// Transform normal from tangent space to view space.
	normal = mul(normal, TBN);
	return normalize(float4(normal, 0));
}

struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};
float4 DoDiffuse(Light light, float4 L, float4 N)
{
	float NdotL = max(dot(N, L), 0);
	return light.Color * NdotL;
}

float4 DoSpecular(Light light, Material material, float4 V, float4 L, float4 N)
{
	float4 R = normalize(reflect(-L, N));
	float RdotV = max(dot(R, V), 0);

	return light.Color * pow(RdotV, material.SpecularPower);
}

float DoAttenuation(Light light, float d)
{
	return 1.0f - smoothstep(light.Range * 0.75f, light.Range, d);
}

float DoSpotCone(Light light, float4 L)
{
	// If the cosine angle of the light's direction 
	// vector and the vector from the light source to the point being 
	// shaded is less than minCos, then the spotlight contribution will be 0.
	float minCos = cos(radians(light.SpotlightAngle));
	// If the cosine angle of the light's direction vector
	// and the vector from the light source to the point being shaded
	// is greater than maxCos, then the spotlight contribution will be 1.
	float maxCos = lerp(minCos, 1, 0.5f);
	float cosAngle = dot(light.DirectionWS, -L);
	// Blend between the maxixmum and minimum cosine angles.
	return smoothstep(minCos, maxCos, cosAngle);
}
LightingResult DoPointLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = float4(light.PositionWS,1.0f) - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);

	result.Diffuse = DoDiffuse(light, L, N) * attenuation * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * attenuation * light.Intensity;

	return result;
}

LightingResult DoDirectionalLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = normalize(light.DirectionWS);

	result.Diffuse = DoDiffuse(light, L, N) * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * light.Intensity;

	return result;
}

LightingResult DoSpotLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = float4(light.PositionWS,1.0) - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);
	float spotIntensity = DoSpotCone(light, L);

	result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * attenuation * spotIntensity * light.Intensity;

	return result;
}
LightingResult DoLighting(Material mat,  float4 P, float4 N)
{
	

	LightingResult totalResult = (LightingResult)0;

	for (int i = 0; i < MAX_LIGHT; ++i)
	{
		float4 V = normalize(float4(light[i].EyePosition,1.0f) - P);
		LightingResult result = (LightingResult)0;

		// Skip lights that are not enabled.
		if (!light[i].Enabled) continue;
		// Skip point and spot lights that are out of range of the point being shaded.
		if (light[i].Type != DIRECTIONAL_LIGHT && length(light[i].PositionWS.xyz - P.xyz) > light[i].Range) continue;

		switch (light[i].Type)
		{
		case DIRECTIONAL_LIGHT:
		{
			result = DoDirectionalLight(light[i], mat, V, P, N);
		}
		break;
		case POINT_LIGHT:
		{
			result = DoPointLight(light[i], mat, V, P, N);
		}
		break;
		case SPOT_LIGHT:
		{
			result = DoSpotLight(light[i], mat, V, P, N);
		}
		break;
		}
		totalResult.Diffuse += result.Diffuse;
		totalResult.Specular += result.Specular;
	}

	return totalResult;
}
//----------------------------------------------

//-------------------------------------Shadow Mapping
static const float SMAP_SIZE = 20192.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;
float CalcShadowFactor(SamplerComparisonState samShadow,
	Texture2D shadowMap,
	float depth, float2 texCoord)
{
	// Texel size.
	const float dx = SMAP_DX;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
	};
	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMap.SampleCmpLevelZero(samShadow,
			texCoord.xy + offsets[i], depth).r;
	}

	return percentLit /= 10.0f;
}
//---------------------------------------------------------