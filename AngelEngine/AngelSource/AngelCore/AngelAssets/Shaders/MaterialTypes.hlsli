

struct MeshMaterial
{
	float4 diffuse;
	float4 specular;
	float4 ambient;
	float4 colorTint;
	float4 emissive;
	bool useTexture;
	float specularPower;
	bool defaultAlpha;
	float specularIntensity;
};

struct SpriteMaterial
{
	float4 colorTint;
	bool useSprite;
	float3 padding;
};