#include"PostProcessShared.hlsli"
Texture2D gInput : register(t0);
Texture2D gInputCurVelocity : register(t1);
Texture2D gInputLastVelocity : register(t2);
SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput : register(u0);

static const float NumberOfPostProcessSamples = 12.0f;
#define N 256

[numthreads(N, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float2 pixelVelocity;
	float2 uv = float2(DTid.xy) / float2(WindowSize.x, WindowSize.y);
	float4 curVelocity = gInputCurVelocity.SampleLevel(Sampler, uv, 0);
	float4 lastVelocity = gInputLastVelocity.SampleLevel(Sampler, uv, 0);
	float curVelocitySqMag = curVelocity.r * curVelocity.r +
		curVelocity.g * curVelocity.g;
	float lastVelocitySqMag = lastVelocity.r * lastVelocity.r +
		lastVelocity.g * lastVelocity.g;
	float PixelBlurConst = frameRate*0.1;

	if (lastVelocitySqMag > curVelocitySqMag)
	{
		pixelVelocity.x = lastVelocity.r*PixelBlurConst;
		pixelVelocity.y = -lastVelocity.g*PixelBlurConst;
	}
	else
	{
		pixelVelocity.x = curVelocity.r*PixelBlurConst;
		pixelVelocity.y = -curVelocity.g*PixelBlurConst;

	}
	
	float3 Blurred = 0;
	for (float i = 0; i < NumberOfPostProcessSamples; i++)
	{
		// Sample texture in a new spot based on pixelVelocity vector 
		// and average it with the other samples        
		float2 lookup = pixelVelocity * i / NumberOfPostProcessSamples + uv;

		// Lookup the color at this new spot
		float4 Current = gInput.SampleLevel(Sampler, lookup,0);

		// Add it with the other samples
		Blurred += Current.rgb;
	}
	gOutput[DTid.xy] = float4(Blurred / NumberOfPostProcessSamples, 1.0f);
}