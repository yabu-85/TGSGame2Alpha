#pragma once
#include "../Engine/Transform.h"
class Character;

//�e�̈ʒu�ɍ��킹��HP�Q�[�W�̕\��������
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
	HealthGauge(Character* parent, XMFLOAT2 size = XMFLOAT2(1.0f, 1.0f));
	virtual ~HealthGauge();
	virtual void Draw(int index);

	void SetOffSetPosition(XMFLOAT2 pos);
	void SetParcent(float f);
	void SetIsDraw(bool b);
};