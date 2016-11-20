struct VS_IN_DEFFERED_RENDERER
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct PS_IN_DEFFERED_RENDERER
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct VS_IN_MESH
{
	float3 PosL     : POSITION0;
	float3 Normal  : NORMAL0;
	float4 Tangent : TANGENT0;
	float4 Color   : COLOR0;
	float2 UV      : TEXCOORD0;
};

struct PS_IN_MESH
{
	float3 positionVS   : TEXCOORD0;    // View space position.
	float2 texCoord     : TEXCOORD1;    // Texture coordinate
	float3 tangentVS    : TANGENT;      // View space tangent.
	float3 binormalVS   : BINORMAL;     // View space binormal.
	float3 normalVS     : NORMAL;       // View space normal.
	float4 position     : SV_POSITION;  // Clip space position.
};

struct PixelShaderOutput
{
	float4 Diffuse              : SV_Target1;   // Diffuse Albedo (R8G8B8_UNORM) Material Type (A8_UNORM)
	float4 Specular             : SV_Target2;   // Specular Color (R8G8B8_UNROM) Specular Power(A8_UNORM)
	float4 NormalVS             : SV_Target3;   // View space normal (R32G32B32_FLOAT) Unused (A32_FLOAT)
};
cbuffer ScreenToViewParams : register(b3)
{
	float4x4 InverseProjection;
	float2 ScreenDimensions;
}

float4 ClipToView(float4 clip)
{
	// View space position.
	float4 view = mul(InverseProjection, clip);
	// Perspecitive projection.
	view = view / view.w;

	return view;
}

// Convert screen space coordinates to view space.
float4 ScreenToView(float4 screen)
{
	// Convert to normalized texture coordinates
	float2 texCoord = screen.xy / ScreenDimensions;

	// Convert to clip space
	float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

	return ClipToView(clip);
}

struct Material
{
	float4  GlobalAmbient;
	//-------------------------- ( 16 bytes )
	float4  AmbientColor;
	//-------------------------- ( 16 bytes )
	float4  EmissiveColor;
	//-------------------------- ( 16 bytes )
	float4  DiffuseColor;
	//-------------------------- ( 16 bytes )
	float4  SpecularColor;
	//-------------------------- ( 16 bytes )
	float   SpecularPower;
	float   SpecularScale;
	float   AlphaThreshold;
	int		MaterialType;
	//-------------------------- ( 16 bytes )
};  //--------------------------- ( 16 * 6 = 96 bytes )

struct Light
{
	/**
	* Position for point and spot lights (World space).
	*/
	float4   PositionWS;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Direction for spot and directional lights (World space).
	*/
	float4   DirectionWS;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Position for point and spot lights (View space).
	*/
	float4   PositionVS;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Direction for spot and directional lights (View space).
	*/
	float4   DirectionVS;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Color of the light. Diffuse and specular colors are not seperated.
	*/
	float4   Color;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* The half angle of the spotlight cone.
	*/
	float    SpotlightAngle;
	/**
	* The range of the light.
	*/
	float    Range;

	/**
	* The intensity of the light.
	*/
	float    Intensity;

	/**
	* Disable or enable the light.
	*/
	bool    Enabled;
	//--------------------------------------------------------------( 16 bytes )

	/**
	* The type of the light.
	*/
	uint    Type;
	float3  Padding;
	//--------------------------------------------------------------( 16 bytes )
	//--------------------------------------------------------------( 16 * 7 = 112 bytes )
};

cbuffer Material : register(b2)
{
	Material Mat;
};

Texture2D MeshTexture[8] : register(t0);

sampler LinearRepeatSampler     : register(s0);
sampler LinearClampSampler      : register(s1);

float3 ExpandNormal(float3 normal)
{
	return normal*2.0f - 1.0f;
}

float4 DoNormalMapping(float3x3 TBN, Texture2D tex, sampler s, float2 uv)
{
	float3 normal = tex.Sample(s, uv).xyz;
	normal = ExpandNormal(normal);

	// Transform normal from tangent space to view space.
	normal = mul(normal, TBN);
	return normalize(float4(normal, 0));
}

cbuffer cbLight : register(b4)
{
	Light light;
};
struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};
float4 DoDiffuse(Light light, float4 L, float4 N)
{
	float NdotL = max(dot(N, L), 0);
	return light.Color * NdotL;
}

float4 DoSpecular(Light light, Material material, float4 V, float4 L, float4 N)
{
	float4 R = normalize(reflect(-L, N));
	float RdotV = max(dot(R, V), 0);

	return light.Color * pow(RdotV, material.SpecularPower);
}

float DoAttenuation(Light light, float d)
{
	return 1.0f - smoothstep(light.Range * 0.75f, light.Range, d);
}

float DoSpotCone(Light light, float4 L)
{
	// If the cosine angle of the light's direction 
	// vector and the vector from the light source to the point being 
	// shaded is less than minCos, then the spotlight contribution will be 0.
	float minCos = cos(radians(light.SpotlightAngle));
	// If the cosine angle of the light's direction vector
	// and the vector from the light source to the point being shaded
	// is greater than maxCos, then the spotlight contribution will be 1.
	float maxCos = lerp(minCos, 1, 0.5f);
	float cosAngle = dot(light.DirectionVS, -L);
	// Blend between the maxixmum and minimum cosine angles.
	return smoothstep(minCos, maxCos, cosAngle);
}
LightingResult DoPointLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = light.PositionVS - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);

	result.Diffuse = DoDiffuse(light, L, N) * attenuation * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * attenuation * light.Intensity;

	return result;
}

LightingResult DoDirectionalLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = normalize(-light.DirectionVS);

	result.Diffuse = DoDiffuse(light, L, N) * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * light.Intensity;

	return result;
}

LightingResult DoSpotLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = light.PositionVS - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);
	float spotIntensity = DoSpotCone(light, L);

	result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * attenuation * spotIntensity * light.Intensity;

	return result;
}
