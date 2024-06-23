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

	void CreateVfxExplode(XMFLOAT3 pos);
	void CreateVfxExplodeSmall(XMFLOAT3 pos);
	void CreateVfxSmoke(XMFLOAT3 pos);

};

