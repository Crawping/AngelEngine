
#include "ParamPostProcess.hlsli"

Texture2D TextureScene : register(t0);
Texture2D TextureGUI : register(t1);
SamplerState Sampler : register(s0);

float4 main(PS_IN IN) : SV_TARGET
{
	float4 textureSceneColor = TextureScene.Sample(Sampler, IN.uv);
	float4 TextureGUIColor = TextureGUI.Sample(Sampler, IN.uv);
	if (any(TextureGUIColor.xyz))
	{
		return TextureGUIColor;
	}

	return textureSceneColor;
}

//#include"ParamPostProccess.hlsli"
//
//cbuffer Type : register(b0)
//{
//	int postProccessType = 1;
//	float3 padd;
//};
//const float GlowInten = 2.5f;
//
//Texture2D orginalTexture : register(t0);
//Texture2D BluredTexture : register(t1);
//
//SamplerState Sampler;
//float4 main(PS_IN IN) : SV_TARGET
//{
//	float4 SceneColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	float3 glowColor = float3(0.0f, 0.0f, 0.0f);
//	SceneColor = orginalTexture.Sample(Sampler, IN.uv);
//	glowColor = BluredTexture.Sample(Sampler, IN.uv);
//	if (postProccessType == 1)
//	{
//		SceneColor.rgb = glowColor;
//	}
//	if (postProccessType == 2)
//	{
//		SceneColor.rgb += glowColor ;
//	}
//	SceneColor.w = 1.0f;
//	return SceneColor;
//}