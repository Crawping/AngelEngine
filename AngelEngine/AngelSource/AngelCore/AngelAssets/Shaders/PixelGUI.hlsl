#include"ParamGUI.hlsli"

Texture2D GUITexture : register(t0);
SamplerState Sampler : register(s0);


float4 main(PS_IN IN) : SV_TARGET
{
	float4 GUIColor = GUITexture.Sample(Sampler,IN.uv);
	if (GUIColor.a < 0.1f) discard;
	return GUIColor;
}