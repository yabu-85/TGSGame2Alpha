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
	const XMFLOAT2 PC_CTRL_BUTTON_SIZE = XMFLOAT2(0.35f, 0.35f);
	const XMFLOAT2 GM_CTRL_BUTTON_SIZE = XMFLOAT2(0.22f, 0.22f);

	const XMFLOAT3 CTRL_BACK_POS = XMFLOAT3(0.0f, 0.38f, 0.0f);		//
	const XMFLOAT3 CTRL_BACK_SIZE = XMFLOAT3(18.0f, 2.2f, 0.0f);	//
	const XMFLOAT3 AIM_BACK_POS = XMFLOAT3(0.0f, -0.05f, 0.0f);		//
	const XMFLOAT3 AIM_BACK_SIZE = XMFLOAT3(18.0f, 2.0f, 0.0f);		//

	const XMFLOAT3 VOLUME_BACK_POS = XMFLOAT3(-0.5f, -0.48f, 0.0f);	//
	const XMFLOAT3 VOLUME_BACK_SIZE = XMFLOAT3(8.0f, 1.7f, 0.0f);	//
	const XMFLOAT3 SHADOW_BACK_POS = XMFLOAT3(0.5f, -0.48f, 0.0f);	//
	const XMFLOAT3 SHADOW_BACK_SIZE = XMFLOAT3(8.0f, 1.7f, 0.0f);	//

}

SettingScreen::SettingScreen() : Screen(), aimSliderUI_{nullptr, nullptr}, volumeSliderUI_(nullptr), animTime_(0.0f)
{
	//ImageÇÃèâä˙ê›íË
	const char* fileName[] = { "Image/PCCtrl.png", "Image/GamePad.png", "Image/PC_GAMEPAD.png", "Image/AimSpeed.png" ,
		"Image/BlackFade.png", "Image/WhiteFade.png", "Image/Player1.png", "Image/Player2.png", "Image/GameVolume.png", "Image/ShadowDraw.png" };
	for (int i = 0; i < PICT_MAX; i++) {
		hPict_[i] = -1;
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	//ImageÇÃTransformê›íË
	Transform t;
	t.scale_ = XMFLOAT3(0.5f, 0.5f, 0.0f);
	t.position_ = XMFLOAT3(-0.5f, 0.7f, 0.0f);
	Image::SetTransform(hPict_[PLAYER1], t);

	t.position_ = XMFLOAT3(0.5f, 0.7f, 0.0f);
	Image::SetTransform(hPict_[PLAYER2], t);

	t.position_ = XMFLOAT3(0.0f, 0.4f, 0.0f);
	Image::SetTransform(hPict_[PC_GAMEPAD], t);

	t.position_ = XMFLOAT3(0.0f, -0.0f, 0.0f);
	Image::SetTransform(hPict_[AIM_SPEED], t);

	t.position_ = XMFLOAT3(-0.5f, -0.45f, 0.0f);
	Image::SetTransform(hPict_[GAME_VOLUME], t);

	t.position_ = XMFLOAT3(0.5f, -0.45f, 0.0f);
	Image::SetTransform(hPict_[SHADOW_DRAW], t);

	Image::SetFullScreenTransform(hPict_[BACK_BLACK]);
	Image::SetAlpha(hPict_[BACK_BLACK], 150);
	Image::SetAlpha(hPict_[BACK_WHITE], 50);

	UIBase* ui = nullptr;

	//ñﬂÇË
	ui = ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.8f), XMFLOAT2(0.35f, 0.3f), XMFLOAT2(0.35f, 0.35f), [this]()
		{
			state_ = ENDDRAW;
		});
	AddUI(ui);
	ui->SetSelect(true);

	//PCCtrlON1-------------------------------------------
	ui = ui->UIInstantiate<ButtonUI>("PCCtrl", XMFLOAT2(-0.65f, 0.3f), XMFLOAT2(0.2f, 0.5f), PC_CTRL_BUTTON_SIZE, [this]()
		{
			SetPCCtrlOn(0);
		});
	AddUI(ui);
	ctrlButtonUI_[0] = static_cast<ButtonUI*>(ui);
	ctrlButtonUI_[0]->SetFrameDraw(false);

	//PCCtrlOFF1
	ui = ui->UIInstantiate<ButtonUI>("GamePad", XMFLOAT2(-0.35f, 0.3f), XMFLOAT2(0.2f, 0.5f), GM_CTRL_BUTTON_SIZE, [this]()
		{
			SetPCCtrlOff(0);
		});
	AddUI(ui);
	ctrlButtonUI_[1] = static_cast<ButtonUI*>(ui);
	ctrlButtonUI_[1]->SetFrameDraw(false);

	//PCCtrlON2
	ui = ui->UIInstantiate<ButtonUI>("PCCtrl", XMFLOAT2(0.35f, 0.3f), XMFLOAT2(0.2f, 0.5f), PC_CTRL_BUTTON_SIZE, [this]()
		{
			SetPCCtrlOn(1);
		});
	AddUI(ui);
	ctrlButtonUI_[2] = static_cast<ButtonUI*>(ui);
	ctrlButtonUI_[2]->SetFrameDraw(false);

	//PCCtrlOFF2
	ui = ui->UIInstantiate<ButtonUI>("GamePad", XMFLOAT2(0.65f, 0.3f), XMFLOAT2(0.2f, 0.5f), GM_CTRL_BUTTON_SIZE, [this]()
		{
			SetPCCtrlOff(1);
		});
	AddUI(ui);
	ctrlButtonUI_[3] = static_cast<ButtonUI*>(ui);
	ctrlButtonUI_[3]->SetFrameDraw(false);
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
	ui = ui->UIInstantiate<SliderUI>("", XMFLOAT2(-0.5f, -0.55f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			SetJsonSetting();
		});
	AddUI(ui);
	volumeSliderUI_ = static_cast<SliderUI*>(ui);

	//-----------------------ShadowDraw-----------------------
	AddUI(ui->UIInstantiate<ButtonUI>("On", XMFLOAT2(0.35f, -0.55f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			GameManager::SetShadowDraw(true);
		}));
	AddUI(ui->UIInstantiate<ButtonUI>("Off", XMFLOAT2(0.65f, -0.55f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.23f, 0.25f), [this]()
		{
			GameManager::SetShadowDraw(false);
		}));

	//JsonSettingì«Ç›çûÇ›
	//AimSensitivity
	JsonReader::Load("Json/GameSetting.json");
	auto& player1Section = JsonReader::GetSection("Player1");
	aimSliderUI_[0]->SetGaugeParcent(player1Section["aimSensitivity"]);
	auto& player2Section = JsonReader::GetSection("Player2");
	aimSliderUI_[1]->SetGaugeParcent(player2Section["aimSensitivity"]);
	//GameVolume
	auto& commonSection = JsonReader::GetSection("Common");
	volumeSliderUI_->SetGaugeParcent(commonSection["gameVolume"]);

}

SettingScreen::~SettingScreen()
{
}

void SettingScreen::Draw()
{
	//âÊñ ï™äÑñ≥ÇµÇ…
	Camera::SetOneProjectionMatrix();
	Direct3D::SetViewOne();
	Direct3D::SetViewPort(0);

	//Back_Black
	Image::Draw(hPict_[BACK_BLACK]);

	//Back_White
	//CTRL
	Transform t;
	t.position_ = CTRL_BACK_POS;
	t.scale_ = CTRL_BACK_SIZE;
	Image::SetTransform(hPict_[BACK_WHITE], t);
	Image::Draw(hPict_[BACK_WHITE]);

	//AIMSenc
	t.position_ = AIM_BACK_POS;
	t.scale_ = AIM_BACK_SIZE;
	Image::SetTransform(hPict_[BACK_WHITE], t);
	Image::Draw(hPict_[BACK_WHITE]);

	//GameVol
	t.position_ = VOLUME_BACK_POS;
	t.scale_ = VOLUME_BACK_SIZE;
	Image::SetTransform(hPict_[BACK_WHITE], t);
	Image::Draw(hPict_[BACK_WHITE]);

	//Shadow
	t.position_ = SHADOW_BACK_POS;
	t.scale_ = SHADOW_BACK_SIZE;
	Image::SetTransform(hPict_[BACK_WHITE], t);
	Image::Draw(hPict_[BACK_WHITE]);

	//PlayerNumber
	Image::Draw(hPict_[PLAYER1]);
	Image::Draw(hPict_[PLAYER2]);

	//Ctrlï`âÊ
	const float addSelectSize = 0.025f;
	const float selectAnimSpeed = 0.09f;
	animTime_ += selectAnimSpeed;
	for (int i = 0; i < 4; i++) {
		int pcNum = GameManager::GetPCCtrlNumber();
		bool pcCtrl = GameManager::IsPCCtrl();

		bool gmSelect = false;
		bool pcSelect = pcCtrl && ((i == 0 && pcNum == 0) || i == 2 && pcNum == 1);
		if (!pcCtrl && (i == 1 || i == 3)) gmSelect = true;
		if(pcCtrl && i == 1 && pcNum == 1) gmSelect = true;
		if(pcCtrl && i == 3 && pcNum == 0) gmSelect = true;

		//ëIÇÒÇ≈Ç¢ÇÈÇŸÇ§ÇÃï`âÊ
		if (pcSelect || gmSelect) {
			float as = cosf(animTime_);
			as *= addSelectSize;
			if(i == 0 || i == 2) ctrlButtonUI_[i]->SetImageScale(XMFLOAT3(PC_CTRL_BUTTON_SIZE.x + as, PC_CTRL_BUTTON_SIZE.y + as, 0.0f));
			else ctrlButtonUI_[i]->SetImageScale(XMFLOAT3(GM_CTRL_BUTTON_SIZE.x + as, GM_CTRL_BUTTON_SIZE.y + as, 0.0f));
		}
		//ëIëÇµÇƒÇ»Ç¢ÇŸÇ§ÇÃï`âÊ
		else {
			if (i == 0 || i == 2) ctrlButtonUI_[i]->SetImageScale(XMFLOAT3(PC_CTRL_BUTTON_SIZE.x, PC_CTRL_BUTTON_SIZE.y, 0.0f));
			else ctrlButtonUI_[i]->SetImageScale(XMFLOAT3(GM_CTRL_BUTTON_SIZE.x, GM_CTRL_BUTTON_SIZE.y, 0.0f));
		}
	}

	//PC/GamePadÇÃâÊëú
	Image::Draw(hPict_[PC_GAMEPAD]);

	//Aim
	Image::Draw(hPict_[AIM_SPEED]);

	//GameVolume
	Image::Draw(hPict_[GAME_VOLUME]);

	//ShadowDraw
	Image::Draw(hPict_[SHADOW_DRAW]);

	Screen::Draw();
	
	//âÊñ ï™äÑ2Ç…ñﬂÇ∑
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
	//PCCtrlÇ»ÇµÇ…
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

	//ç°PlaySceneÇ≈ÅAÇQêlÇ∆Ç‡ê∂Ç´ÇƒÇ¢ÇÈèÍçá
	if (GameManager::GetPlayer(0) && GameManager::GetPlayer(1)) {
		GameManager::GetPlayer(0)->GetAim()->SetAimSensitivity(aimParcent[0]);
		GameManager::GetPlayer(1)->GetAim()->SetAimSensitivity(aimParcent[1]);
	}

	nlohmann::json j;
	nlohmann::json playerJson;

	//GameSettingì«Ç›çûÇ›
	JsonReader::Load("Json/GameSetting.json");
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

	//ã§í èÓïÒ
	auto& sectionCommon = JsonReader::GetSection("Common"); 
	float gameVolume = volumeSliderUI_->GetGaugeParcent();
	nlohmann::json commonJson;
	commonJson["gameVolume"] = gameVolume;
	if(GameManager::IsShadowDraw()) commonJson["shadowDraw"] = 1;
	else commonJson["shadowDraw"] = 0;
	j[jsonName[2]] = commonJson;
	
	//AudioÇÃâπó ÉZÉbÉg
	AudioManager::SetVolume(gameVolume);

	std::ofstream ofs("Json/GameSetting.json");
	if (!ofs.is_open())
	{
		assert(false);
		return;
	}

	ofs << j.dump(4);	
}

