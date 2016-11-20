#include"ParamDepth.hlsli"
#include"ParamForwardRenderer.hlsli"
float4 main(PS_IN IN) : SV_TARGET
{
	return length(IN.lightVec);
}