
struct VS_IN
{
	float4 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
struct PS_IN
{
	float4 posH : SV_POSITION0;
	float2 uv : TEXCOORD0;
};
