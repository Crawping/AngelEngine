#include"PostProcessShared.hlsli"
Texture2D gInput : register(t0);
SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput: register(u0);


cbuffer BlurAmount
{
	static const int gBlurRadius = 5;
};

#define N 256
#define groupCacheSize (N+2*gBlurRadius)

groupshared float4 gCache[groupCacheSize];


[numthreads(1, N, 1)]
void main(int3 groupThreadID : SV_GroupThreadID, int3 dispatchThreadID : SV_DispatchThreadID)
{

	float gWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
	};
	

	if (groupThreadID.y < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = max(dispatchThreadID.y - gBlurRadius, 0);
		
		gCache[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}
	if (groupThreadID.y >= N - gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = min(dispatchThreadID.y + gBlurRadius, gInput.Length.y - 1);
		gCache[groupThreadID.y + 2 * gBlurRadius] = gInput[int2(dispatchThreadID.x, y)];
	}

	// Clamp out of bound samples that occur at image borders.

	gCache[groupThreadID.y + gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy - 1)];


	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();

	//
	// Now blur each pixel.
	//

	float4 blurColorHor = float4(0, 0, 0, 0);

		[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;

		blurColorHor += gWeights[i + gBlurRadius] * gCache[k];
	}

	gOutput[dispatchThreadID.xy] = blurColorHor;
}