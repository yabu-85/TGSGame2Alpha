#pragma once
#include "Screen.h"

class SliderUI;

class SettingScreen : public Screen
{
	enum PICT_TYPE {
		PC_CTRL = 0,
		GAMEPAD,
		AIM_SPEED,
		BACK_BLACK,
		BACK_WHITE,
		PLAYER1,
		PLAYER2,
		PICT_MAX,
	};

	int hPict_[PICT_MAX];
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

