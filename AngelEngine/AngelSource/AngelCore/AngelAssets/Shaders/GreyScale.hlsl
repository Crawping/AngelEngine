Texture2D gInput : register(t0);

SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput : register(u0);

#define N 256

[numthreads(N, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float4 colorAvg = (gInput[DTid.xy].x + gInput[DTid.xy].y + gInput[DTid.xy].z) / 3.0f;
	gOutput[DTid.xy] = colorAvg;
}