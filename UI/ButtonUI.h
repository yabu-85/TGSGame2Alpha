#pragma once
#include "UIBase.h"

class ButtonUI : public UIBase
{
	XMFLOAT2 widePos_;			//0～1980とかの座標
	XMFLOAT2 frameHalfSize_;	//半分のサイズ

public:
	ButtonUI();
	~ButtonUI() override;
	bool IsWithinBound() override;
	void Initialize() override;

};

