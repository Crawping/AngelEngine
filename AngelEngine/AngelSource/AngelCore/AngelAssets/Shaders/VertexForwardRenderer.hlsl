#include "ParamForwardRenderer.hlsli"



PS_IN_MESH main(VS_IN_MESH IN)
{
	PS_IN_MESH OUT = (PS_IN_MESH)0;
	float4x3 skinning = 0;
	if (Mat.Skinning == 1)
	{
		[unroll]
		for (int i = 0; i < 4; i++)
		{
			skinning += Bones[IN.Indices[i]] * IN.Weights[i];
		}
		float3 skinnedPos = mul(IN.PosL, skinning);
		OUT.position = mul(float4(skinnedPos, 1.0f), WVP);
		OUT.positionVS = mul(float4(skinnedPos,1.0f), World).xyz;
	}
	else if (Mat.Instancing == 1)
	{
		float4 InstancePosition = mul(IN.PosL, IN.mTransform);
		OUT.position = mul(InstancePosition, WVP);
		OUT.positionVS = mul((IN.PosL), World).xyz;
	}
	else
	{
		OUT.position = mul(IN.PosL, WVP);
		OUT.positionVS = mul((IN.PosL), World).xyz;
	}
	
	
	OUT.texCoord = IN.UV;
	OUT.tangentVS = mul(IN.Tangent, World).xyz;
	OUT.normalVS = mul(IN.Normal, World).xyz;
	
	float4 posCurrent = OUT.position;
	float4 posLast= mul((IN.PosL), WVPLast);
	//posCurrent /= posCurrent.w;
	//posLast /= posLast.w;
	float2 velocity = posCurrent - posLast;
	velocity /= 2.0f;
	if (Mat.Skinning == 1)
		velocity /= 10.0f;
	OUT.velocity = velocity;
	OUT.binormalVS = cross(OUT.tangentVS, OUT.normalVS);
	OUT.shadowPosH = mul((IN.PosL), World);
	OUT.shadowPosH = mul(OUT.shadowPosH, lightViewMatrix);
	OUT.shadowPosH = mul(OUT.shadowPosH, lightProjectionMatrix);

	return OUT;
}