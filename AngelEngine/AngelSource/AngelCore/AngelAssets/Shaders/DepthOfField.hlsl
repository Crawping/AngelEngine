#include"PostProcessShared.hlsli"
Texture2D gInputColor : register(t0);
Texture2D gInputColorBlured : register(t1);
Texture2D gInputDepth : register(t2);
Texture2D gInputPos : register(t3);
SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput : register(u0);

#define N 256

[numthreads(N, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float2 uv = float2(DTid.xy) / float2(WindowSize.x, WindowSize.y);//Temp Texture Coordinates.

	float depthValue = gInputPos.SampleLevel(Sampler, uv, 0).x;
	float4 originalColor = float4(gInputColor[DTid.xy].x , gInputColor[DTid.xy].y , gInputColor[DTid.xy].z,1.0f);
	float4 bluredColor = float4(gInputColorBlured[DTid.xy].x, gInputColorBlured[DTid.xy].y, gInputColorBlured[DTid.xy].z, 1.0f);
	float4 finalColor = lerp(originalColor, bluredColor, gInputDepth[DTid.xy].x);
	
	gOutput[DTid.xy] = lerp(bluredColor, originalColor,depthValue);;
}