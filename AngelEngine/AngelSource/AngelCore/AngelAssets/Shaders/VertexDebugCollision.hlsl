#include"ParamDebugCollision.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
};

PS_IN main(VS_IN IN)
{
	PS_IN OUT;
	//transform homogeneus clip space
	OUT.posH = mul(float4(IN.pos,1.0f), WVP);
	return OUT;
}