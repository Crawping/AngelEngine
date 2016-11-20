#include"ParamDefferedRenderer.hlsli"

// The diffuse color from the view space texture.
Texture2D DiffuseTextureVS : register(t0);
// The specular color from the screen space texture.
Texture2D SpecularTextureVS : register(t1);
// The normal from the screen space texture.
Texture2D NormalTextureVS : register(t2);
// The depth from the screen space texture.
Texture2D DepthTextureVS : register(t3);

float4 main(PS_IN_DEFFERED_RENDERER IN) : SV_TARGET0
{
	float4 eyePos = { 0, 0, 0, 1 };
	float2 texCoord = IN.uv;
	float depth = DepthTextureVS.Load
		(int3(texCoord, 0)).r;
	float4 P = ScreenToView(float4(texCoord, depth, 1.0f));
	float4 V = normalize(eyePos - P);
	float4 diffuse = DiffuseTextureVS.Load(int3(texCoord, 0));
	return depth;
	float4 specular = SpecularTextureVS.Load(int3(texCoord, 0));
	float4 N = NormalTextureVS.Load(int3(texCoord, 0));
	float specularPower = exp2(specular.a * 10.5f);
	int MaterialType = diffuse.a;
	//Light light = Lights[LightIndex];

	Material mat = (Material)0;
	mat.DiffuseColor = diffuse;
	mat.SpecularColor = specular;
	mat.SpecularPower = specularPower;
	mat.MaterialType = MaterialType;

	LightingResult lit = (LightingResult)0;

	switch (light.Type)
	{
	case 0:
		lit = DoDirectionalLight(light, mat, V, P, N);
		break;
	case 1:
		lit = DoPointLight(light, mat, V, P, N);
		break;
	case 2:
		lit = DoSpotLight(light, mat, V, P, N);
		break;
	}

	return (diffuse * lit.Diffuse) + (specular * lit.Specular);
}

//#define MAX_LIGHT 2
//SamplerState SampleTypePoint : register(s0);
//
//struct Light
//{
//	float3 position;
//	int	   type;
//	float3 direction;
//	float  range;
//	float4 lightcolor;
//};
//
//
//cbuffer cbLight : register(b0)
//{
//	Light light[MAX_LIGHT];
//};
//
//cbuffer cbGBufferUnpack: register(b1)
//{
//	float4 PerspectiveValues : packoffset(c0);
//	float4x4 ViewInv : packoffset(c1);
//}
//
//struct SURFACE_DATA
//{
//	float LinearDepth;
//	float3 Color;
//	float3 Normal;
//	float  SpecInt;
//	float  SpecPow;
//	float3 posW;
//};
//
//SURFACE_DATA UnpackGBuffer(float2 uv)
//{
//	SURFACE_DATA OUT;
//	float4 unpackedDepth = depthTexture.Sample(SampleTypePoint, uv);
//	OUT.LinearDepth = unpackedDepth.x;
//	float4 colors = colorTexture.Sample(SampleTypePoint, uv);
//	OUT.Color = colors.xyz;
//	OUT.SpecInt = colors.w;
//	float4 normals = normalTexture.Sample(SampleTypePoint, uv);
//	OUT.Normal = normalize(normals.xyz*2.0f - 1.0f);
//	OUT.posW=posWTexture.Sample(SampleTypePoint, uv);
//	return OUT;
//
//}
//
//float ConvertDepthToLinear(float depth)
//{
//	float linearDepth = PerspectiveValues.z / (depth +
//	PerspectiveValues.w);
//	return linearDepth;
//}
//
//float3 CalcWorldPos(float2 csPos, float linearDepth)
//{
//	float4 position;
//	position.xy = csPos.xy * PerspectiveValues.xy * linearDepth;
//	position.z = linearDepth;
//	position.w = 1.0;
//	return mul(position, ViewInv).xyz;
//}
//
//float3 CalcPoint(float3 positionW,float3 normal,float4 color,Light l)
//{
//	float3 ToLight = l.position.xyz - positionW;
//	float DistToLight = length(ToLight);
//	//float DistToLight = length(ToLight);
//	ToLight /= DistToLight; // Normalize
//	float NDotL = saturate(dot(ToLight, normal));
//	float3 finalColor = color.rgb * NDotL;
//	// Blinn specular
//	/*ToEye = normalize(ToEye);
//	float3 HalfWay = normalize(ToEye + ToLight);
//	float NDotH = saturate(dot(HalfWay, material.normal));
//	finalColor += PointColor.rgb * pow(NDotH, material.specExp) *
//		material.specIntensity;*/
//	// Attenuation
//	float DistToLightNorm = 1.0 - saturate(DistToLight *
//		l.range);
//	float Attn = DistToLightNorm * DistToLightNorm;
//	finalColor *= l.lightcolor * Attn;
//	return finalColor;
//}
//
//float4 main(PS_IN IN) : SV_TARGET0
//{
//	
//	SURFACE_DATA data;
//	data = UnpackGBuffer(IN.uv);
//	;
//	if (data.Normal.x == 1 &&
//		data.Normal.y == 1 &&
//		data.Normal.z == 1)
//	{
//		return float4(data.Color, 1.0f);
//	}
//	//float3 position= CalcWorldPos()
//	/*float4 unpackedDepth = depthTexture.Sample(SampleTypePoint, IN.uv);
//	float depth = unpackedDepth.x;
//	normals = normalize(normals);
//	float3 lightDir = -direction;
//	float intensity = saturate(dot(lightDir, normals.xyz));
//	float4 finalColor = colors;
//	finalColor += saturate(intensity*colors*color);
//	finalColor.a = 1.0f;*/
//	float4 Point = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	for (int i = 0;i <MAX_LIGHT;i++)
//	{
//		Point.rgb+=CalcPoint(data.posW.xyz, data.Normal, float4(data.Color, 1.0f),light[i]);
//	}
//	return float4(Point.rgb+data.Color,1.0f);
//	//return float4(data.Color, 1.0f);
//}