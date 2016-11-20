
#include"ParamDiffuse.hlsli"


Texture2D Texture[8] : register(t0);
SamplerState Sampler : register(s0);

struct PixelOutputType
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 posW : SV_Target2;
};

PixelOutputType main(PS_IN input)
{
	PixelOutputType output;


// Sample the color from the texture and store it for output to the render target.
	output.color = Texture[0].Sample(Sampler, input.uv);

// Store the normal for output to the render target.
	output.normal = float4(input.normal, 1.0f);

	output.posW = input.posW;

	return output;
}


//#include"ParamFog.hlsli"
//#include"MaterialTypes.hlsli"
//#include"LightTypes.hlsli"
//
//static const float SMAP_SIZE = 2048.0f;
//static const float SMAP_DX = 1.0f / SMAP_SIZE;
//
//float CalcShadowFactor(SamplerComparisonState samShadow,
//	Texture2D shadowMap,
//	float depth,float2 texCoord)
//{
//	// Texel size.
//	const float dx = SMAP_DX;
//
//	float percentLit = 0.0f;
//	const float2 offsets[9] =
//	{
//		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
//		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
//		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
//	};
//	[unroll]
//	for (int i = 0; i < 9; ++i)
//	{
//		percentLit += shadowMap.SampleCmpLevelZero(samShadow,
//			texCoord.xy + offsets[i], depth).r;
//	}
//
//	return percentLit /= 9.0f;
//}
//
//
//cbuffer cbMaterial : register(b3)
//{
//	MeshMaterial Mat;
//};
//
//cbuffer Lights : register(
//{
//	Light light[MAX_MULTILIGHT];
//};
//
//
//cbuffer cbPerFrame : register(b1)
//{
//	float3 eyePos;
//	float pad;
//};
//
//Texture2D Texture : register(t0);
//Texture2D depthMap : register(t3);
//
//SamplerState SamplerWrap : register(s0);
//SamplerComparisonState SamplerCalmp : register(s1);
//SamplerState SamplerDepth : register(s2);
//float4 main(PS_IN IN) : SV_TARGET
//{
//
//	/////INIT
//	float3 toEye = eyePos - IN.posW;
//	float distToEye = length(toEye);
//	float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
//	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	float4 TotalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	float4 TotalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	float2 projectTexCoord;
//	float lightDepthValue = IN.shadowPosH.z / IN.shadowPosH.w;
//	float lighting = 1.0f;
//	/////////////*****
//
//
//	texColor = Texture.Sample(SamplerWrap, IN.uv);
//	
//	if (texColor.w < 0.2f) discard;
//	IN.normal = normalize(IN.normal);
//	projectTexCoord.x = IN.shadowPosH.x / IN.shadowPosH.w / 2.0f + 0.5f;
//	projectTexCoord.y = -IN.shadowPosH.y / IN.shadowPosH.w / 2.0f + 0.5f;
//	
//	[unroll]
//		for (int i = 0; i < MAX_MULTILIGHT; i++)
//		{
//			float4 TempAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
//			float4 TempDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
//			float3 toLight = float3(0.0f, 0.0f, 0.0f);
//			float distToLight = 0.0f;
//			float diffuseAmount = 0.0f;
//			float distToLightNorm = 0.0f;
//			float atten = 0.0f;
//			if (light[i].enable)
//			{
//				switch (light[i].lightType)
//				{
//				case DIRECTIONAL_LIGHT:
//					toLight = light[i].direction;
//					TempAmbient = Mat.ambient * light[i].color;
//					toLight = normalize(toLight);
//					diffuseAmount = saturate(dot(toLight, IN.normal));
//					if (light[i].castShadow)
//					{
//						lighting = CalcShadowFactor(SamplerCalmp, depthMap, lightDepthValue, projectTexCoord);
//					}
//					TempDiffuse = diffuseAmount*Mat.diffuse*light[i].color * lighting;
//					break;
//				case POINT_LIGHT:
//					toLight = light[i].position - IN.posW;
//					distToLight = length(toLight);
//
//					toLight /= distToLight;
//					diffuseAmount = saturate(dot(toLight, IN.normal));
//					TempDiffuse = diffuseAmount* light[i].color;
//					if (distToLight < light[i].range)
//						distToLightNorm = 1.0 - saturate((1 / light[i].range)*distToLight);
//					atten = distToLightNorm;
//					if (light[i].castShadow)
//					{
//						lighting = CalcShadowFactor(SamplerCalmp, depthMap, lightDepthValue - 0.001f, projectTexCoord);
//						
//					}
//					
//					TempDiffuse *= atten;
//					TempDiffuse *= Mat.diffuse*light[i].intensity * lighting;;
//
//					break;
//				case SPOT_LIGHT:
//					toLight = light[i].position - IN.posW;
//					distToLight = length(toLight);
//					toLight /= distToLight;
//					diffuseAmount = saturate(dot(toLight, IN.normal));
//					TempDiffuse = diffuseAmount* light[i].color;
//					//TempAmbient = Mat.ambient * light[i].color;
//					float cosAng = dot(light[i].direction, toLight);
//					float conAtt = saturate((cosAng - 0.1) *
//					0.01);
//					if (distToLight < light[i].range)
//					{
//						distToLightNorm = 1.0 - saturate((1 / light[i].range)*distToLight);
//						
//					}
//					if (light[i].castShadow)
//					{
//						lighting = CalcShadowFactor(SamplerCalmp, depthMap, lightDepthValue - 0.001f, projectTexCoord);
//						
//						
//						
//					}
//					atten = distToLightNorm;
//					TempDiffuse *= atten;
//					TempDiffuse *= Mat.diffuse*light[i].intensity*conAtt *lighting;
//					//TempDiffuse = lerp(TempDiffuse, lighting, 0.9);
//					
//					break;
//
//				}
//			}
//			TotalDiffuse += TempDiffuse;
//			TotalAmbient += TempAmbient;
//		}
//	
//		finalColor = texColor*Mat.colorTint*saturate(TotalAmbient + TotalDiffuse);
//
//	if (fogEnable)
//	{
//		float s = saturate((distToEye - fogStart) / fogRange);
//		finalColor = lerp(finalColor, fogColor, s);
//	}
//
//	if (!Mat.defaultAlpha)
//	{
//		finalColor.w = Mat.diffuse.w;
//	}
//
//	return finalColor;
//}