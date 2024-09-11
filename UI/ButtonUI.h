#pragma once
#include "UIBase.h"

class ButtonUI : public UIBase
{
	bool isFrameDraw_;			//枠を描画するか
	XMFLOAT2 widePos_;			//0〜1980とかの座標
	XMFLOAT2 frameHalfSize_;	//半分のサイズ

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

