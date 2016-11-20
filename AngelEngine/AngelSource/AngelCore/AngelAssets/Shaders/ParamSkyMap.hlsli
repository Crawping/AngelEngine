struct VS_IN
{
	float3 pos : POSITION;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 posW : POSITION;
	float3 uv : TEXCOORD;
};