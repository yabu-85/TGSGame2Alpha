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
	void Update(int id);

	//���_�i�J�����̈ʒu�j��ݒ�
	void SetPosition(XMFLOAT3 position, int id);

	//�œ_�i����ʒu�j��ݒ�
	void SetTarget(XMFLOAT3 target, int id);

	//�ʒu���擾
	XMFLOAT3 GetPosition(int id);

	//�œ_���擾
	XMFLOAT3 GetTarget(int id);

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