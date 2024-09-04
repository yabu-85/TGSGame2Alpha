#pragma once
#include "Screen.h"

class SliderUI;

class SettingScreen : public Screen
{
	enum PICT_TYPE {
		PC_CTRL = 0,
		GAMEPAD,
		AIM_SPEED,
		BACK,
		PLAYER1,
		PLAYER2,
		PICT_MAX,
	};

	int hPict_[PICT_MAX];
	SliderUI* aimSliderUI_[2];

	//PCCtrl
	void SetPCCtrlOn(int index);
	void SetPCCtrlOff(int index);
	
	//AimSensitivity
	void SetPlayerSetting();

public:
	SettingScreen();
	~SettingScreen() override;
	void Draw() override;
};

