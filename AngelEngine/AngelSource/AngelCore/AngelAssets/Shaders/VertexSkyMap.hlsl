#include"ParamSkyMap.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
	float4x4 World;
};

cbuffer cbUV : register(c2)
{
	float3 uv;
	float padd;
}

PS_IN main( VS_IN  IN)
{
	PS_IN Out;
	Out.pos = mul(float4(IN.pos, 1.0f), WVP).xyww;
	Out.posW = mul(float4(IN.pos, 1.0f), World);
	Out.uv = IN.pos;;
	
	return Out;
}