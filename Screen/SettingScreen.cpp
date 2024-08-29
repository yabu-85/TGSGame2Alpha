#include "SettingScreen.h"
#include "../Engine/SceneManager.h"
#include "../UI/SliderUI.h"
#include "../UI/ButtonUI.h"

SettingScreen::SettingScreen() : Screen()
{
	UIBase* ui = nullptr;

	//Stage1
	ui = ui->UIInstantiate<ButtonUI>("Play", XMFLOAT2(0.0f, 0.5f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.1f, 0.1f), [this]()
		{
		});
	AddUI(ui);
	ui->SetSelect(true);

	//Stage2
	AddUI(ui->UIInstantiate<SliderUI>("Setting", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.1f, 0.1f), [this]()
		{
		}));

	//–ß‚è
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.2f, 0.2f), [this]()
		{
			state_ = ENDDRAW;
		}));

}

SettingScreen::~SettingScreen()
{
}

void SettingScreen::Draw()
{
	Screen::Draw();
	
}