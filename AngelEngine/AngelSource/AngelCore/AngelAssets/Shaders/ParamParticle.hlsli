struct VSParticleIn
{
	float3 Pos	  : POSITION0;
	float3 Vel	  : VELOCITY0;
	float Life	  : LIFE0;
	float Size    : SIZE0;
};

struct GSParticleIn
{
	float3 Pos    : POSITION0;
	float3 Vel	  : VELOCITY0;
	float Life	  :	LIFE0;	//stage of animation we're in [0..1] 0 is first frame, 1 is last
	float Size    : SIZE0;
};

struct PSParticleIn
{
	float4 Pos			  : SV_POSITION;
	float3 Tex			  : TEXCOORD0;
	float2 ScreenTex	  : TEXCOORD1;
	float2 Depth		  : TEXCOORD2;
	float  Size			  : TEXCOORD3;
	float3 worldPos		  : TEXCOORD4;
	float3 particleOrig	  : TEXCOORD5;
	float3 particleColor  : TEXCOORD6;
};

cbuffer cbPerObject : register(c0)
{
	matrix WVP;
	matrix World;
	matrix InvView;
	matrix InvProjection;
};

cbuffer cbImmutable : register(c1)
{
	
	
	float4 g_directional1 = float4(0.992, 1.0, 0.880, 0.0);
	float4 g_directional2 = float4(0.595, 0.6, 0.528, 0.0);
	float4 g_ambient = float4(0.525, 0.474, 0.474, 0.0);
};


cbuffer cbUser : register(c2)
{
	float g_fFadeDistance = 10.0f;
	float g_fSizeZScale = 5.0f;
	/*float2 g_vScreenSize;
	float4 g_vViewLightDir1;
	float4 g_vViewLightDir2;
	float4 g_vWorldLightDir1;
	float4 g_vWorldLightDir2;
	float4 g_vEyePt;*/

	float g_stepSize = 0.01;
	float g_noiseSize = 40.0;
	float g_noiseOpacity = 20.0;
};

Texture2D ColorGradiant : register(t0); // Color change over time.
Texture2D ParticleTexture : register(t1);//Main texture.
Texture2D DepthTexture		:register(t2);

SamplerState SamplerLinear : register(s0);
SamplerState SamplerPoint : register(s1);
