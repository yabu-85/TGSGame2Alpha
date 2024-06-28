#include <Windows.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

//�G���W���֘A�̃f�[�^
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

//ImGui�֘A�̃f�[�^
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "../Player/Player.h"

#pragma comment(lib,"Winmm.lib")

//�萔�錾
const char* WIN_CLASS_NAME = "SampleGame";	//�E�B���h�E�N���X��

//�}�E�X
bool isCursorLimited = FALSE;	// �}�E�X�|�C���^�[�̐���
bool isCursorVisible = TRUE;  // �J�[�\�����\������Ă��邩�ǂ����������t���O

//�t���X�N���[���悤�ɃO���[�o��
bool isFullscreen = false;
LONG_PTR g_windowStyle;
RECT winRect;

//�v���g�^�C�v�錾
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LimitMousePointer(HWND hwnd);
void ReleaseMousePointer();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// �G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ����������
	srand((unsigned)time(NULL));

	//�@���ݒn���A�Z�b�g�t�H���_�ɐݒ�
	SetCurrentDirectory("Assets");

	//�������t�@�C���isetup.ini�j����K�v�ȏ����擾
	int fpsLimit = GetPrivateProfileInt("GAME", "Fps", 60, ".\\setup.ini");				//FPS�i��ʍX�V���x�j
	int isDrawFps = GetPrivateProfileInt("DEBUG", "ViewFps", 0, ".\\setup.ini");		//�L���v�V�����Ɍ��݂�FPS��\�����邩�ǂ���
	int screenWidth = GetPrivateProfileInt("SCREEN", "Width", 800, ".\\setup.ini");			//�X�N���[���̕�
	int screenHeight = GetPrivateProfileInt("SCREEN", "Height", 600, ".\\setup.ini");		//�X�N���[���̍���

#if 1 //_DEBUG
	screenWidth = 700;
	screenHeight = 500;
#endif

	//�E�B���h�E���쐬
	HWND hWnd = InitApp(hInstance, screenWidth, screenHeight, nCmdShow);

	//Direct3D����
	Direct3D::Initialize(hWnd, screenWidth, screenHeight);

	//ImGui��������
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Direct3D::pDevice_, Direct3D::pContext_);

	//�}�E�X����
	if (isCursorLimited) LimitMousePointer(hWnd);

	//�J����������
	Camera::Initialize();

	//���͏����i�L�[�{�[�h�A�}�E�X�A�R���g���[���[�j�̏���
	Input::Initialize(hWnd);

	InputManager::Initialize();
	Audio::Initialize();
	VFXManager::Initialize();
	Light::Initialize();

	//���[�g�I�u�W�F�N�g����
	//���ׂẴQ�[���I�u�W�F�N�g�̐e�ƂȂ�I�u�W�F�N�g
	RootObject* pRootObject = new RootObject;
	pRootObject->Initialize();

	//���b�Z�[�W���[�v�i�����N����̂�҂j
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//���b�Z�[�W����i���������D��j
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//���b�Z�[�W�Ȃ��i�����ŃQ�[���̏����j
		else
		{
			//���Ԍv��
			timeBeginPeriod(1);	//���Ԍv���̐��x���グ��
			static int FPS = 0;								//��ʍX�V�񐔂̃J�E���^
			static DWORD lastFpsResetTime = timeGetTime();	//�Ō�ɃL���v�V������FPS��\������
			static DWORD lastUpdateTime = timeGetTime();	//�Ō�ɉ�ʂ��X�V��������
			DWORD nowTime = timeGetTime();					//���݂̎���

			//�L���v�V�����Ɍ��݂�FPS��\������
			if (isDrawFps)
			{
				//1�b�i1000�~���b�j�o������
				if (nowTime - lastFpsResetTime > 1000)
				{
					//FPS�̒l��\��
					char string[16];
					wsprintf(string, "FPS:%d", FPS);
					SetWindowText(GetActiveWindow(), string);
					FPS = 0;
					lastFpsResetTime = nowTime;
				}
			}

			//�w�肵�����ԁiFPS��60�ɐݒ肵���ꍇ��60����1�b�j�o�߂��Ă�����X�V����
			if ((nowTime - lastUpdateTime) * fpsLimit > 1000.0f)
			{
				//���Ԍv���֘A
				lastUpdateTime = nowTime;	//���݂̎��ԁi�Ō�ɉ�ʂ��X�V�������ԁj���o���Ă���
				FPS++;						//��ʍX�V�񐔂��J�E���g����

				Input::Update();
				VFX::Update();
				DamageUI::Update();
				ScreenManager::Update();
				pRootObject->UpdateSub();

#if 1

				//�P���
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
				//�Q���
				Direct3D::BeginDraw2();
				pRootObject->DrawSub();
				VFX::Draw();
				

				////////////////////////
				//ImGui�̍X�V����
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				StageEditor::DrawStageEditor();
				StageEditor::DrawNodeEditor();

				ImGui::Begin("Hello");//ImGui�̏������J�n
				{
					//ImGui::Text("");		�e�L�X�g
					//ImGui::Button("");	�{�^��
					//ImGui::Separator();	��؂��
					//ImGui::SameLine();	�����ꏊ�ɔz�u
					//ImGui::Spacing();		�w�肵�����X�y�[�X���m��
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
				ImGui::End();//ImGui�̏������I��
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

				Direct3D::EndDraw();

				//������Ƌx�܂���
				Sleep(1);
			}			
			timeEndPeriod(1);	//���Ԍv���̐��x��߂�
		}
	}

	//���낢����
	ScreenManager::AllDeleteScreen();
	VFX::Release();
	Audio::AllRelease();
	Model::AllRelease();
	Image::AllRelease();
	pRootObject->ReleaseSub();
	SAFE_DELETE(pRootObject);

	//ImGui�̊J������
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	Direct3D::Release();

	return 0;
}


//�E�B���h�E�̍쐬
HWND InitApp(HINSTANCE hInstance, int screenWidth, int screenHeight, int nCmdShow)
{
	//�E�B���h�E�N���X�i�݌v�}�j���쐬
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);					//���̍\���̂̃T�C�Y
	wc.hInstance = hInstance;						//�C���X�^���X�n���h��
	wc.lpszClassName = WIN_CLASS_NAME;				//�E�B���h�E�N���X��
	wc.lpfnWndProc = WndProc;						//�E�B���h�E�v���V�[�W��
	wc.style = CS_VREDRAW | CS_HREDRAW;				//�X�^�C���i�f�t�H���g�j
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);	//�A�C�R��
	wc.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);	//�������A�C�R��
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	//�}�E�X�J�[�\��
	wc.lpszMenuName = nullptr;						//���j���[�i�Ȃ��j
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//�w�i�i���j
	RegisterClassEx(&wc);

	//�E�B���h�E�T�C�Y�̌v�Z
	winRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	
	//�^�C�g���o�[�ɕ\��������e
	char caption[64];
	GetPrivateProfileString("SCREEN", "Caption", "***", caption, 64, ".\\setup.ini");

	//�E�B���h�E���쐬
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,					            //�E�B���h�E�N���X��
		caption,						            //�^�C�g���o�[�ɕ\��������e

		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),	//�X�^�C���i�ʏ�̃X�^�C�� & �ő剻�s�� | �T�C�Y�ύX�s�j
		//WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT,					            //�\���ʒu���i���܂����j
		CW_USEDEFAULT,					            //�\���ʒu��i���܂����j
		winRect.right - winRect.left,	            //�E�B���h�E��
		winRect.bottom - winRect.top,	            //�E�B���h�E����
		nullptr,						            //�e�E�C���h�E�i�Ȃ��j
		nullptr,						            //���j���[�i�Ȃ��j
		hInstance,						            //�C���X�^���X
		nullptr							            //�p�����[�^�i�Ȃ��j
	);

	//�E�B���h�E��\��
	ShowWindow(hWnd, nCmdShow);

	//�J�[�\���̕`��ݒ�
	if(!isCursorVisible)
	while (ShowCursor(FALSE) >= 0);

	return hWnd;
}

//�E�B���h�E�v���V�[�W���i�������������ɂ�΂��֐��j
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch (msg)
	{
		//�E�B���h�E�����
	case WM_DESTROY:
		ReleaseMousePointer();
		PostQuitMessage(0);		//�v���O�����I��
		return 0;

		//�}�E�X��������
	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));

		return 0;
	case WM_KEYDOWN:
		if (wParam == 'P')
		{
			if (isCursorVisible)
			{
				// �J�[�\�����\��
				while (ShowCursor(FALSE) >= 0);
				isCursorVisible = false;
			}
			else
			{
				// �J�[�\����\��
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
			int result = MessageBox(hWnd, "�v���O�������I�����܂����H", "�m�F", MB_OKCANCEL | MB_ICONQUESTION);

			if (result == IDOK)
			{
				ReleaseMousePointer();
				PostQuitMessage(0);
				return 0;
			}
		}
	}

	//ImGui�ɏ���n��
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// �}�E�X�|�C���^�[���O�ɏo�Ȃ��悤�ɂ���
void LimitMousePointer(HWND hwnd)
{
	RECT windowRect;
	GetClientRect(hwnd, &windowRect);

	// �E�B���h�E�̋�`�̈���X�N���[�����W�ɕϊ��i����{�^�������Ȃ��悤�ɂ��邽�ߏ���������
	MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&windowRect), 2);
	windowRect.left++;
	windowRect.top++;
	windowRect.right--;
	windowRect.bottom--;

	// �}�E�X�|�C���^�[�𐧌�
	ClipCursor(&windowRect);

	// �E�B���h�E�̒��S���W���v�Z���ăZ�b�g
	POINT windowCenter = { Direct3D::screenWidth_ / 2, Direct3D::screenHeight_ / 2 };
	SetCursorPos(windowRect.left + windowCenter.x, windowRect.top + windowCenter.y);

}

// �}�E�X�|�C���^�[�̐�������������֐�
void ReleaseMousePointer()
{
	ClipCursor(nullptr);
}