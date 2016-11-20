#include"ParamBillBoardText.hlsli"

[maxvertexcount(4)]
void main(point VS_OUT gin[1],
	uint primID : SV_PrimitiveID,
	inout TriangleStream<VS_OUT> triStream)
{
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = eye.xyz - gin[0].pos.xyz;
	look.y = 0.0f; // y-axis aligned, so project to xz-plane
	look = normalize(look);
	float3 right = cross(up, look);

	//
	// Compute triangle strip vertices (quad) in world space.
	//
	float halfWidth = 0.5f*1;
	float halfHeight = 0.5f*1;

	float4 v[4];
	v[0] = float4(gin[0].pos.xyz + halfWidth*right - halfHeight*up, 1.0f);
	v[1] = float4(gin[0].pos.xyz + halfWidth*right + halfHeight*up, 1.0f);
	v[2] = float4(gin[0].pos.xyz - halfWidth*right - halfHeight*up, 1.0f);
	v[3] = float4(gin[0].pos.xyz - halfWidth*right + halfHeight*up, 1.0f);

	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
	float2 gTexC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};
	VS_OUT gout;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		gout.pos = mul(v[i], transpose(View));
		gout.uv = gTexC[i];

		triStream.Append(gout);
	}
}