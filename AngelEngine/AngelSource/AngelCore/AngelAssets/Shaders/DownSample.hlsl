#include"PostProcessShared.hlsli"
Texture2D gInput : register(t0);

SamplerState Sampler : register(s0);
RWTexture2D<float4> gOutput : register(u0);

cbuffer Offsets : register(c0)
{
	float4      tcDSOffsets[9];
}

#define N 256
#define SampleCount 16
[numthreads(N, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//float2 uv = float2(DTid.xy) / float2(800,600);//Temp Texture Coordinates.
	////float4 color = gInput.SampleLevel(Sampler, uv,3);
	//float3 colorAvg = 0;
	//float x = 0;
	//float y = 0;
	//float z = 0;
	//float2 uv = DTid.xy;
	//
	//for (int i = 1;i < SampleCount;i++)
	//{
	//	colorAvg += gInput[float2(uv.x,uv.y+i)].xyz;//Up
	//	colorAvg += gInput[float2(uv.x, uv.y - i)].xyz;//Down
	//	colorAvg += gInput[float2(uv.x-i, uv.y)].xyz;//Left
	//	colorAvg += gInput[float2(uv.x+i, uv.y)].xyz;//Right

	//	//x += gInput[float2(uv.x, uv.y + 1)].x;//Up
	//	//x += gInput[float2(uv.x, uv.y - 1)].x;//Down
	//	//x += gInput[float2(uv.x - 1, uv.y)].x;//Left
	//	//x += gInput[float2(uv.x + 1, uv.y)].x;//Right

	//	//y += gInput[DTid.xy + i].y;
	//	//z += gInput[DTid.xy + i].z;
	//	/*float4 colorAvg = float4((gInput[DTid.xy].x + gInput[DTid.xy + i].x) / 2,
	//		(gInput[DTid.xy].y + gInput[DTid.xy + i].y) / 2, (gInput[DTid.xy + 1].z + gInput[DTid.xy + 1].z) / 2, 1.0f);*/
	//}
	//float4 t = 0;
	//for (int i = 0;i < 9;i++)
	//{
	//	t += float4(tcDSOffsets[i].x * 50, tcDSOffsets[i].y, tcDSOffsets[i].z, 1.0f);
	//}
	//

	// Compute the average of the 10 necessary samples
	float4 color = 0.0f;
	float maximum = -1e20;
	float4 average = 0.0f;
	float2 uv = float2(DTid.xy) / float2(WindowSize.x, WindowSize.y);//Temp Texture Coordinates.
	for (int i = 0; i < 9; i++)
	{
		color = gInput.SampleLevel(Sampler, uv + float2(tcDSOffsets[i].x, tcDSOffsets[i].y),0);
		average += color;
		maximum = max(maximum, color.g);
	}

	// We've just taken 9 samples from the
	// high resolution texture, so compute the
	// actual average that is written to the
	// lower resolution texture (render target).
	average /= 9.0f;

	// Return the new average luminance
	gOutput[DTid.xy] = average;
}