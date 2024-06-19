#pragma once
#include <DirectXMath.h>
using namespace DirectX;

//ダメージ表示
//モンハンライズの仕様
//最新のを最前面に表示して順に重ねていく感じ
//途中のサイズ変更はないけど、透明度は変わっているかもしれない
//時間経過で多分消える

//武器以外から追加することもあると思うから、どこからでも追加ができるように
//とりあえずUIに追加の、VFXをまねてnamespaceのDamageEffect

namespace DamageUI
{
	void Initialize();

	//Screen追加
	void AddDamage(XMFLOAT3 _pos, int _damage);
	
	//Screen初期化
	void ResetDamageList();

	//Screenリストの更新
	void Update();

	//Screenリストの描画
	void Draw();

};