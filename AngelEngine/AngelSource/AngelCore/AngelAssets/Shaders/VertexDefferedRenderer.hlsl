#include "ParamDefferedRenderer.hlsli"

cbuffer cbPerObject : register(b0)
{
	float4x4 WVP;
	float4x4 WorldView;
	float4x4 WorldInvTranspose;
};

PS_IN_MESH main(VS_IN_MESH IN)
{
	PS_IN_MESH OUT = (PS_IN_MESH)0;

	OUT.positionVS = mul(float4(IN.PosL,1.0f), WorldView).xyz;
	OUT.texCoord = IN.UV;
	OUT.tangentVS = mul(IN.Tangent, WorldView).xyz;
	OUT.normalVS = mul(IN.Normal, WorldView).xyz;
	OUT.position = mul(float4(IN.PosL, 1.0f), WVP);
	OUT.binormalVS = cross(OUT.tangentVS, OUT.normalVS);
	return OUT;
}