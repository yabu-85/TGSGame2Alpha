#pragma once
#include "Screen.h"
#include "../Engine/Transform.h"

class PauseUIManager : public Screen
{
	int hPict_[2];
	Transform pauseTrans_;

public:
	PauseUIManager();
	~PauseUIManager() override;
	void Draw() override;

};

