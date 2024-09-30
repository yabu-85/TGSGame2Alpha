#include "SegmentCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Model.h"

//�R���X�g���N�^�i�����蔻��̍쐬�j
//�����FbasePos	�����蔻��̒��S�ʒu�i�Q�[���I�u�W�F�N�g�̌��_���猩���ʒu�j
//�����Fsize	�����蔻��̃T�C�Y
SegmentCollider::SegmentCollider(XMFLOAT3 center, XMVECTOR vec)
{
	center_ = center;
	type_ = COLLIDER_SEGMENT;

	float size = XMVectorGetX(XMVector3Length(vec));
	size_ = XMFLOAT3(size, size, size);
	vec_ = XMVector3Normalize(vec);

	//�����[�X���͔���g�͕\�����Ȃ�
#ifdef _DEBUG
	//�e�X�g�\���p����g
	hDebugModel_ = Model::Load("DebugCollision/sphereCollider.fbx");
#endif
}

void SegmentCollider::SetVector(XMVECTOR _vec)
{
	vec_ = XMVector3Normalize(_vec);
}

//�ڐG����
//�����Ftarget	����̓����蔻��
//�ߒl�F�ڐG���Ă��true
bool SegmentCollider::IsHit(Collider* target)
{
	switch (target->type_)
	{
	case COLLIDER_BOX:		return IsHitBoxVsSegment((BoxCollider*)target, this);
	case COLLIDER_CIRCLE:	return IsHitCircleVsSegment((SphereCollider*)target, this);
	case COLLIDER_CAPSULE:	return IsHitCapsuleVsSegment((CapsuleCollider*)target, this);
	case COLLIDER_SEGMENT:	return false;	//����������Ȃ����
	}

	return false;
}