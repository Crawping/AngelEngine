#include"ParamSkyMapPlane.hlsli"

Texture2D Cloud : register(t0);
Texture2D perturbTexture : register(t1);

struct PixelOutputType
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};

SamplerState Sampler;

cbuffer SkyBuffer: register(b1)
{
	float translation;
	float scale;
	float brightness;
	float padding;
};

float4 main(PS_IN In) : SV_TARGET
{
	PixelOutputType OUT;
	In.uv.x += translation;
	float4 perturbValue;
	perturbValue = perturbTexture.Sample(Sampler, In.uv);
	perturbValue *= scale;
	perturbValue.xy = perturbValue.xy + In.uv.xy + translation;

	float4 textureColor = Cloud.Sample(Sampler, perturbValue.xy);
	textureColor *= brightness;

	/*OUT.color = textureColor;
	OUT.color.a = 1.0f;
	OUT.normal = float4(1.0f, 1.0f, 1.0f, 1.0f);*/

	return textureColor;
}