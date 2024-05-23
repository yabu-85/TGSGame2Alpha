//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D g_texture : register(t0);	// テクスチャー
SamplerState g_sampler : register(s0); // テクスチャーサンプラー

//───────────────────────────────────────
 // コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global
{
    float4x4 matWVP; //ワールド・ビュー・プロジェクションの合成行列
    float4x4 matNormal; //法線の変換行列（回転行列と拡大の逆行列）
    float4x4 matWorld; //ワールド変換行列
    float4x4 g_mWLP; //ワールド・ライトビュー・プロジェクションの合成 
    float4x4 g_mWLPT; //ワールド・ライトビュー・プロジェクション・UV 行列の合成 
    float4 diffuseColor; //マテリアルの色
    float4 speculer; //スペキュラーカラー
    float4 camPos; //カメラの座標
    float4 lightPos; //ライトの座標
    float shininess; //ハイライトの強さ
    bool isTexture; //テクスチャがあるか
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUTPUT
{
	float4 pos	: SV_POSITION;	// 位置
	float2 uv	: TEXCOORD;		// UV座標
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUTPUT VS(float4 pos : POSITION, float4 uv : TEXCOORD)
{
	VS_OUTPUT output;
    output.pos = mul(pos, matWorld);
    output.uv = mul(uv, matNormal);
	return output;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUTPUT input) : SV_Target
{
    return diffuseColor * g_texture.Sample(g_sampler, input.uv);
}