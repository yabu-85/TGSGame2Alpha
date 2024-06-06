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

	//直接生成
	void CreateVfxExplode1(XMFLOAT3 pos);
};

