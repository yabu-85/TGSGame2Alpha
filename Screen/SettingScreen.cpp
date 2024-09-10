#include "SettingScreen.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "../Engine/Image.h"
#include "../Other/GameManager.h"
#include "../Other/AudioManager.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Json/JsonReader.h"
#include "../UI/SliderUI.h"
#include "../UI/ButtonUI.h"
#include <fstream>

namespace {
	const XMFLOAT3 CTRL_SIZE = XMFLOAT3(0.5f, 0.5f, 0.0f);		//CTRL画像のサイズ
	const XMFLOAT3 CTRL_POS1 = XMFLOAT3(-0.5f, 0.47f, 0.0f);	//CTRL画像の座標１
	const XMFLOAT3 CTRL_POS2 = XMFLOAT3(0.5f, 0.47f, 0.0f);		//CTRL画像の座標２

	const XMFLOAT3 CTRL_BACK_POS = XMFLOAT3(0.0f, 0.38f, 0.0f);			//
	const XMFLOAT3 CTRL_BACK_SIZE = XMFLOAT3(18.0f, 2.2f, 0.0f);		//
	const XMFLOAT3 AIM_SENSITIVITY_POS = XMFLOAT3(0.0f, -0.05f, 0.0f);	//
	const XMFLOAT3 AIM_SENSITIVITY_SIZE = XMFLOAT3(16.0f, 2.0f, 0.0f);	//

}

SettingScreen::SettingScreen() : Screen(), aimSliderUI_{nullptr, nullptr}, volumeSliderUI_(nullptr)
{
	//Imageの初期設定
	const char* fileName[] = { "Image/PCCtrl.png", "Image/GamePad.png", "Image/AimSpeed.png" , "Image/BlackFade.png", "Image/WhiteFade.png",
		"Image/Player1.png", "Image/Player2.png" };
	for (int i = 0; i < PICT_MAX; i++) {
		hPict_[i] = -1;
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	//ImageのTransform設定
	Transform t;
	t.scale_ = XMFLOAT3(0.5f, 0.5f, 0.0f);
	t.position_ = XMFLOAT3(-0.5f, 0.7f, 0.0f);
	Image::SetTransform(hPict_[PLAYER1], t);

	t.position_ = XMFLOAT3(0.5f, 0.7f, 0.0f);
	Image::SetTransform(hPict_[PLAYER2], t);

	t.position_ = XMFLOAT3(0.0f, -0.0f, 0.0f);
	Image::SetTransform(hPict_[AIM_SPEED], t);

	Image::SetFullScreenTransform(hPict_[BACK_BLACK]);
	Image::SetAlpha(hPict_[BACK_BLACK], 100);
	Image::SetAlpha(hPict_[BACK_WHITE], 100);

	UIBase* ui = nullptr;

	//戻り
	ui = ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.8f), XMFLOAT2(0.35f, 0.3f), XMFLOAT2(0.35f, 0.35f), [this]()
		{
			state_ = ENDDRAW;
		});
	AddUI(ui);
	ui->SetSelect(true);

	//PCCtrlON1-------------------------------------------
	AddUI(ui->UIInstantiate<ButtonUI>("OK", XMFLOAT2(-0.65f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOn(0);
		}));

	//PCCtrlOFF1
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(-0.4f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOff(0);
		}));

	//PCCtrlON2
	AddUI(ui->UIInstantiate<ButtonUI>("OK", XMFLOAT2(0.35f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOn(1);
		}));

	//PCCtrlOFF2
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.6f, 0.3f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			SetPCCtrlOff(1);
		}));
	//------------------------------------------------

	//MouseSence1
	ui = ui->UIInstantiate<SliderUI>("", XMFLOAT2(-0.525f, -0.15f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			SetJsonSetting();
		});
	AddUI(ui);
	aimSliderUI_[0] = static_cast<SliderUI*>(ui);

	//MouseSence2
	ui = ui->UIInstantiate<SliderUI>("", XMFLOAT2(0.525f, -0.15f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			SetJsonSetting();
		});
	AddUI(ui);
	aimSliderUI_[1] = static_cast<SliderUI*>(ui);

	//------------------------SoundValue------------------------
	ui = ui->UIInstantiate<SliderUI>("", XMFLOAT2(0.0f, -0.6f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			SetJsonSetting();
		});
	AddUI(ui);
	volumeSliderUI_ = static_cast<SliderUI*>(ui);


	//JsonSetting読み込み
	JsonReader::Load("Json/PlayerSetting.json");
	auto& player1Section = JsonReader::GetSection("Player1");
	aimSliderUI_[0]->SetGaugeParcent(player1Section["aimSensitivity"]);
	auto& player2Section = JsonReader::GetSection("Player2");
	aimSliderUI_[1]->SetGaugeParcent(player2Section["aimSensitivity"]);

	auto& commonSection = JsonReader::GetSection("Common");
	volumeSliderUI_->SetGaugeParcent(commonSection["gameVolume"]);

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

	//Back_Black
	Image::Draw(hPict_[BACK_BLACK]);

	//Back_White
	Transform t;
	t.position_ = CTRL_BACK_POS;
	t.scale_ = CTRL_BACK_SIZE;
	Image::SetTransform(hPict_[BACK_WHITE], t);
	Image::Draw(hPict_[BACK_WHITE]);

	//Back_White
	t.position_ = AIM_SENSITIVITY_POS;
	t.scale_ = AIM_SENSITIVITY_SIZE;
	Image::SetTransform(hPict_[BACK_WHITE], t);
	Image::Draw(hPict_[BACK_WHITE]);

	//PlayerNumber
	Image::Draw(hPict_[PLAYER1]);
	Image::Draw(hPict_[PLAYER2]);

	//デバッグPCCTRLNumberの描画
	if (GameManager::IsPCCtrl()) {
		//Player1がPCCtrlの時
		if (GameManager::GetPCCtrlNumber() == 0) {
			Transform t;
			t.scale_ = CTRL_SIZE;
			t.position_ = CTRL_POS1;
			Image::SetTransform(hPict_[PC_CTRL], t);
			Image::Draw(hPict_[PC_CTRL]);

			t.position_ = CTRL_POS2;
			Image::SetTransform(hPict_[GAMEPAD], t);
			Image::Draw(hPict_[GAMEPAD]);
		}
		//Player2がPCCtrlの時
		else {
			Transform t;
			t.scale_ = CTRL_SIZE;
			t.position_ = CTRL_POS1;
			Image::SetTransform(hPict_[GAMEPAD], t);
			Image::Draw(hPict_[GAMEPAD]);

			t.position_ = CTRL_POS2;
			Image::SetTransform(hPict_[PC_CTRL], t);
			Image::Draw(hPict_[PC_CTRL]);
		}
	}
	else {
		Transform t;
		t.scale_ = CTRL_SIZE;
		t.position_ = CTRL_POS1;
		Image::SetTransform(hPict_[GAMEPAD], t);
		Image::Draw(hPict_[GAMEPAD]);

		t.position_ = CTRL_POS2;
		Image::SetTransform(hPict_[GAMEPAD], t);
		Image::Draw(hPict_[GAMEPAD]);
	}

	//Aim
	Image::Draw(hPict_[AIM_SPEED]);

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
	SetJsonSetting();
}

void SettingScreen::SetPCCtrlOff(int index)
{
	//PCCtrlなしに
	if (GameManager::IsPCCtrl() && index == GameManager::GetPCCtrlNumber()) {
		GameManager::SetPCCtrlOFF();
	}

	SetJsonSetting();

}

void SettingScreen::SetJsonSetting()
{
	float aimParcent[2] = { 0.0f };
	aimParcent[0] = aimSliderUI_[0]->GetGaugeParcent();
	aimParcent[1] = aimSliderUI_[1]->GetGaugeParcent();

	//今PlaySceneで、２人とも生きている場合
	if (GameManager::GetPlayer(0) && GameManager::GetPlayer(1)) {
		GameManager::GetPlayer(0)->GetAim()->SetAimSensitivity(aimParcent[0]);
		GameManager::GetPlayer(1)->GetAim()->SetAimSensitivity(aimParcent[1]);
	}

	nlohmann::json j;
	nlohmann::json playerJson;

	//PlayerSetting読み込み
	JsonReader::Load("Json/PlayerSetting.json");
	auto& section1 = JsonReader::GetSection("Player1");
	auto& section2 = JsonReader::GetSection("Player2");

	std::string jsonName[] = { "Player1", "Player2", "Common"};
	for (int i = 0; i < 2; i++) {
		//Player
		if (i == 0) {
			playerJson["hp"] = (int)section1["hp"];
		}
		else {
			playerJson["hp"] = (int)section2["hp"];
		}

		//Aim
		playerJson["aimSensitivity"] = aimParcent[i];

		//Othre
		if (GameManager::IsPCCtrl() && GameManager::GetPCCtrlNumber() == i) playerJson["pcCtrl"] = 1;
		else playerJson["pcCtrl"] = 0;
	
		j[jsonName[i]] = playerJson;
	}

	//共通情報
	auto& sectionCommon = JsonReader::GetSection("Common"); 
	float gameVolume = volumeSliderUI_->GetGaugeParcent();
	nlohmann::json commonJson;
	commonJson["gameVolume"] = gameVolume;
	j[jsonName[2]] = commonJson;
	//Setする
	AudioManager::SetVolume(gameVolume);

	std::ofstream ofs("Json/PlayerSetting.json");
	if (!ofs.is_open())
	{
		assert(false);
		return;
	}

	ofs << j.dump(4);	
}

