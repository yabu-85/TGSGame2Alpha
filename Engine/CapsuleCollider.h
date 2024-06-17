#pragma once
#include "Collider.h"


//-----------------------------------------------------------
//球体の当たり判定
//-----------------------------------------------------------
class CapsuleCollider : public Collider
{
	//Colliderクラスのprivateメンバにアクセスできるようにする
	friend class Collider;

	//接触判定
	//引数：target	相手の当たり判定
	//戻値：接触してればtrue
	bool IsHit(Collider* target) override;

public:
	//コンストラクタ（当たり判定の作成）
	//引数：basePos	当たり判定の中心位置（ゲームオブジェクトの原点から見た位置）
	//引数：radius	当たり判定のサイズ（半径）
	//引数：height	当たり判定のサイズ（縦全体の長さ、半径は考慮しない）
	CapsuleCollider(XMFLOAT3 center, float radius, float height);
	CapsuleCollider(XMFLOAT3 center, float radius, float height, XMVECTOR direction);

	void Draw(XMFLOAT3 position);

	float height_;			//縦の長さ
	XMVECTOR direction_;	//カプセルの向いている方向

	float targetDit_;		//当たった距離保存用
	XMFLOAT3 targetPos_;		//当たった場所保存用

};

