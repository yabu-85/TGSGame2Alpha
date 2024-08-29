#include "TitleScreen.h"
#include "../Engine/SceneManager.h"
#include "../Other/GameManager.h"
#include "../UI/ButtonUI.h"
#include "../Scene/SceneBase.h"
#include "../Screen/SelectScreen.h"

TitleScreen::TitleScreen() : Screen(), hPict_{ -1, -1 }
{
	UIBase* ui = nullptr;
	
	//Stage1
	ui = ui->UIInstantiate<ButtonUI>("A", XMFLOAT2(0.0f, 0.5f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.1f, 0.1f), [this]()
	{
	});
	AddUI(ui);
	ui->SetSelect(true);

	//Stage2
	AddUI(ui->UIInstantiate<ButtonUI>("B", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.1f, 0.1f), [this]()
	{
	}));

	//–ß‚è
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.2f, 0.2f), [this]()
	{
		GameManager::GetScene()->AddScreen(new SelectScreen);
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