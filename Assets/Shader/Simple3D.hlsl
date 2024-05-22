//───────────────────────────────────────
 // テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D       g_texture : register(t0);           //テクスチャー
SamplerState    g_sampler : register(s0);           //サンプラー
SamplerState	g_depthSampler : register(s1);   //サンプラー
Texture2D       g_depthTexture : register(t2); //深度テクスチャー

//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global
{
    float4x4 matWVP;        //ワールド・ビュー・プロジェクションの合成行列
    float4x4 matNormal;     //法線の変換行列（回転行列と拡大の逆行列）
    float4x4 matWorld;      //ワールド変換行列
    float4x4 g_mWLP;		//ワールド・”ライトビュー”・プロジェクションの合成 
	float4x4 g_mWLPT;	    //ワールド・”ライトビュー”・プロジェクション・UV 行列の合成 
    float4 diffuseColor;    //マテリアルの色
    float4 speculer;        //スペキュラーカラー
    float4 camPos;          //カメラの座標
    float4 lightPos;        //ライトの座標
    float shininess;        //ハイライトの強さ
    bool isTexture;         //テクスチャがあるか
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
    float4 pos : SV_POSITION;       //位置
    float2 uv : TEXCOORD0;          //UV座標
	float4 normal : TEXCOORD1;		//法線
    float4 color : COLOR;           //色（明るさ）
    float4 lightDir : TEXCOORD5;    //ライトの方向
	float4 eye	  : TEXCOORD2;		//視線
    float4 lightTex : TEXCOORD3;    //ライトから見たトキのテクスチャ点位置
    float4 lightViewPos : TEXCOORD4;//ライトから見た頂点座標
    
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
    outData.uv = uv.xy;

	//法線を回転
    normal.w = 0;
    normal = mul(normal, matNormal);
    outData.normal = normal;

    //視線ベクトル（ハイライトの計算に必要
    float4 worldPos = mul(pos, matWorld);           //ローカル座標にワールド行列をかけてワールド座標へ
    outData.eye = normalize(camPos - worldPos);     //視点から頂点位置を引き算し視線を求めてピクセルシェーダーへ
    
    float4 light = normalize(mul(pos, matWorld) - lightPos);
    outData.lightDir = light;
    outData.color = clamp(dot(normal, light), 0, 1);
    
    outData.lightTex = mul(pos, g_mWLPT);
    outData.lightViewPos = mul(pos, g_mWLP);
    
	//まとめて出力
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    //ライトの向き
    float4 lightDir = normalize(inData.lightDir);

	//法線はピクセルシェーダーに持ってきた時点で補完され長さが変わっている
	//正規化しておかないと面の明るさがおかしくなる
    inData.normal.w = 0;
    inData.normal = normalize(inData.normal);

    float4 shade = saturate(dot(-lightDir, inData.normal));
    shade.a = 1;
    
    float4 diffuse;
	//テクスチャ有無
    if (isTexture == true)
    {
		//テクスチャの色
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
		//マテリアルの色
        diffuse = diffuseColor;
    }

	//環境光（アンビエント）
	//これはMaya側で指定し、グローバル変数で受け取ったものをそのまま
    float4 ambient = float4(0.2f, 0.2f, 0.2f, 1.0f);

	//鏡面反射光（スペキュラー）
    float4 speculer = float4(0.0f, 0.0f, 0.0f, 1.0f); //とりあえずハイライトは無しにしておいて…
    float4 specColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float4 R = reflect(lightDir, inData.normal); //正反射ベクトル
    speculer = pow(saturate(dot(R, inData.eye)), shininess) * specColor; //ハイライトを求める

	//最終的な色
    float4 color = diffuse * shade + diffuse * ambient + speculer;
    
    inData.lightTex /= inData.lightTex.w;
    float TexValue = g_depthTexture.Sample(g_depthSampler, inData.lightTex.xy).r;
    float LightLength = inData.lightViewPos.z / inData.lightViewPos.w;
    
	//ライトから見た頂点のZ値と深度テクスチャの値を比べて、深度テクスチャの方が小さければ影とみなす
    if (TexValue + 0.003f < LightLength)
    {
        //color.r = 2.0f;
        color *= 0.3f;
    }

	//もしアルファ値がすこしでも透明でなければ
    if (diffuse.a == 1.0f) color.a = 1.0f;
    else color.a = diffuse.a;

    return color;
}