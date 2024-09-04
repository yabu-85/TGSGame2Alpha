#pragma once
#include "Screen.h"

class SliderUI;

class SettingScreen : public Screen
{
	int hPict_[5];
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

