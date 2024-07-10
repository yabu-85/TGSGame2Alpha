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

	int hPict_[MAX];		//�摜�n���h��
	int gaugeAlpha_[2];		//�Q�[�W�̓����x�i2��ʎ���2�f�[�^���K�v�j
	int visuallyTime_[2];	//���Ă��鎞��

	float height_;			//ui�̍���
	float parcent_;			//hp�̃p�[�Z���g
	bool isDraw_;			//�\�����邩�ǂ���
	Transform transform_[2];//Hp��HPBack�p��Transform
	Character* pParent_;

	//Gauge�̓����x�v�Z����
	void SetGaugeAlpha(int value, int index);

public:
	HealthGauge(Character* parent);
	virtual ~HealthGauge();
	virtual void Draw(int index);

	void SetHeight(float h);
	void SetParcent(float f);
	void SetIsDraw(bool b);

};