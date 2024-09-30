#pragma once
#include "Screen.h"

class TitleScreen : public Screen
{
	int hPict_[2];

public:
	TitleScreen();
	~TitleScreen() override;
	void Update() override;
	void Draw() override;

};

