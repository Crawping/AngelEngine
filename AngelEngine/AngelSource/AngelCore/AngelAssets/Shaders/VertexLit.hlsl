#include"ParamLit.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
};

PS_INLIT main(VS_INLIT IN)
{
	PS_INLIT OUT;
	//transform homogeneus clip space
	OUT.posH = mul(float4(IN.pos,1.0f), WVP);
	OUT.normal = IN.normal;
	OUT.uv = IN.uv;
	
	return OUT;
}