cbuffer ObjectInfo : register(b0)
{
	float4x4 VP;
	float4 EyePosW;
	float4x4 rotation;
}

cbuffer ColorChanging : register(b2)
{
	float4 colorStart;
	float4 colorEnd;
	float colorChangeSpeed;
	bool fadeOverTime;
	float fadeSpeed;
	float paddingColorChange;
}

cbuffer SizeChanging : register(b3)
{
	float2 startSize;
	float2 endSize;
	float sizeChangeSpeed;
	bool sizeChangeOverTime;
	float2 paddingSizeChange;
}

cbuffer EmittSpeed : register(b4)
{
	float emittSpeed;
	float destroyTimeOut;
	float2 paddEmittSpeed;
}

cbuffer Acceleration : register(b5)
{
	float3 accDir;
	float accSpeed;
}

struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW : SIZE;
	float Age : AGE;
	uint Type : TYPE;
};

cbuffer ParticleInfo : register(b1)
{
	float4 gEmitPosW;
	float4 gEmitDirW;
	float gGameTime;
	float gTimeStep;
	float2 padd;
}

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};


struct VertexOut
{
	float4 PosW : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint Type : TYPE;
	float Age : AGE;
};

struct GSOutput
{
	float4 PosH : SV_Position;
	float4 Color : COLOR;
	float2 Tex : TEXCOORD;
	float Age : AGE;

};

cbuffer TextureChanging : register(b6)
{
	float changeTime;
	float3 padddd;
};


Texture1D gRandomTex: register(t0);
Texture2D ParticleTexture[8] : register(t1);
SamplerState Sampler : register(s0);

float3 RandVec3(float offset)
{
	float u = (gGameTime + offset);

	float3 ret = gRandomTex.SampleLevel(samLinear, u, 0);

	return ret;
}