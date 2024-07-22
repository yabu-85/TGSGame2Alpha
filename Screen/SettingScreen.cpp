#include "SettingScreen.h"
#include "../Engine/SceneManager.h"
#include "../UI/UIBase.h"

SettingScreen::SettingScreen() : Screen()
{
	UIBase* ui = AddUI("B", UI_SLIDER, XMFLOAT2(-0.3f, 0.5f), XMFLOAT2(0.3f, 1.0f), [this]() { 
	}, XMFLOAT2(0.6f, 0.6f));
	if (ui) ui->SetSelect(true);

}

SettingScreen::~SettingScreen()
{
}

void SettingScreen::Draw()
{
	Screen::Draw();
	
}