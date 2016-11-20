#include"ParamDiffuse.hlsli"
#include"LightingCalculation.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
	float4x4 World;
	float4x4 WorldInvTranspose;
};

cbuffer cbUV : register(b2)
{
	float4 uv = float4(0.0f, 0.0f, 1.0f, 1.0f);
};


PS_IN main( VS_IN IN )
{
	PS_IN OUT;
	//transform homogeneus clip space
	OUT.posH = mul(IN.pos, WVP);
	OUT.uv = IN.uv;
	//transform normal vectors
	OUT.normal = mul(IN.normal, (float3x3)WorldInvTranspose);

	OUT.posW = mul(IN.pos, World);
	return OUT;
}