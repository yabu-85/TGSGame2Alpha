#pragma once
#include "Screen.h"

class SelectScreen : public Screen
{
	int hPict_[2];

public:
	SelectScreen();
	~SelectScreen() override;
	void Draw() override;

};

