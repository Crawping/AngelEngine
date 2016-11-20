#include"ParamForwardRenderer.hlsli"

PS_IN_TERRAIN main(VS_IN_TERRAIN IN )
{
	PS_IN_TERRAIN OUT = (PS_IN_TERRAIN)0;
	OUT.pos = mul(float4(IN.pos,1.0f), WVP);
	OUT.normal = mul(float4(IN.normal, 1.0f), World);
	OUT.uv = IN.uv;
	OUT.posW= mul(float4(IN.pos, 1.0f), World);
	OUT.shadowPosH = mul(float4(IN.pos,1.0f), World);
	OUT.shadowPosH = mul(OUT.shadowPosH, lightViewMatrix);
	OUT.shadowPosH = mul(OUT.shadowPosH, lightProjectionMatrix);
	OUT.color = IN.color;
	OUT.layers = IN.layers;
	OUT.tileNumber = IN.tileNumber;
	return OUT;
}