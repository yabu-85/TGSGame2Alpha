#include "SettingScreen.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "../Other/GameManager.h"
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
	//‰æ–Ê•ªŠ„–³‚µ‚É
	Camera::SetOneProjectionMatrix();
	Direct3D::SetViewOne();
	Direct3D::SetViewPort(0);

	Screen::Draw();
	
	//‰æ–Ê•ªŠ„2‚É–ß‚·
	if (!GameManager::IsOnePlayer()) {
		Camera::SetTwoProjectionMatrix();
		Direct3D::SetViewTwo();
		Direct3D::SetViewPort(0);
	}

}