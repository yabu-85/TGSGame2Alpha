#pragma once
#include "Screen.h"

class SettingScreen : public Screen
{
public:
	SettingScreen();
	~SettingScreen() override;
	void Draw() override;
};

