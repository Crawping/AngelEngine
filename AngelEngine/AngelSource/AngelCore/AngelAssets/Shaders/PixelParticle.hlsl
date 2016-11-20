#include"ParamParticle.hlsli"

float4 main(PSParticleIn input) : SV_TARGET
{
	float2 screenTex = 0.5*((input.ScreenTex) + float2(1,1));
	screenTex.y = 1 - screenTex.y;

	float4 particleSample = ParticleTexture.Sample(SamplerLinear, input.Tex);

	float particleDepth = input.Depth.x;
	particleDepth /= input.Depth.y;

	float depthFade = 1;
	//if (bSoftParticles)
	{
		// We need to determine the distance between the value stored in the depth buffer
		// and the value that came in from the GS
		// Because the depth values aren't linear, we need to transform the depthsample back into view space
		// in order for the comparison to give us an accurate distance
		float depthSample;


		depthSample = DepthTexture.Sample(SamplerPoint, screenTex);

		float4 depthViewSample = mul(float4(input.ScreenTex, depthSample, 1), InvProjection);
		float4 depthViewParticle = mul(float4(input.ScreenTex, particleDepth, 1), InvProjection);

		float depthDiff = (-depthViewSample.z / depthViewSample.w) - (-depthViewParticle.z / depthViewParticle.w);
		if (depthDiff < 0)
			discard;

		depthFade = saturate(depthDiff / 0.00001);
	}
	particleSample.rgb *= float3(1.0f,1.0f,1.0f);
	particleSample.a *= depthFade;

	return particleSample;
}