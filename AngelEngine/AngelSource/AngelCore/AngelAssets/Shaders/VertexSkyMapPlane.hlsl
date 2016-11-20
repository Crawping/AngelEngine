#include"ParamSkyMapPlane.hlsli"

cbuffer WVP : register(b0)
{
	matrix WVP;
}



PS_IN main(VS_IN In)
{
	PS_IN Out = (PS_IN)0;
	Out.posH = mul(In.posL, WVP);
	Out.uv = In.uv;
	return Out;
}