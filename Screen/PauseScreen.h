#pragma once
#include "Screen.h"

class PauseScreen : public Screen
{
	int hPict_[2];
	Transform pauseTrans_;

public:
	PauseScreen();
	~PauseScreen() override;
	void Draw() override;

};

