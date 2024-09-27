#pragma once
#include "../Engine/Transform.h"
class Character;

//親の位置に合わせてHPゲージの表示をする
class HealthGauge
{
protected:
	bool isDraw_;				//表示するかどうか
	int hPict_[2];				//画像ハンドル
	int gaugeAlpha_[2];			//ゲージの透明度（2画面時は2つデータが必要）
	int visuallyTime_[2];		//見ている時間
	float parcent_;				//hpのパーセント

	XMFLOAT2 offsetPosition_;	//表示位置
	Transform transform_;
	Character* pParent_;

	//Gaugeの透明度計算する
	void SetGaugeAlpha(int value, int index);

public:
	HealthGauge(Character* parent, XMFLOAT2 size = XMFLOAT2(1.0f, 1.0f));
	virtual ~HealthGauge();
	virtual void Draw(int index);

	void SetOffSetPosition(XMFLOAT2 pos);
	void SetParcent(float f);
	void SetIsDraw(bool b);
};