#include <d3dcompiler.h>
#include "Direct3D.h"
#include "Global.h"
#include "Transform.h"
#include "Camera.h"
#include "Sprite.h"

//画面の描画に関する処理
namespace Direct3D
{
	//【スワップチェーン】
	//画用紙を2枚用紙しておき、片方を画面に映している間にもう一方に描画。
	//1フレーム分の絵が出来上がったら画用紙を交換。これにより画面がちらつかない。
	//その辺を司るのがスワップチェーン
	IDXGISwapChain*         pSwapChain_ = nullptr;

	//【レンダーターゲットビュー】
	//描画したいものと、描画先（上でいう画用紙）の橋渡しをするもの
	ID3D11RenderTargetView* pRenderTargetView_ = nullptr;

	//【デプスステンシル】
	//Zバッファ法を用いて、3D物体の前後関係を正しく表示するためのもの
	ID3D11Texture2D*		pDepthStencil;

	//【デプスステンシルビュー】
	//デプスステンシルの情報をシェーダーに渡すためのもの
	ID3D11DepthStencilView* pDepthStencilView;
	ID3D11DepthStencilState* pDepthStencilState[BLEND_MAX];

	//【ブレンドステート】
	//半透明のものをどのように表現するか
	ID3D11BlendState*	pBlendState[BLEND_MAX];

	//extern宣言した変数の初期化
	ID3D11Device*           pDevice_ = nullptr;
	ID3D11DeviceContext*    pContext_ = nullptr;
	SHADER_BUNDLE			shaderBundle[SHADER_MAX] = { 0 };

	bool					isDrawCollision_ = true;	//コリジョンを表示するか
	int						screenWidth_ = 0;
	int						screenHeight_ = 0;

	// シェーダーバンドルの保存用の変数
	SHADER_TYPE currentType;

	//シャドウマップ用-------------------------------------
	D3D11_VIEWPORT vp1[2];
	D3D11_VIEWPORT vp2;

	ID3D11RenderTargetView* pDepthTargetView_;
	ID3D11Texture2D* pRenderTexture;
	ID3D11ShaderResourceView* pDepthSRV_;
	ID3D11SamplerState* pDepthSampler_;

	XMMATRIX lightViewMatrix;
	XMMATRIX clipToUVMatrix;

	//ちょいとここに置いとく
	XMFLOAT3	PlayerPosition;
	XMFLOAT3	EnemyPosition;
	float playerSpeed = 0.0f;
	float playerCamX = 0.0f;
	float playerCamY = 0.0f;
	bool		playerClimb;
	bool		playerFaly;

	//-------------------------------------------------------

	//初期化処理
	HRESULT Direct3D::Initialize(HWND hWnd, int screenWidth, int screenHeight)
	{
		//コリジョン表示するか
		isDrawCollision_ = GetPrivateProfileInt("DEBUG", "ViewCollider", 0, ".\\setup.ini") != 0;

		screenWidth_ = screenWidth;
		screenHeight_ = screenHeight;

		///////////////////////////いろいろ準備するための設定///////////////////////////////

		//スパップチェインの設定
		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(scDesc));					//とりあえず全部0

		//描画先のフォーマット
		scDesc.BufferDesc.Width = screenWidth;					//画面幅
		scDesc.BufferDesc.Height = screenHeight;				//画面高さ
		scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 何色使えるか

		//FPS（1/60秒に1回）
		scDesc.BufferDesc.RefreshRate.Numerator = 60;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;

		//その他
		scDesc.Windowed = TRUE;									//ウィンドウモードかフルスクリーンか
		scDesc.OutputWindow = hWnd;								//ウィンドウハンドル
		scDesc.BufferCount = 1;									//裏画面の枚数
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//画面に描画するために使う
		scDesc.SampleDesc.Count = 1;							//MSAA（アンチエイリアス）の設定
		scDesc.SampleDesc.Quality = 0;							//　〃

		///////////////////////////上記設定をもとにデバイス、コンテキスト、スワップチェインを作成///////////////////////////////
		D3D_FEATURE_LEVEL level;
		HRESULT  hr;
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,					// どのビデオアダプタを使用するか？既定ならばnullptrで
			D3D_DRIVER_TYPE_HARDWARE,	// ドライバのタイプを渡す。これ以外は基本的にソフトウェア実装で、どうしてもという時やデバグ用に用いるべし.
			nullptr,					// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定した際に、その処理を行うDLLのハンドルを渡す。それ以外を指定している際には必ずnullptrを渡す.
			0,							// 何らかのフラグを指定する。（デバッグ時はD3D11_CREATE_DEVICE_DEBUG？）
			nullptr,					// デバイス、コンテキストのレベルを設定。nullptrにしとけば可能な限り高いレベルにしてくれる
			0,							// 上の引数でレベルを何個指定したか
			D3D11_SDK_VERSION,			// SDKのバージョン。必ずこの値
			&scDesc,					// 上でいろいろ設定した構造体
			&pSwapChain_,				// 無事完成したSwapChainのアドレスが返ってくる
			&pDevice_,					// 無事完成したDeviceアドレスが返ってくる
			&level,						// 無事完成したDevice、Contextのレベルが返ってくる
			&pContext_);				// 無事完成したContextのアドレスが返ってくる

		//失敗したら終了
		if (FAILED(hr))	return hr;


		///////////////////////////レンダーターゲットビュー作成///////////////////////////////
		//スワップチェーンからバックバッファを取得（バックバッファ ＝ 裏画面 ＝ 描画先）
		ID3D11Texture2D* pBackBuffer;
		hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr))	return hr;

		//レンダーターゲットビューを作成
		hr = pDevice_->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView_);
		if (FAILED(hr))	return hr;

		//一時的にバックバッファを取得しただけなので解放
		pBackBuffer->Release();

		/////////////////////////////////////////////////////////////////////////////////////////////

		//Viewの設定、一人用で
		SetViewOne();

		//各パターンのシェーダーセット準備
		InitShaderBundle();

		//深度ステンシルビューの作成
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = screenWidth;
		descDepth.Height = screenHeight;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		pDevice_->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
		pDevice_->CreateDepthStencilView(pDepthStencil, NULL, &pDepthStencilView);


		//深度テストを行う深度ステンシルステートの作成
		{
			//デフォルト
			D3D11_DEPTH_STENCIL_DESC desc = {};
			desc.DepthEnable = true;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.StencilEnable = true;
			pDevice_->CreateDepthStencilState(&desc, &pDepthStencilState[BLEND_DEFAULT]);
			pContext_->OMSetDepthStencilState(pDepthStencilState[BLEND_DEFAULT], 0);

			//加算合成用（書き込みなし）
			desc.StencilEnable = false;
			pDevice_->CreateDepthStencilState(&desc, &pDepthStencilState[BLEND_ADD]);
		}


		//ブレンドステート
		{
			//通常
			D3D11_BLEND_DESC BlendDesc;
			ZeroMemory(&BlendDesc, sizeof(BlendDesc));
			BlendDesc.AlphaToCoverageEnable = FALSE;
			BlendDesc.IndependentBlendEnable = FALSE;
			BlendDesc.RenderTarget[0].BlendEnable = TRUE;
			BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			pDevice_->CreateBlendState(&BlendDesc, &pBlendState[BLEND_DEFAULT]);
			float blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
			pContext_->OMSetBlendState(pBlendState[BLEND_DEFAULT], blendFactor, 0xffffffff);

			//加算合成（重なるほど光って見える効果）
			BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			pDevice_->CreateBlendState(&BlendDesc, &pBlendState[BLEND_ADD]);
		}

		//データを画面に描画するための一通りの設定（パイプライン）
		pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// データの入力種類を指定



		//シャドウマップで追加ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
		//新しいレンダーターゲット作成
		D3D11_TEXTURE2D_DESC texdec;
		texdec.Width = screenWidth;
		texdec.Height = screenHeight;
		texdec.MipLevels = 1;
		texdec.ArraySize = 1;
		texdec.MiscFlags = 0;
		texdec.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texdec.SampleDesc.Count = 1;
		texdec.SampleDesc.Quality = 0;
		texdec.Usage = D3D11_USAGE_DEFAULT;
		texdec.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texdec.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pDevice_->CreateTexture2D(&texdec, nullptr, &pRenderTexture);

		//新しいレンダーターゲットビュー作成
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		renderTargetViewDesc.Format = texdec.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = pDevice_->CreateRenderTargetView(pRenderTexture, &renderTargetViewDesc, &pDepthTargetView_);

		//シェーダーリソースビュー
		D3D11_SHADER_RESOURCE_VIEW_DESC srv;
		ZeroMemory(&srv, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = 1;
		hr = pDevice_->CreateShaderResourceView(pRenderTexture, &srv, &pDepthSRV_);

		// テクスチャー用サンプラー作成
		D3D11_SAMPLER_DESC  SamDesc;
		ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
		SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		SamDesc.BorderColor[0] = 1.0f; //R
		SamDesc.BorderColor[1] = 1.0f; //G
		SamDesc.BorderColor[2] = 1.0f; //B
		SamDesc.BorderColor[3] = 1.0f; //A
		Direct3D::pDevice_->CreateSamplerState(&SamDesc, &pDepthSampler_);

		//ビューポートの設定
		//レンダリング結果を表示する範囲
		vp2.Width = (float)screenWidth;	//幅
		vp2.Height = (float)screenHeight;//高さ
		vp2.MinDepth = 0.0f;	//手前
		vp2.MaxDepth = 1.0f;	//奥
		vp2.TopLeftX = 0;	//左
		vp2.TopLeftY = 0;	//上

		XMFLOAT4X4 clipToUV;
		ZeroMemory(&clipToUV, sizeof(XMFLOAT4X4));
		clipToUV._11 = 0.5f;
		clipToUV._22 = -0.5f;
		clipToUV._33 = 1.0f;
		clipToUV._41 = 0.5f;
		clipToUV._42 = 0.5f;
		clipToUV._44 = 1.0f;
		clipToUVMatrix = XMLoadFloat4x4(&clipToUV);

		//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

		return S_OK;
	}

	//シェーダー関連で必要なセット準備
	void InitShaderBundle()
	{
		DWORD vectorSize = sizeof(XMFLOAT3);

		//3D
		{
			// 頂点シェーダの作成（コンパイル）
			ID3DBlob *pCompileVS = NULL;
			D3DCompileFromFile(L"Shader/Simple3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
			pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle[SHADER_3D].pVertexShader);


			// ピクセルシェーダの作成（コンパイル）
			ID3DBlob *pCompilePS = NULL;
			D3DCompileFromFile(L"Shader/Simple3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
			pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle[SHADER_3D].pPixelShader);


			// 頂点レイアウトの作成（1頂点の情報が何のデータをどんな順番で持っているか）
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vectorSize * 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//頂点位置
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vectorSize * 1, D3D11_INPUT_PER_VERTEX_DATA, 0 },	//法線
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, vectorSize * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 },	//テクスチャ（UV）座標
			};
			pDevice_->CreateInputLayout(layout, 3, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle[SHADER_3D].pVertexLayout);


			//シェーダーが無事作成できたので、コンパイルしたやつはいらない
			pCompileVS->Release();
			pCompilePS->Release();

			//ラスタライザ作成
			D3D11_RASTERIZER_DESC rdc = {};
			rdc.CullMode = D3D11_CULL_BACK;
			rdc.FillMode = D3D11_FILL_SOLID;
			rdc.FrontCounterClockwise = TRUE;	//反時計回りは表面じゃない
			pDevice_->CreateRasterizerState(&rdc, &shaderBundle[SHADER_3D].pRasterizerState);
		}


		//2D
		{
			// 頂点シェーダの作成（コンパイル）
			ID3DBlob *pCompileVS = NULL;
			D3DCompileFromFile(L"Shader/Simple2D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
			pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle[SHADER_2D].pVertexShader);


			// ピクセルシェーダの作成（コンパイル）
			ID3DBlob *pCompilePS = NULL;
			D3DCompileFromFile(L"Shader/Simple2D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
			pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle[SHADER_2D].pPixelShader);


			// 頂点レイアウトの作成（1頂点の情報が何のデータをどんな順番で持っているか）
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vectorSize * 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, vectorSize * 1, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			pDevice_->CreateInputLayout(layout, 2, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle[SHADER_2D].pVertexLayout);


			//シェーダーが無事作成できたので、コンパイルしたやつはいらない
			pCompileVS->Release();
			pCompilePS->Release();

			//ラスタライザ作成
			D3D11_RASTERIZER_DESC rdc = {};
			rdc.CullMode = D3D11_CULL_BACK;
			rdc.FillMode = D3D11_FILL_SOLID;
			rdc.FrontCounterClockwise = TRUE;
			pDevice_->CreateRasterizerState(&rdc, &shaderBundle[SHADER_2D].pRasterizerState);
		}

		//DEBUG用
		{
			// 頂点シェーダの作成（コンパイル）
			ID3DBlob *pCompileVS = NULL;
			D3DCompileFromFile(L"Shader/Debug3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
			pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle[SHADER_UNLIT].pVertexShader);


			// ピクセルシェーダの作成（コンパイル）
			ID3DBlob *pCompilePS = NULL;
			D3DCompileFromFile(L"Shader/Debug3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
			pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle[SHADER_UNLIT].pPixelShader);


			// 頂点レイアウトの作成（1頂点の情報が何のデータをどんな順番で持っているか）
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			pDevice_->CreateInputLayout(layout, 1, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle[SHADER_UNLIT].pVertexLayout);


			//シェーダーが無事作成できたので、コンパイルしたやつはいらない
			pCompileVS->Release();
			pCompilePS->Release();

			//ラスタライザ作成
			D3D11_RASTERIZER_DESC rdc = {};
			rdc.CullMode = D3D11_CULL_NONE;
			rdc.FillMode = D3D11_FILL_WIREFRAME;
			rdc.FrontCounterClockwise = TRUE;
			pDevice_->CreateRasterizerState(&rdc, &shaderBundle[SHADER_UNLIT].pRasterizerState);
		}

		//BillBoard
		{
			// 頂点シェーダの作成（コンパイル）
			ID3DBlob* pCompileVS = NULL;
			D3DCompileFromFile(L"Shader/BillBoard.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
			pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle[SHADER_BILLBOARD].pVertexShader);


			// ピクセルシェーダの作成（コンパイル）
			ID3DBlob* pCompilePS = NULL;
			D3DCompileFromFile(L"Shader/BillBoard.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
			pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle[SHADER_BILLBOARD].pPixelShader);


			// 頂点レイアウトの作成（1頂点の情報が何のデータをどんな順番で持っているか）
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vectorSize * 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, vectorSize * 1, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			pDevice_->CreateInputLayout(layout, 2, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle[SHADER_BILLBOARD].pVertexLayout);


			//シェーダーが無事作成できたので、コンパイルしたやつはいらない
			pCompileVS->Release();
			pCompilePS->Release();

			//ラスタライザ作成
			D3D11_RASTERIZER_DESC rdc = {};
			rdc.CullMode = D3D11_CULL_NONE;
			rdc.FillMode = D3D11_FILL_SOLID;
			rdc.FrontCounterClockwise = TRUE;
			pDevice_->CreateRasterizerState(&rdc, &shaderBundle[SHADER_BILLBOARD].pRasterizerState);
		}

		//ShadowMap
		{
			// 頂点シェーダの作成（コンパイル）
			ID3DBlob* pCompileVS = NULL;
			D3DCompileFromFile(L"Shader/ShadowMap.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
			pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle[SHADER_SHADOWMAP].pVertexShader);


			// ピクセルシェーダの作成（コンパイル）
			ID3DBlob* pCompilePS = NULL;
			D3DCompileFromFile(L"Shader/ShadowMap.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
			pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle[SHADER_SHADOWMAP].pPixelShader);


			// 頂点レイアウトの作成（1頂点の情報が何のデータをどんな順番で持っているか）
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, vectorSize * 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			pDevice_->CreateInputLayout(layout, 1, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle[SHADER_SHADOWMAP].pVertexLayout);

			//シェーダーが無事作成できたので、コンパイルしたやつはいらない
			pCompileVS->Release();
			pCompilePS->Release();

			//ラスタライザ作成
			D3D11_RASTERIZER_DESC rdc = {};
			rdc.CullMode = D3D11_CULL_BACK;
			rdc.FillMode = D3D11_FILL_SOLID;
			rdc.FrontCounterClockwise = TRUE;
			pDevice_->CreateRasterizerState(&rdc, &shaderBundle[SHADER_SHADOWMAP].pRasterizerState);
		}
	}

	//今から描画するShaderBundleを設定
	void SetShader(SHADER_TYPE type)
	{
		currentType = type;
		pContext_->RSSetState(shaderBundle[type].pRasterizerState);
		pContext_->VSSetShader(shaderBundle[type].pVertexShader, NULL, 0);                         // 頂点シェーダをセット
		pContext_->PSSetShader(shaderBundle[type].pPixelShader, NULL, 0);                          // ピクセルシェーダをセット
		pContext_->IASetInputLayout(shaderBundle[type].pVertexLayout);
	}
	SHADER_TYPE GetCurrentShader() { return currentType; }

	//ブレンドモードの変更
	void SetBlendMode(BLEND_MODE blendMode)
	{
		//加算合成
		float blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
		pContext_->OMSetBlendState(pBlendState[blendMode], blendFactor, 0xffffffff);
		
		//Zバッファへの書き込み
		pContext_->OMSetDepthStencilState(pDepthStencilState[blendMode], 0);
	}

	//描画開始
	void BeginDraw()
	{
		//ShadowMapで追加-------------------------------
		pContext_->OMSetRenderTargets(1, &pDepthTargetView_, pDepthStencilView);            // 描画先を設定

		pContext_->RSSetViewports(1, &vp2);
		//---------------------------------------

		//背景の色
		float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//R,G,B,A

		//画面をクリア
		pContext_->ClearRenderTargetView(pDepthTargetView_, clearColor);

		//深度バッファクリア
		pContext_->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	

		SetShader(SHADER_SHADOWMAP);
	}

	void BeginDraw2()
	{
		//描画先を設定
		pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView);

		//画面をクリアと深度バッファクリア
		float clearColor[4] = { 0.1f, 0.2f, 0.2f, 1.0f };//R,G,B,A
		pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);
		pContext_->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		SetShader(SHADER_3D);
	}

	void SetViewPort(int i)
	{
		pContext_->RSSetViewports(1, &vp1[i]);
	}

	void SetViewOne()
	{
		//ビューポートの設定（二人用の設定に
		//レンダリング結果を表示する範囲
		vp1[0].Width = (float)screenWidth_;	//幅
		vp1[0].Height = (float)screenHeight_;//高さ
		vp1[0].MinDepth = 0.0f;	//手前
		vp1[0].MaxDepth = 1.0f;	//奥
		vp1[0].TopLeftX = 0;	//左
		vp1[0].TopLeftY = 0;	//上

		vp1[1].Width = (float)screenWidth_;	//幅
		vp1[1].Height = (float)screenHeight_;//高さ
		vp1[1].MinDepth = 0.0f;	//手前
		vp1[1].MaxDepth = 1.0f;	//奥
		vp1[1].TopLeftX = 0;	//左
		vp1[1].TopLeftY = 0;	//上
	}

	void SetViewTwo()
	{
		//ビューポートの設定（二人用の設定に
		//レンダリング結果を表示する範囲
		vp1[0].Width = (float)screenWidth_ / 2;	//幅
		vp1[0].Height = (float)screenHeight_;//高さ
		vp1[0].MinDepth = 0.0f;	//手前
		vp1[0].MaxDepth = 1.0f;	//奥
		vp1[0].TopLeftX = 0.0f;	//左
		vp1[0].TopLeftY = 0;	//上

		vp1[1].Width = (float)screenWidth_ / 2;	//幅
		vp1[1].Height = (float)screenHeight_;//高さ
		vp1[1].MinDepth = 0.0f;	//手前
		vp1[1].MaxDepth = 1.0f;	//奥
		vp1[1].TopLeftX = (float)screenWidth_ / 2;	//左
		vp1[1].TopLeftY = 0;	//上
	}

	//描画終了
	void EndDraw()
	{
		//スワップ（バックバッファを表に表示する）
		pSwapChain_->Present(0, 0);

	}


	//開放処理
	void Release()
	{
		SAFE_RELEASE(pDepthStencil);
		SAFE_RELEASE(pDepthStencilView);
		SAFE_RELEASE(pRenderTargetView_);
		SAFE_RELEASE(pDepthTargetView_);
		SAFE_RELEASE(pSwapChain_);

		for (int i = 0; i < BLEND_MAX; i++)
		{
			SAFE_RELEASE(pBlendState[i]);
			SAFE_RELEASE(pDepthStencilState[i]);
		}

		for (int i = 0; i < SHADER_MAX; i++)
		{
			SAFE_RELEASE(shaderBundle[i].pRasterizerState);
			SAFE_RELEASE(shaderBundle[i].pVertexLayout);
			SAFE_RELEASE(shaderBundle[i].pVertexShader);
			SAFE_RELEASE(shaderBundle[i].pPixelShader);
		}

		if (pContext_)
		{
			pContext_->ClearState();
		}

		SAFE_RELEASE(pRenderTargetView_);
		SAFE_RELEASE(pDepthTargetView_);
		SAFE_RELEASE(pSwapChain_);
		SAFE_RELEASE(pContext_);
		SAFE_RELEASE(pDevice_);
	}


	//三角形と線分の衝突判定（衝突判定に使用）
	//https://pheema.hatenablog.jp/entry/ray-triangle-intersection
	bool Intersect(XMFLOAT3 & start, XMFLOAT3 & direction, XMFLOAT3 & v0, XMFLOAT3 & v1, XMFLOAT3 & v2, float* distance, XMVECTOR* normal)
	{
		// 微小な定数([M?ller97] での値)
		constexpr float kEpsilon = 1e-6f;

		//三角形の２辺
		XMVECTOR edge1 = XMVectorSet(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z, 0.0f);
		XMVECTOR edge2 = XMVectorSet(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z, 0.0f);

		//平面の法線ベクトルを求める
		*normal = XMVector3Cross(edge1, edge2);
		*normal /= XMVectorGetX(XMVector3Length(*normal));

		XMVECTOR alpha = XMVector3Cross(XMLoadFloat3(&direction), edge2);
		float det = XMVector3Dot(edge1, alpha).m128_f32[0];

		// 三角形に対して、レイが平行に入射するような場合 det = 0 となる。
		// det が小さすぎると 1/det が大きくなりすぎて数値的に不安定になるので
		// det ? 0 の場合は交差しないこととする。
		if (-kEpsilon < det && det < kEpsilon) 
		{
			return false;
		}

		float invDet = 1.0f / det;
		XMFLOAT3 r = XMFLOAT3(start.x - v0.x, start.y - v0.y, start.z - v0.z);

		// u が 0 <= u <= 1 を満たしているかを調べる。
		float u = XMVector3Dot(alpha, XMLoadFloat3(&r)).m128_f32[0] * invDet;
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		XMVECTOR beta = XMVector3Cross(XMLoadFloat3(&r), edge1);

		// v が 0 <= v <= 1 かつ u + v <= 1 を満たすことを調べる。
		// すなわち、v が 0 <= v <= 1 - u をみたしているかを調べればOK。
		float v = XMVector3Dot(XMLoadFloat3(&direction), beta).m128_f32[0] * invDet;
		if (v < 0.0f || u + v > 1.0f) 
		{
			return false;
		}

		// t が 0 <= t を満たすことを調べる。
		float t = XMVector3Dot(edge2, beta).m128_f32[0] * invDet;
		if (t < 0.0f) 
		{
			return false;
		}

		*distance = t;
		return true;
	}

	bool Intersect(XMFLOAT3& start, XMFLOAT3& direction, XMVECTOR& v0, XMVECTOR& v1, XMVECTOR& v2, float* distance, XMVECTOR* normal)
	{
		// 微小な定数([M?ller97] での値)
		constexpr float kEpsilon = 1e-6f;

		XMVECTOR edge1 = v1 - v0;
		XMVECTOR edge2 = v2 - v0;

		//平面の法線ベクトルを求める
		//*normal = XMVector3Cross(edge1, edge2);
		//*normal /= XMVectorGetX(XMVector3Length(*normal));

		XMVECTOR alpha = XMVector3Cross(XMLoadFloat3(&direction), edge2);
		float det = XMVector3Dot(edge1, alpha).m128_f32[0];

		// 三角形に対して、レイが平行に入射するような場合 det = 0 となる。
		// det が小さすぎると 1/det が大きくなりすぎて数値的に不安定になるので
		// det ? 0 の場合は交差しないこととする。
		if (-kEpsilon < det && det < kEpsilon)
		{
			return false;
		}

		float invDet = 1.0f / det;
		XMVECTOR r = XMLoadFloat3(&start) - v0;

		// u が 0 <= u <= 1 を満たしているかを調べる。
		float u = XMVector3Dot(alpha, r).m128_f32[0] * invDet;
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		XMVECTOR beta = XMVector3Cross(r, edge1);

		// v が 0 <= v <= 1 かつ u + v <= 1 を満たすことを調べる。
		// すなわち、v が 0 <= v <= 1 - u をみたしているかを調べればOK。
		float v = XMVector3Dot(XMLoadFloat3(&direction), beta).m128_f32[0] * invDet;
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		// t が 0 <= t を満たすことを調べる。
		float t = XMVector3Dot(edge2, beta).m128_f32[0] * invDet;
		if (t < 0.0f)
		{
			return false;
		}

		*distance = t;
		return true;
	}

	//Zバッファへの書き込みON/OFF
	void SetDepthBafferWriteEnable(bool isWrite)
	{
		//ON
		if (isWrite)
		{
			//Zバッファ（デプスステンシルを指定する）
			pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView);
		}

		//OFF
		else
		{
			pContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);
		}
	}

}
