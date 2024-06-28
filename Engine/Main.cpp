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
#include "VFX.h"
#include "Light.h"
#include "../Stage/StageEditor.h"
#include "../Other/InputManager.h"
#include "../Other/VFXManager.h"
#include "../UI/ScreenManager.h"
#include "../UI/DamageUI.h"

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

#if 1 //_DEBUG
	screenWidth = 700;
	screenHeight = 500;
#endif

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

	InputManager::Initialize();
	Audio::Initialize();
	VFXManager::Initialize();
	Light::Initialize();

	//ルートオブジェクト準備
	//すべてのゲームオブジェクトの親となるオブジェクト
	RootObject* pRootObject = new RootObject;
	pRootObject->Initialize();

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
			if ((nowTime - lastUpdateTime) * fpsLimit > 1000.0f)
			{
				//時間計測関連
				lastUpdateTime = nowTime;	//現在の時間（最後に画面を更新した時間）を覚えておく
				FPS++;						//画面更新回数をカウントする

				Input::Update();
				VFX::Update();
				DamageUI::Update();
				ScreenManager::Update();
				pRootObject->UpdateSub();

#if 1

				//１回目
				XMFLOAT3 pos = Camera::GetPosition(0);
				XMFLOAT3 tar = Camera::GetTarget(0);
				Camera::SetPosition(XMFLOAT3(Light::GetPosition(0).x, Light::GetPosition(0).y, Light::GetPosition(0).z), 0);
				Camera::SetTarget(XMFLOAT3(50, 0, 50), 0);
				Camera::Update(0);
				Direct3D::lightViewMatrix = Camera::GetViewMatrix();

				Direct3D::BeginDraw();
				pRootObject->DrawSub();
				Direct3D::EndDraw();
				Camera::SetPosition(pos, 0);
				Camera::SetTarget(tar, 0);
				Camera::Update(0);
				///////////////////////////
				//２回目
				Direct3D::BeginDraw2();
				pRootObject->DrawSub();
				VFX::Draw();
				

				////////////////////////
				//ImGuiの更新処理
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				StageEditor::DrawStageEditor();
				StageEditor::DrawNodeEditor();

				ImGui::Begin("Hello");//ImGuiの処理を開始
				{
					//ImGui::Text("");		テキスト
					//ImGui::Button("");	ボタン
					//ImGui::Separator();	区切り線
					//ImGui::SameLine();	同じ場所に配置
					//ImGui::Spacing();		指定した分スペースを確保
					XMFLOAT4 position = Light::GetPosition(0);
					XMFLOAT4 target = Light::GetTarget(0);
					ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
					ImGui::Text("Target: (%.2f, %.2f, %.2f)", target.x, target.y, target.z);
					ImGui::Separator();
					ImGui::Text("Player: (%.2f, %.2f, %.2f)", Direct3D::PlayerPosition.x, Direct3D::PlayerPosition.y, Direct3D::PlayerPosition.z);
					ImGui::Text("Enemy : (%.2f, %.2f, %.2f)", Direct3D::EnemyPosition.x, Direct3D::EnemyPosition.y, Direct3D::EnemyPosition.z);
					ImGui::Separator();
					ImGui::SliderFloat("Player Speed", &Direct3D::playerSpeed, 0.0f, 1.0f);

					if (Direct3D::playerFaly) ImGui::Text("Player Faly   : true");
					else ImGui::Text("Player Faly  : false");

					if (Direct3D::playerClimb) ImGui::Text("Player Climb : true");
					else ImGui::Text("Player Climb : false");
				}
				ImGui::End();//ImGuiの処理を終了
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

				Direct3D::EndDraw();

				//ちょっと休ませる
				Sleep(1);
			}			
			timeEndPeriod(1);	//時間計測の制度を戻す
		}
	}

	//いろいろ解放
	ScreenManager::AllDeleteScreen();
	VFX::Release();
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
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//背景（白）
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