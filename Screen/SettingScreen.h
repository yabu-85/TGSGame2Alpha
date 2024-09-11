#pragma once
#include "Screen.h"

class ButtonUI;
class SliderUI;

class SettingScreen : public Screen
{
	enum PICT_TYPE {
		PC_CTRL = 0,
		GAMEPAD,
		PC_GAMEPAD,
		AIM_SPEED,
		BACK_BLACK,
		BACK_WHITE,
		PLAYER1,
		PLAYER2,
		GAME_VOLUME,
		SHADOW_DRAW,
		PICT_MAX,
	};

	int hPict_[PICT_MAX];
	float animTime_;

	ButtonUI* ctrlButtonUI_[4];
	SliderUI* aimSliderUI_[2];
	SliderUI* volumeSliderUI_;

	//PCCtrl
	void SetPCCtrlOn(int index);
	void SetPCCtrlOff(int index);
	
	//AimSensitivity
	void SetJsonSetting();

public:
	SettingScreen();
	~SettingScreen() override;
	void Draw() override;
};

