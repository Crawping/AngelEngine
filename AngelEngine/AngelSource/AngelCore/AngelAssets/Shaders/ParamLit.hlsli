
struct VS_INLIT
{
	float3 pos : POSITION0;
	float3 normal :NORMAL0;
	float4 tangent :TANGENT;
	float4 color :COLOR;
	float2 uv : TEXCOORD0;
};
struct PS_INLIT
{
	float4 posH : SV_POSITION0;
	float4 posW : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};
