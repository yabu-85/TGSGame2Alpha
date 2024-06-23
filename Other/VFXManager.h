#pragma once
#include <DirectXMath.h>
using namespace DirectX;

enum VFX_TYPE {
	Explode = 0,
};

namespace VFXManager
{
	//必ず一回は呼ぶVFXデータの初期化
	void Initialize();

	void CreateVfxExplode(XMFLOAT3 pos);
	void CreateVfxExplodeSmall(XMFLOAT3 pos);
	void CreateVfxSmoke(XMFLOAT3 pos);

};

