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
	//初期化
	void Initialize();

	//シーンチェンジ
	void SceneChange();

	//解放処理
	void Release();

	//与えられた側のプレイヤーID
	void AddDirectionDamage(XMFLOAT3 pos, XMFLOAT3 pos2, int index);

	//ダメージUI追加
	//場所・ダメージ量・与えられた側のプレイヤーID
	void AddDamage(XMFLOAT3 _pos, int _damage, int index);
	
	//ダメージUIリストリセット
	void ResetDamageList();

	//ダメージUIのUpdate
	void Update();

	//ダメージUIの描画
	//引数index ：プレイヤーのID
	void Draw(int index);

};