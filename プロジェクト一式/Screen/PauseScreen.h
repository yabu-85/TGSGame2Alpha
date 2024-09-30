#pragma once
#include "Screen.h"

class PauseScreen : public Screen
{
	int hPict_[2];

public:
	PauseScreen();
	~PauseScreen() override;
	void Draw() override;

};

