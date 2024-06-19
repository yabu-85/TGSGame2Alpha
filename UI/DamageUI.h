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
	void Initialize();

	//Screen�ǉ�
	void AddDamage(XMFLOAT3 _pos, int _damage);
	
	//Screen������
	void ResetDamageList();

	//Screen���X�g�̍X�V
	void Update();

	//Screen���X�g�̕`��
	void Draw();

};