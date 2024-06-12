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

	int hPict_[MAX];		//�摜�n���h��
	int gaugeAlpha_;		//�Q�[�W�̓����x
	float height_;			//ui�̍���
	float parcent;			//hp�̃p�[�Z���g
	bool isDraw_;			//�\�����邩�ǂ���
	Transform transform_[2];//Hp��HPBack�p��Transform
	GameObject* pParent_;

	//Gauge�̓����x�v�Z����
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