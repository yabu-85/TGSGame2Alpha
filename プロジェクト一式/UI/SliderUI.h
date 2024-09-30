#pragma once
#include "UIBase.h"

class SliderUI : public UIBase
{
	float gaugePercent_;	//ゲージパーセント
	float buttonPosX_;		//X軸のボタンの座標（0~1
	bool isDragging_;		//ボタンを押しているか
	XMFLOAT2 frameSize_;	//フレームのサイズ

	//スライダー動かしているときの処理
	void Dragging();

public:
	SliderUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	~SliderUI() override;
	void Update() override;
	void Draw() override;
	void Initialize(std::string name) override;
	bool IsWithinBound() override;
	void SelectUpdate() override;

	//０～１の間で取得
	float GetGaugeParcent() { return gaugePercent_; }

	void SetGaugeParcent(float f);

};

