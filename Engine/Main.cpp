#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

//エンジン関連のデータ
#include "Global.h"
#include "RootObject.h"
#include "Model.h"
#include "Image.h"
#include "Camera.h"
#include "Input.h"
#include "Audio.h"
#include "Light.h"
#include "EffekseeLib/EffekseerVFX.h"
#include "../Other/VFXManager.h"
#include "../Other/GameManager.h"
#include "../Stage/StageEditor.h"

//ImGui関連のデータ
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "../Player/Player.h"

#pragma comment(lib,"Winmm.lib")

//定数宣言
const char* WIN_CLASS_NAME = "SampleGame";	//ウィンドウクラス名

//マウス
bool isCursorLimited = FALSE;	// マウスポインターの制限
bool isCursorVisible = TRUE;  // カーソルが表示されているかどうかを示すフラグ

//フルスクリーンようにグローバル
bool isFullscreen = false;
LONG_PTR g_windowStyle;
RECT winRect;

//プロトタイプ宣言
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LimitMousePointer(HWND hwnd);
void ReleaseMousePointer();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// エントリーポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 乱数初期化
	srand((unsigned)time(NULL));

	//　現在地をアセットフォルダに設定
	SetCurrentDirectory("Assets");

	//初期化ファイル（setup.ini）から必要な情報を取得
	int fpsLimit = GetPrivateProfileInt("GAME", "Fps", 60, ".\\setup.ini");				//FPS（画面更新速度）
	int isDrawFps = GetPrivateProfileInt("DEBUG", "ViewFps", 0, ".\\setup.ini");		//キャプションに現在のFPSを表示するかどうか
	int screenWidth = GetPrivateProfileInt("SCREEN", "Width", 800, ".\\setup.ini");			//スクリーンの幅
	int screenHeight = GetPrivateProfileInt("SCREEN", "Height", 600, ".\\setup.ini");		//スクリーンの高さ

	//ウィンドウを作成
	HWND hWnd = InitApp(hInstance, screenWidth, screenHeight, nCmdShow);

	//Direct3D準備
	Direct3D::Initialize(hWnd, screenWidth, screenHeight);

	//ImGuiを初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Direct3D::pDevice_, Direct3D::pContext_);

	//マウス制限
	if (isCursorLimited) LimitMousePointer(hWnd);

	//カメラを準備
	Camera::Initialize();

	//入力処理（キーボード、マウス、コントローラー）の準備
	Input::Initialize(hWnd);

	//その他初期化
	Audio::Initialize();
	Light::Initialize();

	EFFEKSEERLIB::gEfk = new EFFEKSEERLIB::EffekseerManager;
	EFFEKSEERLIB::gEfk->Initialize(Direct3D::pDevice_, Direct3D::pContext_);
	VFXManager::Initialize();

	//ルートオブジェクト準備
	//すべてのゲームオブジェクトの親となるオブジェクト
	RootObject* pRootObject = new RootObject;
	pRootObject->Initialize();

	GameManager::Initialize();
	GameManager::SetRootObject(pRootObject);

	//メッセージループ（何か起きるのを待つ）
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//メッセージあり（こっちが優先）
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//メッセージなし（ここでゲームの処理）
		else
		{
			//時間計測
			timeBeginPeriod(1);	//時間計測の制度を上げる
			static int FPS = 0;								//画面更新回数のカウンタ
			static DWORD lastFpsResetTime = timeGetTime();	//最後にキャプションにFPSを表示した
			static DWORD lastUpdateTime = timeGetTime();	//最後に画面を更新した時間
			DWORD nowTime = timeGetTime();					//現在の時間

			//キャプションに現在のFPSを表示する
			if (isDrawFps)
			{
				//1秒（1000ミリ秒）経ったら
				if (nowTime - lastFpsResetTime > 1000)
				{
					//FPSの値を表示
					char string[16];
					wsprintf(string, "FPS:%d", FPS);
					SetWindowText(GetActiveWindow(), string);
					FPS = 0;
					lastFpsResetTime = nowTime;
				}
			}

			//指定した時間（FPSを60に設定した場合は60分の1秒）経過していたら更新処理
			double deltaT = nowTime - lastUpdateTime; 
			if (deltaT * fpsLimit > 1000.0f)
			{
				//時間計測関連
				lastUpdateTime = nowTime;	//現在の時間（最後に画面を更新した時間）を覚えておく
				FPS++;						//画面更新回数をカウントする

				Input::Update();
				pRootObject->UpdateSub();

				Light::Update();

				//エフェクト更新
				EFFEKSEERLIB::gEfk->Update(deltaT / 1000.0, 0);
				EFFEKSEERLIB::gEfk->Update(deltaT / 1000.0, 1);
				
				if (GameManager::IsCursorMode()) {
					//カーソルを表示
					while (ShowCursor(TRUE) < 0);
					isCursorVisible = true;

					//リミット制限無し
					isCursorLimited = false;
					ReleaseMousePointer();
				}
				else {
					//カーソルを非表示
					while (ShowCursor(FALSE) >= 0);
					isCursorVisible = false;

					//リミット制限あり
					LimitMousePointer(hWnd);
					isCursorLimited = true;
				}

				GameManager::Update();

				GameManager::Draw();

				//ちょっと休ませる
				Sleep(1);
			}			
			timeEndPeriod(1);	//時間計測の制度を戻す
		}
	}

	//いろいろ解放
	Audio::AllRelease();
	Model::AllRelease();
	Image::AllRelease();
	pRootObject->ReleaseSub();
	SAFE_DELETE(pRootObject);

	//ImGuiの開放処理
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	Direct3D::Release();

	return 0;
}


//ウィンドウの作成
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow)
{
	//ウィンドウクラス（設計図）を作成
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);					//この構造体のサイズ
	wc.hInstance = hInstance;						//インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;				//ウィンドウクラス名
	wc.lpfnWndProc = WndProc;						//ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;				//スタイル（デフォルト）
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);	//アイコン
	wc.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);	//小さいアイコン
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	//マウスカーソル
	wc.lpszMenuName = nullptr;						//メニュー（なし）
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//背景(まぶしいから黒に変更）
	RegisterClassEx(&wc);

	//ウィンドウサイズの計算
	winRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	
	//タイトルバーに表示する内容
	char caption[64];
	GetPrivateProfileString("SCREEN", "Caption", "***", caption, 64, ".\\setup.ini");

	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,					            //ウィンドウクラス名
		caption,						            //タイトルバーに表示する内容

		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),	//スタイル（通常のスタイル & 最大化不可 | サイズ変更不可）
		//WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT,					            //表示位置左（おまかせ）
		CW_USEDEFAULT,					            //表示位置上（おまかせ）
		winRect.right - winRect.left,	            //ウィンドウ幅
		winRect.bottom - winRect.top,	            //ウィンドウ高さ
		nullptr,						            //親ウインドウ（なし）
		nullptr,						            //メニュー（なし）
		hInstance,						            //インスタンス
		nullptr							            //パラメータ（なし）
	);

	//ウィンドウを表示
	ShowWindow(hWnd, nCmdShow);

	//カーソルの描画設定
	if(!isCursorVisible)
	while (ShowCursor(FALSE) >= 0);

	return hWnd;
}

//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch (msg)
	{
		//ウィンドウを閉じた
	case WM_DESTROY:
		ReleaseMousePointer();
		PostQuitMessage(0);		//プログラム終了
		return 0;

		//マウスが動いた
	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));

		return 0;
	case WM_KEYDOWN:
		if (wParam == 'P')
		{
			if (isCursorVisible)
			{
				// カーソルを非表示
				while (ShowCursor(FALSE) >= 0);
				isCursorVisible = false;
			}
			else
			{
				// カーソルを表示
				while (ShowCursor(TRUE) < 0);
				isCursorVisible = true;
			}
		}

		if (wParam == 'L') {
			isCursorLimited = !isCursorLimited;
			
			if (isCursorLimited) LimitMousePointer(hWnd);
			else ReleaseMousePointer();
		}

		if (wParam == VK_ESCAPE)
		{
			while (ShowCursor(TRUE) < 0);
			int result = MessageBox(hWnd, "プログラムを終了しますか？", "確認", MB_OKCANCEL | MB_ICONQUESTION);

			if (result == IDOK)
			{
				ReleaseMousePointer();
				PostQuitMessage(0);
				return 0;
			}
		}
	}

	//ImGuiに情報を渡す
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// マウスポインターが外に出ないようにする
void LimitMousePointer(HWND hwnd)
{
	RECT windowRect;
	GetClientRect(hwnd, &windowRect);

	// ウィンドウの矩形領域をスクリーン座標に変換（閉じるボタン押せないようにするため少し小さく
	MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&windowRect), 2);
	windowRect.left++;
	windowRect.top++;
	windowRect.right--;
	windowRect.bottom--;

	// マウスポインターを制限
	ClipCursor(&windowRect);

	// ウィンドウの中心座標を計算してセット
	POINT windowCenter = { Direct3D::screenWidth_ / 2, Direct3D::screenHeight_ / 2 };
	SetCursorPos(windowRect.left + windowCenter.x, windowRect.top + windowCenter.y);

}

// マウスポインターの制限を解除する関数
void ReleaseMousePointer()
{
	ClipCursor(nullptr);
}