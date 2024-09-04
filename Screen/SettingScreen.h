#pragma once
#include "Screen.h"

class SettingScreen : public Screen
{
	void SetPCCtrlOn(int index);
	void SetPCCtrlOff();

public:
	SettingScreen();
	~SettingScreen() override;
	void Draw() override;
};

