#pragma once
#include <DirectXMath.h>
using namespace DirectX;

enum VFX_TYPE {
	Explode = 0,
};

namespace VFXManager {
	//�K�����͌Ă�VFX�f�[�^�̏�����
	void Initialize();

	//Type�Ŏw�肵�Đ��� other��SwordSlahs�Ƃ��Ŏg��
	void CreateVfx(XMFLOAT3 pos, VFX_TYPE type, XMFLOAT3 other = XMFLOAT3());

	void CreateVfxExplode(XMFLOAT3 pos);

};