#include"ParamForwardRenderer.hlsli"

float VectorToDepthValue(float3 Vec)
{
	float3 AbsVec = abs(Vec);
	float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

	const float f = 1000.0;      // TODO: TEMP - use constant buffer values!
	const float n = 0.1;
	float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;


	return (NormZComp + 1.0) * 0.5;
}



SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};


PixelShaderOutput main(PS_IN_MESH IN) : SV_TARGET
{
	PixelShaderOutput OUT = (PixelShaderOutput)0;
	//return MeshTextureDiffuse.Sample(LinearRepeatSampler, IN.texCoord);
	float4 diffuse = Mat.DiffuseColor;
	float4 ambient = Mat.AmbientColor * MeshTexture[0].Sample(LinearRepeatSampler, IN.texCoord);
	
	if (Mat.MaterialType == 0)
	{
		float4 diffuseTex = MeshTexture[0].Sample(LinearRepeatSampler, IN.texCoord);
		if (any(diffuseTex.rgb))
		{
			diffuse *= diffuseTex;
		}
		if (diffuseTex.a < 0.2f) discard;
		OUT.color = diffuseTex;
		OUT.velocityUV = float4(IN.velocity.xy, 1.0f, 1.0f);
		return OUT;
	}
	
	if (Mat.MaterialType >= 1)
	{
		float4 diffuseTex = MeshTexture[0].Sample(LinearRepeatSampler, IN.texCoord);
		if (any(diffuseTex.rgb))
		{
			diffuse *= diffuseTex;
		}
		else
		{
			diffuse = diffuseTex;
		}
	}
	
	//By default,set alpha by diffuse color.
	float alpha = diffuse.a;
	if (alpha < 0.2f) discard;
	//Skip Opacity Texture For Now.
	//Skip Ambient Texture For Now.
	
	//Second Index In Textures Will Be NormalMap
	
	float4 N;
	
	if (Mat.MaterialType >= 2)
	{
		float3x3 TBN = float3x3(normalize(IN.tangentVS),
			normalize(IN.binormalVS),
			normalize(IN.normalVS));
	
		N = DoNormalMapping(TBN,
			MeshTexture[1],
			LinearRepeatSampler,
			IN.texCoord);
		//N = normalize(float4(IN.normalVS, 0));
	}
	else
	{
		N = normalize(float4(IN.normalVS, 0));
	}
	float visibility = 1.0f;
	float4 P = float4(IN.positionVS, 1);
	float2 projectTexCoord;
	float bias = 0.001;
	projectTexCoord.x = IN.shadowPosH.x / IN.shadowPosH.w / 2.0f + 0.5f;
	projectTexCoord.y = -IN.shadowPosH.y / IN.shadowPosH.w / 2.0f + 0.5f;
	float lightDepthValue = IN.shadowPosH.z / IN.shadowPosH.w;
	visibility = CalcShadowFactor(SamplerClamp, shadowMap, lightDepthValue - bias, projectTexCoord);
	float4 PLightDirection = 0.0f;
	//float3 cubemapDir = (float3)(P.xyz - light[0].PositionWS.xyz);
	//cubemapDir.z = -cubemapDir.z;
	/*PLightDirection.xyz = light[0].PositionWS.xyz - P.xyz;
	float distance = length(PLightDirection.xyz);
	PLightDirection.xyz = PLightDirection.xyz / distance;
	float shadowMapDepth = cubeShadowMap.Sample(shadowMapSampler, 
		float3(-PLightDirection.xy, -PLightDirection.z)).x;*/
	//float storedDepth = cubeShadowMap.Sample(shadowMapSampler, cubemapDir);
	//return float4(depthValue, depthValue, depthValue, 1.0f);
	//float3 AbsVec = abs(cubemapDir);
	//float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

	//const float f = 100.0;      // TODO: TEMP - use constant buffer values!
	//const float n = 0.1;
	//float NormZComp = (f + n) / (f - n) - (2 * f * n) / (f - n) / LocalZcomp;
	
	//if (storedDepth + 0.0001 > VectorToDepthValue(cubemapDir))
		//visibility = 1.0;
	
	//return (NormZComp + 1.0) * 0.5;
	//if (distance >shadowMapDepth) return float4(0.1f, 0.1f, 0.1f, 1.1f);
	//Skip Bump Mapping

	LightingResult lit = DoLighting(Mat, P, N);
	diffuse *= float4(lit.Diffuse.rgb, 1.0f); // Discard the alpha value from the lighting calculations.
	
	float specularPower = Mat.SpecularPower;
	float4 specular = 0;
	//Third Index Will Be Specular Map
	if (specularPower > 0.0f)
	{
		specular.rgb = Mat.SpecularColor.rgb;
		if (Mat.MaterialType >= 3)
		{
			float4 specularTex = MeshTexture[2].Sample(LinearRepeatSampler, IN.texCoord);
			if (any(specular.rgb))
			{
				specular *= specularTex;
			}
			else
			{
				specular = specularTex;
			}
		}
		specular *= lit.Specular;
	}
	OUT.color = float4(((diffuse.xyz + specular.xyz).rgb*visibility + ambient.rgb), alpha);
	
	//OUT.velocityUV.x = OUT.velocityUV.x*0.5 + 0.5;
	//OUT.velocityUV.y = OUT.velocityUV.y*0.5 + 0.5;
	OUT.velocityUV /= IN.position.w;
	OUT.velocityUV = float4(IN.velocity.xy/2, 1.0f, 1.0f);
	OUT.ZBuffer = float4(IN.position.z/ IN.position.w,0.0f, 0.0f, 1.0f);
	return OUT;
}