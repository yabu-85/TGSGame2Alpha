#pragma once
#include <DirectXMath.h>
using namespace DirectX;

enum VFX_TYPE {
	Explode = 0,
};

namespace VFXManager
{
	//�K�����͌Ă�VFX�f�[�^�̏�����
	void Initialize();

	//���ڐ���
	void CreateVfxExplode1(XMFLOAT3 pos);
};

