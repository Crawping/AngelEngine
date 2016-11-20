#include"PostProcessShared.hlsli"
Texture2D gInput : register(t0);

SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput : register(u0);

cbuffer TKernel : register(c0)
{
	float4 TexelKernel[13];
}

#define N 256

[numthreads(N, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	static const int g_cKernelSize = 13;
	static const float BlurWeights[g_cKernelSize] =
	{
		0.002216,
		0.008764,
		0.026995,
		0.064759,
		0.120985,
		0.176033,
		0.199471,
		0.176033,
		0.120985,
		0.064759,
		0.026995,
		0.008764,
		0.002216,
	};

	float BloomScale = 1.5f;

	float2 uv = float2(DTid.xy) / float2(WindowSize.x, WindowSize.y);//Temp Texture Coordinates.
	float4 Color = 0;

	for (int i = 0; i < g_cKernelSize; i++)
	{
		Color += gInput.SampleLevel(Sampler, uv + TexelKernel[i].xy,0) * BlurWeights[i];
	}
	gOutput[DTid.xy] = Color * BloomScale;
}