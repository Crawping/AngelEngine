#include"PostProcessShared.hlsli"
Texture2D gInput : register(t0);
SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput: register(u0);

cbuffer BlurAmount
{
	static const int gBlurRadius =5;
};

#define N 256
#define groupCacheSize (N+2*gBlurRadius)

groupshared float4 gCache[groupCacheSize];

[numthreads(N, 1, 1)]
void main(int3 groupThreadID : SV_GroupThreadID, int3 dispatchThreadID : SV_DispatchThreadID)
{
	float gWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
	};
	if (groupThreadID.x < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int x = max(dispatchThreadID.x - gBlurRadius, 0);
		gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	if (groupThreadID.x >= N - gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int x = min(dispatchThreadID.x + gBlurRadius, gInput.Length.x - 1);
		gCache[groupThreadID.x + 2 * gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];
	}

	// Clamp out of bound samples that occur at image borders.
	gCache[groupThreadID.x + gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy - 1)];

	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();

	//
	// Now blur each pixel.
	//

	float4 blurColorVer = float4(0, 0, 0, 0);

		[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + gBlurRadius + i;

		blurColorVer += gWeights[i + gBlurRadius] * gCache[k];
	}

	gOutput[dispatchThreadID.xy] = blurColorVer;

	
}