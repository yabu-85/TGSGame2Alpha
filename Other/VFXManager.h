#pragma once
#include <DirectXMath.h>
using namespace DirectX;

enum VFX_TYPE {
	Explode = 0,
};

namespace VFXManager {
	//必ず一回は呼ぶVFXデータの初期化
	void Initialize();

	//Typeで指定して生成 other＝SwordSlahsとかで使う
	void CreateVfx(XMFLOAT3 pos, VFX_TYPE type, XMFLOAT3 other = XMFLOAT3());

	void CreateVfxExplode(XMFLOAT3 pos);

};