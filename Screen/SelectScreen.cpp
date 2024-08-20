#include "SelectScreen.h"
#include "../Engine/SceneManager.h"
#include "../UI/UIBase.h"
#include "../Scene/SceneBase.h"
#include "../Screen/SettingScreen.h"
#include "../Screen/PauseScreen.h"
#include "../Other/GameManager.h"

SelectScreen::SelectScreen() : Screen(), hPict_{ -1, -1 }
{
	UIBase* ui = AddUI("Play", UI_BUTTON, XMFLOAT2(0.0f, 0.5f), XMFLOAT2(0.7f, 0.3f), [this]() {
		SceneManager* pSceneManager = (SceneManager*)GameManager::GetRootObject()->FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_TEST);
		}, XMFLOAT2(1.0f, 0.5f));
	if (ui) ui->SetSelect(true);

	AddUI("Setting", UI_BUTTON, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.7f, 0.3f), [this]() {
		GameManager::GetScene()->AddScreen(new SettingScreen());
	}, XMFLOAT2(1.0f, 0.5f));

	AddUI("Exit", UI_BUTTON, XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.7f, 0.3f), [this]() {
		GameManager::GetScene()->AddScreen(new PauseScreen());
	}, XMFLOAT2(1.0f, 0.5f));

}

SelectScreen::~SelectScreen()
{
}

void SelectScreen::Update()
{
	Screen::Update();

}

void SelectScreen::Draw()
{
	Screen::Draw();

}