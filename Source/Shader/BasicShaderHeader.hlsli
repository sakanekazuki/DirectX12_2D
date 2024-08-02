// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
    // システム用頂点座標
	float4 svpos : SV_POSITION;
    // uv値
	float2 uv : TEXCOORD;
};