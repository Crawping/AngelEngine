#include"ParamLit.hlsli"
#include"MaterialTypes.hlsli"
#include"ParamFog.hlsli"


cbuffer cbMaterial : register(b1)
{
	float4  Ambient;
	float4  Diffuse;
	float4  Specular;
};

struct PixelOutputType
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 posW : SV_Target2;
};
Texture2D Texture[8] : register(t0);
SamplerState Sampler : register(s0);
float4 main(PS_INLIT IN) : SV_TARGET
{
	
	/*float4 finalColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 texColor = Texture[0].Sample(Sampler, IN.uv);

	if (texColor.x != 0.0f && texColor.y != 0.0f && texColor.z != 0.0f)
	{
		texColor = finalColor;
	}*/

	//if (texColor.w < 0.2f) discard;
	
	return float4(1.0f, 1.0f, 0.0f, 1.0f);

}