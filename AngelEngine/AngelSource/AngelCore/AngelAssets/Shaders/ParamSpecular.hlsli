
struct VS_IN
{
	float4 pos : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};
struct PS_IN
{
	float4 posH : SV_POSITION0;
	float4 posW : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
	float4 shadowPosH : TEXCOORD1;
};
