#pragma once
#include "../Engine/Transform.h"
class Character;

class HealthGauge
{
protected:
	bool isDraw_;				//�\�����邩�ǂ���
	int hPict_[2];				//�摜�n���h��
	int gaugeAlpha_[2];			//�Q�[�W�̓����x�i2��ʎ���2�f�[�^���K�v�j
	int visuallyTime_[2];		//���Ă��鎞��
	float parcent_;				//hp�̃p�[�Z���g

	XMFLOAT2 offsetPosition_;	//�\���ʒu
	Transform transform_;
	Character* pParent_;

	//Gauge�̓����x�v�Z����
	void SetGaugeAlpha(int value, int index);

public:
	HealthGauge(Character* parent);
	virtual ~HealthGauge();
	virtual void Draw(int index);

	void SetOffSetPosition(XMFLOAT2 pos);
	void SetParcent(float f);
	void SetIsDraw(bool b);

};