#pragma once
#include "Screen.h"

class ExitScreen : public Screen
{
	int hPict_[2];

public:
	ExitScreen();
	~ExitScreen() override;
	void Draw() override;

};

