#include"ParamBillBoardText.hlsli"

VS_OUT main( VS_IN IN )
{
	VS_OUT OUT = (VS_OUT)0;
	OUT.pos = float4(IN.pos,1.0f);
	OUT.pos = mul(OUT.pos, WVP);
	OUT.uv = IN.uv;
	return OUT;
}