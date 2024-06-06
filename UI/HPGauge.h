#pragma once
#include "../Engine/Transform.h"
class EnemyBase;

class HPGauge
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
	EnemyBase* pParent_;	//�e

	//Gauge�̓����x�v�Z����
	void SetGaugeAlpha(int value);

public:
	HPGauge(EnemyBase* parent);
	~HPGauge();
	void Initialize(float height); //height�FUi�̍���
	void Draw();
	void SetParcent(float f);
	void SetIsDraw(bool b);
	void SetAlphaMax();

};

