#pragma once
#include "UIBase.h"

class ButtonUI : public UIBase
{
	XMFLOAT2 widePos_;			//0�`1980�Ƃ��̍��W
	XMFLOAT2 frameHalfSize_;	//�����̃T�C�Y

public:
	ButtonUI();
	~ButtonUI() override;
	bool IsWithinBound() override;
	void Initialize() override;

};

