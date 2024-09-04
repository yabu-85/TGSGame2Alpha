#include "SettingScreen.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "../Engine/Image.h"
#include "../Other/GameManager.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Json/JsonReader.h"
#include "../UI/SliderUI.h"
#include "../UI/ButtonUI.h"
#include <fstream>

SettingScreen::SettingScreen() : Screen(), aimSliderUI_{nullptr, nullptr}
{
	//Imageの初期設定
	const char* fileName[] = { "Image/Stage1.png", "Image/Stage2.png", "Image/StageSelect.png", "Image/AimSpeed.png" , "Image/BlackFade.png" };
	for (int i = 0; i < 5; i++) {
		hPict_[i] = -1;
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	//ImageのTransform設定
	Transform t;
	t.position_ = XMFLOAT3(0.0f, 0.5f, 0.0f);
	t.scale_ = XMFLOAT3(0.5f, 0.5f, 0.0f);
	for(int i = 0;i < 3;i++) Image::SetTransform(hPict_[i], t);

	t.position_ = XMFLOAT3(0.0f, -0.2f, 0.0f);
	Image::SetTransform(hPict_[3], t);

	Image::SetAlpha(hPict_[4], 100);
	Image::SetFullScreenTransform(hPict_[4]);

	UIBase* ui = nullptr;

	//戻り
	ui = ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.7f), XMFLOAT2(0.35f, 0.3f), XMFLOAT2(0.35f, 0.35f), [this]()
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
	ui = ui->UIInstantiate<SliderUI>("Setting", XMFLOAT2(-0.525f, -0.3f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			SetPlayerSetting();
		});
	AddUI(ui);
	aimSliderUI_[0] = static_cast<SliderUI*>(ui);

	//MouseSence2
	ui = ui->UIInstantiate<SliderUI>("Setting", XMFLOAT2(0.525f, -0.3f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			SetPlayerSetting();
		});
	AddUI(ui);
	aimSliderUI_[1] = static_cast<SliderUI*>(ui);

	//PlayerSetting読み込み
	JsonReader::Load("Json/PlayerSetting.json");
	auto& player1Section = JsonReader::GetSection("Player1");
	aimSliderUI_[0]->SetGaugeParcent(player1Section["aimSensitivtiy"]);
	auto& player2Section = JsonReader::GetSection("Player2");
	aimSliderUI_[1]->SetGaugeParcent(player2Section["aimSensitivtiy"]);

}

SettingScreen::~SettingScreen()
{
}

void SettingScreen::Draw()
{
	//画面分割無しに
	Camera::SetOneProjectionMatrix();
	Direct3D::SetViewOne();
	Direct3D::SetViewPort(0);

	//Back
	Image::Draw(hPict_[4]);

	//デバッグPCCTRLNumberの描画
	if (GameManager::IsPCCtrl()) {
		if(GameManager::GetPCCtrlNumber() == 0) Image::Draw(hPict_[0]);
		else Image::Draw(hPict_[1]);
	}else Image::Draw(hPict_[2]);

	//Aim
	Image::Draw(hPict_[3]);

	Screen::Draw();
	
	//画面分割2に戻す
	if (!GameManager::IsOnePlayer()) {
		Camera::SetTwoProjectionMatrix();
		Direct3D::SetViewTwo();
		Direct3D::SetViewPort(0);
	}

}

void SettingScreen::SetPCCtrlOn(int index)
{
	GameManager::SetPCCtrlON();
	GameManager::SetPCCtrlNumber(index);
	SetPlayerSetting();
}

void SettingScreen::SetPCCtrlOff(int index)
{
	if (GameManager::IsPCCtrl()) {
		//PCCtrlなしに
		if (index == GameManager::GetPCCtrlNumber()) {
			GameManager::SetPCCtrlOFF();
		}
		//すでにOffの人が押した
		else {
			//処理終了
		}

	}
	else {
		//処理終了
	}

	SetPlayerSetting();

}

void SettingScreen::SetPlayerSetting()
{
	float aimS1 = aimSliderUI_[0]->GetGaugeParcent();
	float aimS2 = aimSliderUI_[1]->GetGaugeParcent();

	//今PlaySceneで、２人とも生きている場合
	if (GameManager::GetPlayer(0) && GameManager::GetPlayer(1)) {
		GameManager::GetPlayer(0)->GetAim()->SetAimSensitivity(aimS1);
		GameManager::GetPlayer(1)->GetAim()->SetAimSensitivity(aimS2);
	}

	nlohmann::json j;
	nlohmann::json environmentJson;

	//Player1
	environmentJson["aimSensitivtiy"] = aimS1;
	if(GameManager::IsPCCtrl() && GameManager::GetPCCtrlNumber() == 0) environmentJson["pcCtrl"] = 1;
	else environmentJson["pcCtrl"] = 0;
	j["Player1"] = environmentJson;

	//Player2
	environmentJson["aimSensitivtiy"] = aimS2;
	if (GameManager::IsPCCtrl() && GameManager::GetPCCtrlNumber() == 1) environmentJson["pcCtrl"] = 1;
	else environmentJson["pcCtrl"] = 0;
	j["Player2"] = environmentJson;

	std::ofstream ofs("Json/PlayerSetting.json");
	if (!ofs.is_open())
	{
		assert(false);
		return;
	}

	ofs << j.dump(4);
}

