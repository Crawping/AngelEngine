#include"ParamForwardRenderer.hlsli"

PixelShaderOutput main(PS_IN_TERRAIN In) : SV_TARGET
{
	PixelShaderOutput OUT = (PixelShaderOutput)0;
	float4 textureColor = 0;
	float4 N;
	float2 uv = float2(0, 0);

	N = normalize(float4(In.normal, 0));
	float4 P = float4(In.posW, 1);
	float2 projectTexCoord;
	float bias = 0.001;
	projectTexCoord.x = In.shadowPosH.x / In.shadowPosH.w / 2.0f + 0.5f;
	projectTexCoord.y = -In.shadowPosH.y / In.shadowPosH.w / 2.0f + 0.5f;
	float depthValue = shadowMap.Sample(LinearClampSampler, projectTexCoord).r;
	float lightDepthValue = In.shadowPosH.z / In.shadowPosH.w;
	lightDepthValue -= bias;
	float visibility = 0.0;
	visibility = CalcShadowFactor(SamplerClamp, shadowMap, lightDepthValue - 0.001f, projectTexCoord);
	float4 PLightDirection = 0.0f;
	LightingResult lit = DoLighting(Mat, P, N);
	float4 diffuse = float4(lit.Diffuse.rgb, 1.0f); // Discard the alpha value from the lighting calculations.
	float4 ambient = float4(1.0f, 1.0f, 1.0f, 1.0f);

	
	float cellWidth = width / numTileWidth;
	float cellPercentage = cellWidth / width;
	
	float cellHeight = height / numTileHeight;
	float cellHpercentage = cellHeight / height;
	//if(In.tileNumber<20)
	//uv = float2((In.uv.x*cellPercentage + cellPercentage*(In.tileNumber % (numTileWidth+1))),
		//(In.uv.y*cellHpercentage + cellHpercentage*((In.tileNumber / (numTileHeight+1)))));
	
	//textureColor = fullTexture.Sample(LinearClampSampler, uv);

	textureColor = lerp(terrainTexture[1].Sample(LinearClampSampler, uv),
		terrainTexture[0].Sample(LinearClampSampler, In.uv), In.layers[0]);
	textureColor = lerp(textureColor,
		terrainTexture[1].Sample(LinearClampSampler, In.uv), In.layers[1]);

	textureColor = lerp(textureColor,
		terrainTexture[2].Sample(LinearClampSampler, In.uv), In.layers[2]);

	textureColor = lerp(textureColor,
		terrainTexture[3].Sample(LinearClampSampler, In.uv), In.layers[3]);

	ambient *= textureColor;
	diffuse *= textureColor;
	OUT.color = float4(((diffuse).rgb) * visibility + ambient, 1.0f) * float4(In.color, 1.0f);
	//OUT.color = float4(In.tileNumber/81, In.tileNumber / 81, In.tileNumber / 81,1.0f) ;
	
	//OUT.velocityUV.x = OUT.velocityUV.x*0.5 + 0.5;
	//OUT.velocityUV.y = OUT.velocityUV.y*0.5 + 0.5;
	OUT.velocityUV /= In.pos.w;
	OUT.velocityUV = float4(1.0f,1.0f, 1.0f, 1.0f);
	OUT.ZBuffer = float4(In.pos.z / In.pos.w,0.0f, 0.0f, 1.0f);
	return OUT;
}