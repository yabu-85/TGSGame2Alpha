#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Model.h"


//コンストラクタ（当たり判定の作成）
//引数：basePos	当たり判定の中心位置（ゲームオブジェクトの原点から見た位置）
//引数：size	当たり判定のサイズ
BoxCollider::BoxCollider(XMFLOAT3 basePos, XMFLOAT3 size)
{
	center_ = basePos;
	size_ = size;
	type_ = COLLIDER_BOX;

	//リリース時は判定枠は表示しない
#if 1 //_DEBUG
	//テスト表示用判定枠
	hDebugModel_ = Model::Load("DebugCollision/boxCollider.fbx");
#endif
}

//接触判定
//引数：target	相手の当たり判定
//戻値：接触してればtrue
bool BoxCollider::IsHit(Collider* target)
{
	switch (target->type_)
	{
	case COLLIDER_BOX:		return IsHitBoxVsBox((BoxCollider*)target, this);
	case COLLIDER_CIRCLE:	return IsHitBoxVsCircle(this, (SphereCollider*)target);
	case COLLIDER_CAPSULE:	return IsHitBoxVsCapsule(this, (CapsuleCollider*)target);
	case COLLIDER_SEGMENT:	return IsHitBoxVsSegment(this, (SegmentCollider*)target);
	}

	return false;
}