#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Model.h"
#include "Global.h"

CapsuleCollider::CapsuleCollider(XMFLOAT3 center, float radius, float height, XMVECTOR direction)
{
	center_ = center;
	size_ = XMFLOAT3(radius, radius, radius);
	height_ = height;
	direction_ = direction;
	type_ = COLLIDER_CAPSULE;

	//リリース時は判定枠は表示しない
#ifdef _DEBUG
	//テスト表示用判定枠
	hDebugModel_ = Model::Load("DebugCollision/sphereCollider.fbx");
#endif
}

void CapsuleCollider::Draw(XMFLOAT3 position)
{
	XMFLOAT3 fDir;
	XMStoreFloat3(&fDir, direction_);
	if (fDir.x == 0.0f && fDir.y == 0.0f && fDir.z) fDir.z = 0.0001f;

	Transform transform;
	transform.position_ = XMFLOAT3(position.x + center_.x, position.y + center_.y, position.z + center_.z);
	transform.scale_ = XMFLOAT3(size_.x, height_, size_.z);
	transform.rotate_ = CalculationRotateXYZ(fDir);
	transform.Calclation();
	Model::SetTransform(hDebugModel_, transform);
	Model::Draw(hDebugModel_);
}

bool CapsuleCollider::IsHit(Collider* target)
{
	switch (target->type_)
	{
	case COLLIDER_BOX:		return IsHitBoxVsCapsule((BoxCollider*)target, this);
	case COLLIDER_CIRCLE:	return IsHitCircleVsCapsule((SphereCollider*)target, this);
	case COLLIDER_CAPSULE:	return IsHitCapsuleVsCapsule((CapsuleCollider*)target, this);
	}

	return false;
}
