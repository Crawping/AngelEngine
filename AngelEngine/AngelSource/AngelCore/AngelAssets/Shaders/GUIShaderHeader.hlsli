struct VS_IN
{
	float2 pos : POSITION; 
	float4 color : COLOR0; 
	float2 uv  : TEXCOORD0; 
};

struct PS_IN
{
	float4 posH : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};