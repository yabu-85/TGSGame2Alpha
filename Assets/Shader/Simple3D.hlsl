//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D       g_texture : register(t0);           //テクスチャー
SamplerState    g_sampler : register(s0);           //サンプラー
SamplerState	g_texDepthSampler : register(s1);   //サンプラー
Texture2D       g_texDepth : register(t2);          //深度テクスチャー

//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global
{
    float4x4 matWVP; // ワールド・ビュー・プロジェクションの合成行列
    float4x4 matNormal; //法線を変形させる行列
    float4x4 matWorld;
    float4x4 g_mWLP; //ワールド・”ライトビュー”・プロジェクションの合成 
    float4x4 g_mWLPT; //ワールド・”ライトビュー”・プロジェクション・UV 行列の合成
    float4 diffuseColor;
    float4 speculer;
    float4 camPos;
    float4 lightPos;
    float shininess;
    bool isTexture;
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
    float4 pos : SV_POSITION; //位置
    float2 uv : TEXCOORD; //UV座標
    float4 color : COLOR; //色（明るさ）
    float4 LightTexCoord : TEXCOORD5;
    float4 LighViewPos : TEXCOORD6;
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//ピクセルシェーダーへ渡す情報
    VS_OUT outData;

	//ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
	//スクリーン座標に変換し、ピクセルシェーダーへ
    outData.pos = mul(pos, matWVP);
    outData.uv = uv;

	//法線を回転
    normal = mul(normal, matNormal);
    normal = normalize(normal);

    float4 light = lightPos - mul(pos, matWorld);
    light = normalize(light);
    outData.color = clamp(dot(normal, light), 0, 1);

	//ライトビューを参照するとき、手がかりとなるテクスチャー座標 
    outData.LightTexCoord = mul(pos, g_mWLPT); //この点が、ライトビューであったときの位置がわかる 

	//ライトビューにおける位置(変換後) 
    outData.LighViewPos = mul(pos, matWorld);

	//まとめて出力
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    float4 diffuse;
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (isTexture == true)
    {
		//テクスチャの色
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
		//マテリアルの色
        ambient = diffuseColor * float4(0.2, 0.2, 0.2, 1);
        diffuse = diffuseColor * inData.color;
        diffuse.a = 1;
    }

	//影の処理 
    inData.LightTexCoord /= inData.LightTexCoord.w;
    float TexValue = g_texDepth.Sample(g_texDepthSampler, inData.LightTexCoord).r;
    
    float LightLength = inData.LighViewPos.z / inData.LighViewPos.w;
    if (TexValue + 0.005 < LightLength)  //ライトビューでの長さが短い（ライトビューでは遮蔽物がある） 
    {
        diffuse *= 0.6f;
    }
    else
    {
        diffuse *= 2.0f;
    }
    
    return diffuse + ambient;
	
}
