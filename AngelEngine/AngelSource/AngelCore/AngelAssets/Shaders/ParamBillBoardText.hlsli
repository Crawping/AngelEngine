struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
};
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv   : TEXCOORD;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

cbuffer World : register(b0)
{
	matrix WVP;
	matrix View;
	matrix Projection;
	float3 eye;
	float padd;
}

struct GeoOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
	uint   PrimID  : SV_PrimitiveID;
};