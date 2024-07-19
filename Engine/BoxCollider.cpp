#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "Model.h"


//�R���X�g���N�^�i�����蔻��̍쐬�j
//�����FbasePos	�����蔻��̒��S�ʒu�i�Q�[���I�u�W�F�N�g�̌��_���猩���ʒu�j
//�����Fsize	�����蔻��̃T�C�Y
BoxCollider::BoxCollider(XMFLOAT3 basePos, XMFLOAT3 size)
{
	center_ = basePos;
	size_ = size;
	type_ = COLLIDER_BOX;

	//�����[�X���͔���g�͕\�����Ȃ�
#if 1 //_DEBUG
	//�e�X�g�\���p����g
	hDebugModel_ = Model::Load("DebugCollision/boxCollider.fbx");
#endif
}

//�ڐG����
//�����Ftarget	����̓����蔻��
//�ߒl�F�ڐG���Ă��true
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