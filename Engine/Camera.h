#pragma once
#include "GameObject.h"

//-----------------------------------------------------------
//�J����
//-----------------------------------------------------------
namespace Camera
{
	//�������i�v���W�F�N�V�����s��쐬�j
	void Initialize();

	//�X�V�i�r���[�s��쐬�j
	void Update();

	//�X�V�i�r���[�s��쐬�j
	void TwoWindowUpdate();

	//���_�i�J�����̈ʒu�j��ݒ�
	void SetPosition(XMFLOAT3 position);
	void SetPosition2(XMFLOAT3 position);

	//�œ_�i����ʒu�j��ݒ�
	void SetTarget(XMFLOAT3 target);
	void SetTarget2(XMFLOAT3 target);

	//�ʒu���擾
	XMFLOAT3 GetPosition();

	//�œ_���擾
	XMFLOAT3 GetTarget();

	//�r���[�s����擾
	XMMATRIX GetViewMatrix();

	//�v���W�F�N�V�����s����擾
	XMMATRIX GetProjectionMatrix();

	//�r���{�[�h�p��]�s����擾
	XMMATRIX GetBillboardMatrix();

	//�����̃��[���h���W����ʓ����ǂ���
	bool IsWorldPositionWithinScreen(XMFLOAT3 pos, float size = 1.0f);

	//�����̃X�N���[�����W����ʓ����ǂ���
	bool IsScreenPositionWithinScreen(XMFLOAT3 pos, float size = 1.0f);

	//��ʂ̂ǂ��̃|�W�V�������ǂ����v�Z
	XMFLOAT3 CalcScreenPosition(XMFLOAT3 pos);

};