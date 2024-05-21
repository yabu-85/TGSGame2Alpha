//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global
{
    float4x4 matWVP;
    float4x4 matNormal;
    float4x4 matWorld;
    float4x4 g_mWLPT;
    float4 diffuseColor;
    float4 speculer;
    float4 camPos;
    float4 lightPos;
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
    float4 pos : SV_POSITION; //位置
    float depth : TEXCOORD1;
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION)
{
    VS_OUT outData;
    outData.pos = mul(pos, matWVP);
    outData.depth = length(lightPos - mul(pos, matWVP)) / 30.0f;
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    return float4(inData.depth, inData.depth, inData.depth, 1.0f);
}
