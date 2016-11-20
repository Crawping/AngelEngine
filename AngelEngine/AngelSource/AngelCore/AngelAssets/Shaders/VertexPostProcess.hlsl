#include"ParamPostProcess.hlsli"

PS_IN main(VS_IN IN)
{
	PS_IN OUT;
	OUT.posH = IN.pos;
	OUT.uv = IN.uv;
	return OUT;
}