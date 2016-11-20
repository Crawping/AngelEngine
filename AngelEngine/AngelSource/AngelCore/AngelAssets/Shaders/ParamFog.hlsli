cbuffer cbFog : register(b5)
{
	float4 fogColor;
	float fogStart;
	float fogRange;
	bool fogEnable;
	float padding;
}