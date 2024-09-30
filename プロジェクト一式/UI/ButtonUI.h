#pragma once
#include "UIBase.h"

class ButtonUI : public UIBase
{
	bool isFrameDraw_;			//�g��`�悷�邩
	XMFLOAT2 widePos_;			//0�`1980�Ƃ��̍��W
	XMFLOAT2 frameHalfSize_;	//�����̃T�C�Y

public:
	ButtonUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	~ButtonUI() override;
	void Update() override;
	void Draw() override;
	void Initialize(std::string name) override;
	bool IsWithinBound() override;
	void SelectUpdate() override;

	void SetFrameDraw(bool b) { isFrameDraw_ = b; }

};

