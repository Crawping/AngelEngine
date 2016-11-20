#include"ParamDefferedRenderer.hlsli"

PixelShaderOutput main(PS_IN_MESH IN) : SV_TARGET
{
	PixelShaderOutput OUT;
	float4 diffuse = Mat.DiffuseColor;
	if (Mat.MaterialType==0)
	{
		float4 diffuseTex = MeshTexture[0].Sample(LinearRepeatSampler, IN.texCoord);
		if (any(diffuse.rgb))
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
	if (alpha < Mat.AlphaThreshold) discard;
	//Skip Opacity Texture For Now.
	//Skip Ambient Texture For Now.
	float4 ambient = Mat.AmbientColor;
	ambient*= Mat.GlobalAmbient;
	//Second Index In Textures Will Be NormalMap

	float4 N;

	if (Mat.MaterialType == 1)
	{
		float3x3 TBN = float3x3(normalize(IN.tangentVS),
			normalize(IN.binormalVS),
			normalize(IN.normalVS));

		N = DoNormalMapping(TBN, 
			MeshTexture[1],
			LinearRepeatSampler,
			IN.texCoord);
	}
	else
	{
		N = normalize(float4(IN.normalVS, 0));
	}

	OUT.NormalVS = N;
	//Skip Bump Mapping

	float specularPower = Mat.SpecularPower;
	float4 specular = 0;
	//Third Index Will Be Specular Map
	if (specularPower > 1.0f)
	{
		specular = Mat.SpecularColor;
		if (Mat.MaterialType == 2)
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
	}
	OUT.Specular = float4(specular.rgb, log2(specularPower) / 10.5f);
	return OUT;
}