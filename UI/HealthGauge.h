#pragma once
#include "../Engine/Transform.h"
class Character;

class HealthGauge
{
	enum PNG_INDEX {
		GAUGE = 0,
		BACK,
		MAX,
	};

	int hPict_[MAX];		//画像ハンドル
	int gaugeAlpha_[2];		//ゲージの透明度（2画面時は2つデータが必要）
	int visuallyTime_[2];	//見ている時間

	float height_;			//uiの高さ
	float parcent_;			//hpのパーセント
	bool isDraw_;			//表示するかどうか
	Transform transform_[2];//HpとHPBack用のTransform
	Character* pParent_;

	//Gaugeの透明度計算する
	void SetGaugeAlpha(int value, int index);

public:
	HealthGauge(Character* parent);
	virtual ~HealthGauge();
	virtual void Draw(int index);

	void SetHeight(float h);
	void SetParcent(float f);
	void SetIsDraw(bool b);

};