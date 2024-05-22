#pragma once
#include "Collider.h"


//-----------------------------------------------------------
//球体の当たり判定
//-----------------------------------------------------------
class CapsuleCollider : public Collider
{
	//Colliderクラスのprivateメンバにアクセスできるようにする
	friend class Collider;

	float height_;
	XMVECTOR direction_;

	//接触判定
	//引数：target	相手の当たり判定
	//戻値：接触してればtrue
	bool IsHit(Collider* target) override;

public:
	//コンストラクタ（当たり判定の作成）
	//引数：basePos	当たり判定の中心位置（ゲームオブジェクトの原点から見た位置）
	//引数：radius	当たり判定のサイズ（半径）
	CapsuleCollider(XMFLOAT3 center, float radius, float height, XMVECTOR direction = { 0,0,0,0 });

	void Draw(XMFLOAT3 position);

};

