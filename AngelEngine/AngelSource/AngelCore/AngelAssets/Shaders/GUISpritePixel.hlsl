
#include "GUISpriteHeader.hlsli"

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

float4 main(PS_IN IN) : SV_TARGET
{
	return Texture.SampleLevel(Sampler, IN.uv,0);
}