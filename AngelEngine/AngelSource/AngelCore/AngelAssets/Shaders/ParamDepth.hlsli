struct VS_IN
{
	float4 pos : POSITION;
};

struct PS_IN
{
	float4 oPositionLight : SV_POSITION;
	float3 lightVec       : TEXCOORD0;
};