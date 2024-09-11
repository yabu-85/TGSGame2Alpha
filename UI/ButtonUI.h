#pragma once
#include "UIBase.h"

class ButtonUI : public UIBase
{
	bool isFrameDraw_;			//ògÇï`âÊÇ∑ÇÈÇ©
	XMFLOAT2 widePos_;			//0Å`1980Ç∆Ç©ÇÃç¿ïW
	XMFLOAT2 frameHalfSize_;	//îºï™ÇÃÉTÉCÉY

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

