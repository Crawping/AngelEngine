#include"PostProcessShared.hlsli"
//Bloomed Image
Texture2D gInputBloomed : register(t0);
//Original Image
Texture2D gInputOriginal : register(t1);
SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput : register(u0);



#define N 256

[numthreads(N, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float2 uv = float2(DTid.xy) / float2(WindowSize.x, WindowSize.y);//Temp Texture Coordinates.
	
	float4 bloomed = gInputBloomed.SampleLevel(Sampler, uv, 0);
	float4 original = gInputOriginal.SampleLevel(Sampler, uv, 0);
	gOutput[DTid.xy] = original+ bloomed;;
}