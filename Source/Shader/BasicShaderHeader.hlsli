// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
    // システム用頂点座標
	float4 svpos : SV_POSITION;
    // uv値
	float2 uv : TEXCOORD;
};

// 0番スロットに設定されたテクスチャ
Texture2D<float4> tex : register(t0);
// 0番スロットに設定されたサンプラー
SamplerState smp : register(s0);

// 定数バッファ
cbuffer cbuff0 : register(b0)
{
	matrix mat;
};