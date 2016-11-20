#include"PostProcessShared.hlsli"
Texture2D gInput : register(t0);

SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput : register(u0);



#define N 256

[numthreads(N, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float2 uv = float2(DTid.xy) / float2(WindowSize.x, WindowSize.y);//Temp Texture Coordinates.
	float3 ColorOut = gInput.SampleLevel(Sampler,uv,0).rgb;
	float Luminance = 0.08f;
	static const float fMiddleGray = 0.18f;
	static const float fWhiteCutoff = 0.8f;
	ColorOut *= fMiddleGray / (Luminance + 0.001f);
	ColorOut *= (1.0f + (ColorOut / (fWhiteCutoff * fWhiteCutoff)));
	ColorOut -= 5.0f;

	ColorOut = max(ColorOut, 0.0f);

	ColorOut /= (10.0f + ColorOut);

	//return float4(ColorOut, 1.0f);
	//float4 colorAvg = (gInput[DTid.xy].x + gInput[DTid.xy].y + gInput[DTid.xy].z) / 3.0f;
	gOutput[DTid.xy] = float4(ColorOut,1.0f);
}