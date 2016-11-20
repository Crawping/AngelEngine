#include"ParamParticle.hlsli"

[maxvertexcount(4)]
void main(point GSParticleIn input[1], inout TriangleStream<PSParticleIn> SpriteStream)
{
	PSParticleIn output;

	float4 orig = mul(float4(input[0].Pos, 1), World);
	output.particleOrig = orig.xyz;

	if (input[0].Life > -1)
	{
		// calculate color from a 1d gradient texture
		float3 particleColor = ColorGradiant.SampleLevel(SamplerLinear, float2(input[0].Life, 0), 0).xyz;
		output.particleColor = particleColor;//Set to zero!

		float3 g_positions[4] =
		{
			float3(-1, 1, 0),
			float3(1, 1, 0),
			float3(-1, -1, 0),
			float3(1, -1, 0),
		};
		float2 g_texcoords[4] =
		{
			float2(0,0),
			float2(1,0),
			float2(0,1),
			float2(1,1),
		};
		//
		// Emit two new triangles
		//
		[unroll] for (int i = 0; i<4; i++)
		{
			float3 position = g_positions[i] * input[0].Size;
			position = mul(position, (float3x3)InvView) + input[0].Pos;
			output.Pos = mul(float4(position, 1.0), WVP);

			// pass along the texcoords
			output.Tex = float3(g_texcoords[i], input[0].Life);

			// screenspace coordinates for the lookup into the depth buffer
			output.ScreenTex = output.Pos.xy / output.Pos.w;

			// output depth of this particle
			output.Depth = output.Pos.zw;

			// size
			output.Size = input[0].Size;

			// world position
			float4 posWorld = mul(float4(position, 1.0), World);
			output.worldPos = posWorld.xyz;

			SpriteStream.Append(output);
		}
		SpriteStream.RestartStrip();
	}
}