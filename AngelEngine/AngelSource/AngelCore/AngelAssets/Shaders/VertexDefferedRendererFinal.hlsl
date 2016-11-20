#include"ParamDefferedRenderer.hlsli"

PS_IN_DEFFERED_RENDERER main(
	VS_IN_DEFFERED_RENDERER IN)
{
	PS_IN_DEFFERED_RENDERER OUT;
	OUT.pos = float4(IN.pos,1.0f);
	OUT.uv = IN.uv;
	return OUT;
}