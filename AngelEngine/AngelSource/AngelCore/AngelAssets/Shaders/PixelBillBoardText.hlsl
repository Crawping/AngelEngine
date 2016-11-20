#include"ParamBillBoardText.hlsli"

Texture2D Texture : register(t0);
Texture2D Texture1 : register(t1);
SamplerState Sampler : register(s0);

cbuffer ColorBuffer : register(c1)
{
	float4 Color;
}

float4 main(PS_IN IN) : SV_TARGET
{
	float4 textTexture = Texture.Sample(Sampler, IN.uv);
	float4 finalColor = 0;
	if (any(Color))
		finalColor = textTexture*Color;
	else
		finalColor = textTexture;
	if (finalColor.a < 0.1f) discard;
	return finalColor;
}