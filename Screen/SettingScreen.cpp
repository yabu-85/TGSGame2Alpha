#include "SettingScreen.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "../Other/GameManager.h"
#include "../Player/Player.h"
#include "../UI/SliderUI.h"
#include "../UI/ButtonUI.h"

SettingScreen::SettingScreen() : Screen()
{
	UIBase* ui = nullptr;

	//–ß‚è
	ui = ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.7f), XMFLOAT2(0.4f, 0.35f), XMFLOAT2(0.4f, 0.4f), [this]()
		{
			state_ = ENDDRAW;
		});
	AddUI(ui);
	ui->SetSelect(true);

	//PCCtrlON1-------------------------------------------
	AddUI(ui->UIInstantiate<ButtonUI>("OK", XMFLOAT2(-0.75f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOn(0);
		}));

	//PCCtrlOFF1
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(-0.3f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOff(0);
		}));

	//PCCtrlON2
	AddUI(ui->UIInstantiate<ButtonUI>("OK", XMFLOAT2(0.25f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOn(1);
		}));

	//PCCtrlOFF2
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.7f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOff(1);
		}));
	//------------------------------------------------

	//MouseSence1
	AddUI(ui->UIInstantiate<SliderUI>("Setting", XMFLOAT2(-0.525f, -0.3f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
		}));

	//MouseSence2
	AddUI(ui->UIInstantiate<SliderUI>("Setting", XMFLOAT2(0.525f, -0.3f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
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

void SettingScreen::SetPCCtrlOn(int index)
{
}

void SettingScreen::SetPCCtrlOff(int index)
{
}
