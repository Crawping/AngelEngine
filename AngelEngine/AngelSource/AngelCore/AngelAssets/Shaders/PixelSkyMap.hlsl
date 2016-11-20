#include"ParamSkyMap.hlsli"
#include"ParamFog.hlsli"

TextureCube skyMapCur : register(t0);
TextureCube skyMapNext : register(t1);
SamplerState Sampler : register(s0);

struct PixelOutputType
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};

cbuffer cbPerFrame : register(b1)
{
	float blendAmount;
	float alpha;
	int dynamicWeather;
	int padd;
};

cbuffer GradiantBuffer : register(b2)
{
	float4 apexColor;
	float4 centerColor;
}


float4 main(PS_IN IN) : SV_TARGET
{
	PixelOutputType OUT;
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 skyMapCurColor = skyMapCur.Sample(Sampler, IN.uv);
	float4 skyMapNextColor = skyMapNext.Sample(Sampler, IN.uv);
	float height = IN.uv.y;
	if (height < -20.0f)
	{
		height = 0.0f;
	}
	float4 skyColor = lerp(centerColor, apexColor, height);
	if (any(skyMapNextColor))
		finalColor = lerp(skyMapCurColor, skyMapNextColor, blendAmount);
	else
		finalColor = skyMapCurColor;

	if(dynamicWeather ==1)
		finalColor = lerp(finalColor, skyColor, 0.5f);

	//finalColor = day;
	finalColor.a = 1.0f;
	return finalColor;
}