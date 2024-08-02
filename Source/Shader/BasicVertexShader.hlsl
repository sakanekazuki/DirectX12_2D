#include"BasicShaderHeader.hlsli"

// 定数バッファ
cbuffer cbuff0 : register(b0)
{
	matrix mat;
};

Output BasicVS(float4 pos : POSITION,
	float2 uv : TEXCOORD)
{
	Output output;
	output.svpos = mul(mat, pos);
	output.uv = uv;
	return output;
}