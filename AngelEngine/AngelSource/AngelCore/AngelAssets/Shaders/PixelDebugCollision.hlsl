#include"ParamDebugCollision.hlsli"


cbuffer cbcolor : register(b1)
{
	float4  color;
};
float4 main(PS_IN IN) : SV_TARGET
{
	return color;
}