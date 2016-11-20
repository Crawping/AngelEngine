
struct VS_Input
{
	float3 Pos     : POSITION0;
	float3 Normal  : NORMAL0;
	float4 Tangent : TANGENT0;
	float4 Color   : COLOR0;
	float2 UV      : TEXCOORD0;
};

struct PS_Input
{
	float4 Pos      : SV_POSITION0;
	float3 WorldPos : POSITION0;
	float3 Normal   : NORMAL0;
	float3 Tangent  : TANGENT0;
	float2 UV       : TEXCOORD0;
	float4 shadowPosH : TEXCOORD1;
};
