#pragma once
#include "../Engine/Transform.h"
class GameObject;

class HealthGauge
{
	enum PNG_INDEX {
		GAUGE = 0,
		BACK,
		MAX,
	};

	int hPict_[MAX];		//画像ハンドル
	int gaugeAlpha_;		//ゲージの透明度
	float height_;			//uiの高さ
	float parcent;			//hpのパーセント
	bool isDraw_;			//表示するかどうか
	Transform transform_[2];//HpとHPBack用のTransform
	GameObject* pParent_;

	//Gaugeの透明度計算する
	void SetGaugeAlpha(int value);

public:
	HealthGauge(GameObject* parent);
	virtual ~HealthGauge();
	virtual void Draw();

	void SetHeight(float h);
	void SetParcent(float f);
	void SetIsDraw(bool b);
	void SetAlphaMax();

};

class BossGauge : public HealthGauge {
public:
	BossGauge(GameObject* parent);
	~BossGauge() override;

};