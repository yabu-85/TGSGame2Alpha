#pragma once
#include "Collider.h"

//-----------------------------------------------------------
//�����̓����蔻��
//-----------------------------------------------------------
class SegmentCollider : public Collider
{
	XMVECTOR vec_; //���̌����i���K��

	//Collider�N���X��private�����o�ɃA�N�Z�X�ł���悤�ɂ���
	friend class Collider;

	//�ڐG����
	//�����Ftarget	����̓����蔻��
	//�ߒl�F�ڐG���Ă��true
	bool IsHit(Collider* target) override;

public:
	//�R���X�g���N�^�i�����蔻��̍쐬�j
	//�����FbasePos	�����蔻��̒��S�ʒu�i�Q�[���I�u�W�F�N�g�̌��_���猩���ʒu�j
	//�����Fvec 	���̌���
	//�����Frange	���̒���
	SegmentCollider(XMFLOAT3 center, XMVECTOR vec);
	void SetVector(XMVECTOR _vec);

	float targetDit_;		//�������������ۑ��p
	XMFLOAT3 targetPos_;		//���������ꏊ�ۑ��p

};