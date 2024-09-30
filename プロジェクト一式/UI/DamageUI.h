#pragma once
#include <DirectXMath.h>
using namespace DirectX;

//�_���[�W�\��
//�����n�����C�Y�̎d�l
//�ŐV�̂��őO�ʂɕ\�����ď��ɏd�˂Ă�������
//�r���̃T�C�Y�ύX�͂Ȃ����ǁA�����x�͕ς���Ă��邩������Ȃ�
//���Ԍo�߂ő���������

//����ȊO����ǉ����邱�Ƃ�����Ǝv������A�ǂ�����ł��ǉ����ł���悤��
//�Ƃ肠����UI�ɒǉ��́AVFX���܂˂�namespace��DamageEffect

namespace DamageUI
{
	//������
	void Initialize();

	//�V�[���`�F���W
	void SceneChange();

	//�������
	void Release();

	//�^����ꂽ���̃v���C���[ID
	void AddDirectionDamage(XMFLOAT3 pos, XMFLOAT3 pos2, int index);

	//�_���[�WUI�ǉ�
	//�ꏊ�E�_���[�W�ʁE�^����ꂽ���̃v���C���[ID
	void AddDamage(XMFLOAT3 _pos, int _damage, int index);
	
	//�_���[�WUI���X�g���Z�b�g
	void ResetDamageList();

	//�_���[�WUI��Update
	void Update();

	//�_���[�WUI�̕`��
	//����index �F�v���C���[��ID
	void Draw(int index);

};