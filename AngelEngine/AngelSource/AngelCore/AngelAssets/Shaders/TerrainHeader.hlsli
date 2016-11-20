
cbuffer perFrame : register(c0)
{
	float4x4 WVP;
	float4x4 World;
	float4x4 lightViewMatrix;
	float4x4 lightProjectionMatrix;
}

SamplerState Sampler : register(s0);
Texture2D Texture : register(t0);
Texture2D shadowMap : register(t1);