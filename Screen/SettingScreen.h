#pragma once
#include "Screen.h"

class SettingScreen : public Screen
{
	void SetPCCtrlOn(int index);
	void SetPCCtrlOff(int index);

public:
	SettingScreen();
	~SettingScreen() override;
	void Draw() override;
};

