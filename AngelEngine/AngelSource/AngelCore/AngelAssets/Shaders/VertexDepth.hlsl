#include"ParamDepth.hlsli"
#include"ParamForwardRenderer.hlsli"

PS_IN main( VS_IN IN )
{
	PS_IN OUT = (PS_IN)0;
	OUT.oPositionLight = mul(IN.pos, WVP);
	float4 positionW = mul(IN.pos, World);
	OUT.lightVec = light[0].PositionWS - positionW.xyz;
	return OUT;
}