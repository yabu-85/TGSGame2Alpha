#pragma once
#include "Screen.h"

class SliderUI;

class SettingScreen : public Screen
{
	int hPict_[3];
	SliderUI* aimSliderUI_[2];

	//PCCtrl
	void SetPCCtrlOn(int index);
	void SetPCCtrlOff(int index);
	
	//AimSensitivity
	void SetAimSensitivity();

	void SaveFilePlayerSetting();

public:
	SettingScreen();
	~SettingScreen() override;
	void Draw() override;
};

