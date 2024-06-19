#pragma once
#include "Screen.h"
#include "../Engine/Transform.h"

class PauseScreen : public Screen
{
	int hPict_[2];
	int hCurPict_;
	Transform pauseTrans_;

public:
	PauseScreen();
	~PauseScreen() override;
	void Draw() override;

};

