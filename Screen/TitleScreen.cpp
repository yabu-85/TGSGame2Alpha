#include "TitleScreen.h"
#include "../Engine/SceneManager.h"
#include "../Other/GameManager.h"
#include "../UI/ButtonUI.h"
#include "../Scene/SceneBase.h"
#include "../Screen/SelectScreen.h"
#include "../Screen/SettingScreen.h"
#include "../Screen/ExitScreen.h"

TitleScreen::TitleScreen() : Screen(), hPict_{ -1, -1 }
{
	UIBase* ui = nullptr;

	//Play
	ui = ui->UIInstantiate<ButtonUI>("Play", XMFLOAT2(0.0f, 0.5f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.4f, 0.4f), [this]()
		{
			GameManager::GetScene()->AddScreen(new SelectScreen);
		});
	AddUI(ui);
	ui->SetSelect(true);

	//Setting
	AddUI(ui->UIInstantiate<ButtonUI>("Setting", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.5f, 0.5f), [this]()
		{
			GameManager::GetScene()->AddScreen(new SettingScreen);
		}));

	//–ß‚è
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.5f, 0.5f), [this]()
		{
			GameManager::GetScene()->AddScreen(new ExitScreen);
		}));

}

TitleScreen::~TitleScreen()
{
}

void TitleScreen::Update()
{
	Screen::Update();

}

void TitleScreen::Draw()
{
	Screen::Draw();

}