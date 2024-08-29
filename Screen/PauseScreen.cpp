#include "PauseScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../Other/GameManager.h"
#include "../Engine/Camera.h"
#include "../Scene/SceneBase.h"
#include "../Screen/SettingScreen.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"

PauseScreen::PauseScreen() : Screen(), hPict_{ -1, -1 }
{
	const char* fileName[] = { "Image/Pause.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	Transform t;
	t.position_ = XMFLOAT3(0.0f, 0.5f, 0.0f);
	Image::SetTransform(hPict_[0], t);
	Image::SetFullScreenTransform(hPict_[1]);
	Image::SetAlpha(hPict_[0], 255);
	Image::SetAlpha(hPict_[1], 100);

	//ƒ{ƒ^ƒ“
	UIBase* ui = nullptr;
	
	//PauseI—¹
	ui = ui->UIInstantiate<ButtonUI>("Ok", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.5f, 0.4f), XMFLOAT2(0.4f, 0.4f), [this]()
		{
			state_ = ENDDRAW;
		});
	AddUI(ui);
	ui->SetSelect(true);

	//Setting
	AddUI(ui->UIInstantiate<ButtonUI>("Setting", XMFLOAT2(0.0f, -0.35f), XMFLOAT2(0.5f, 0.4f), XMFLOAT2(0.5f, 0.5f), [this]()
		{
			GameManager::GetScene()->AddScreen(new SettingScreen);
		}));

	//Title
	AddUI(ui->UIInstantiate<ButtonUI>("ReturnToTitle", XMFLOAT2(0.0f, -0.7f), XMFLOAT2(0.5f, 0.4f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			SceneManager* pSceneManager = (SceneManager*)GameManager::GetRootObject()->FindObject("SceneManager");
			pSceneManager->ChangeScene(SCENE_ID_TITLE);
		}));

}

PauseScreen::~PauseScreen()
{
}

void PauseScreen::Draw()
{
	//‰æ–Ê•ªŠ„–³‚µ‚É
	Camera::SetOneProjectionMatrix();
	Direct3D::SetViewOne();
	Direct3D::SetViewPort(0);

	Image::Draw(hPict_[1]);
	Image::Draw(hPict_[0]); 
	
	Screen::Draw();

	//‰æ–Ê•ªŠ„2‚É–ß‚·
	if (!GameManager::IsOnePlayer()) {
		Camera::SetTwoProjectionMatrix();
		Direct3D::SetViewTwo();
		Direct3D::SetViewPort(0);
	}

}