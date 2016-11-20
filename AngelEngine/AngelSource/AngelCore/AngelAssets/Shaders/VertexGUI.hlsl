#include"ParamGUI.hlsli"

cbuffer VertexTranslation : register(b0)
{
	row_major float4x4 vertexPositive1Positive1Tranlation;
	row_major float4x4 vertexNegative1Negative1Tranlation;
	row_major float4x4 vertexNegative1Positive1Tranlation;
	row_major float4x4 vertexPositive1Negative1Tranlation;
};

PS_IN main( VS_IN IN )
{
	PS_IN OUT = (PS_IN)0;
	OUT.pos = float4(IN.pos,1.0f);

	//+1 +1
	if (IN.pos.x == 1 && IN.pos.y == 1)
		OUT.pos = mul(OUT.pos, (vertexPositive1Positive1Tranlation));
	//-1 -1
	if (IN.pos.x == -1 && IN.pos.y == -1)
		OUT.pos = mul(OUT.pos, (vertexNegative1Negative1Tranlation));
	//-1 +1
	if (IN.pos.x == -1 && IN.pos.y == 1)
		OUT.pos = mul(OUT.pos, (vertexNegative1Positive1Tranlation));
	//+1 -1
	if (IN.pos.x == 1 && IN.pos.y == -1)
		OUT.pos = mul(OUT.pos, (vertexPositive1Negative1Tranlation));

	OUT.uv = IN.uv;
	return OUT;
}