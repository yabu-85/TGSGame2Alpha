#include "BoxCollider.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"

//�R���X�g���N�^
Collider::Collider():
    pGameObject_(nullptr), center_(XMFLOAT3()), size_(XMFLOAT3(1.0f, 1.0f, 1.0f))
{
}

//�f�X�g���N�^
Collider::~Collider()
{
}

//���^���m�̏Փ˔���
//�����FboxA	�P�ڂ̔��^����
//�����FboxB	�Q�ڂ̔��^����
//�ߒl�F�ڐG���Ă����true
bool Collider::IsHitBoxVsBox(BoxCollider* boxA, BoxCollider* boxB)
{

	XMFLOAT3 boxPosA = Transform::Float3Add(boxA->pGameObject_->GetWorldPosition(), boxA->center_);
	XMFLOAT3 boxPosB = Transform::Float3Add(boxB->pGameObject_->GetWorldPosition(), boxB->center_);


	if ((boxPosA.x + boxA->size_.x / 2) > (boxPosB.x - boxB->size_.x / 2) &&
		(boxPosA.x - boxA->size_.x / 2) < (boxPosB.x + boxB->size_.x / 2) &&
		(boxPosA.y + boxA->size_.y / 2) > (boxPosB.y - boxB->size_.y / 2) &&
		(boxPosA.y - boxA->size_.y / 2) < (boxPosB.y + boxB->size_.y / 2) &&
		(boxPosA.z + boxA->size_.z / 2) > (boxPosB.z - boxB->size_.z / 2) &&
		(boxPosA.z - boxA->size_.z / 2) < (boxPosB.z + boxB->size_.z / 2))
	{
		return true;
	}
	return false;
}

//���^�Ƌ��̂̏Փ˔���
//�����Fbox	���^����
//�����Fsphere	�Q�ڂ̔��^����
//�ߒl�F�ڐG���Ă����true
bool Collider::IsHitBoxVsCircle(BoxCollider* box, SphereCollider* sphere)
{
	XMFLOAT3 circlePos = Transform::Float3Add(sphere->pGameObject_->GetWorldPosition(), sphere->center_);
	XMFLOAT3 boxPos = Transform::Float3Add(box->pGameObject_->GetWorldPosition(), box->center_);



	if (circlePos.x > boxPos.x - box->size_.x - sphere->size_.x &&
		circlePos.x < boxPos.x + box->size_.x + sphere->size_.x &&
		circlePos.y > boxPos.y - box->size_.y - sphere->size_.x &&
		circlePos.y < boxPos.y + box->size_.y + sphere->size_.x &&
		circlePos.z > boxPos.z - box->size_.z - sphere->size_.x &&
		circlePos.z < boxPos.z + box->size_.z + sphere->size_.x )
	{
		return true;
	}

	return false;
}

//���̓��m�̏Փ˔���
//�����FcircleA	�P�ڂ̋��̔���
//�����FcircleB	�Q�ڂ̋��̔���
//�ߒl�F�ڐG���Ă����true
bool Collider::IsHitCircleVsCircle(SphereCollider* circleA, SphereCollider* circleB)
{
	XMFLOAT3 centerA = circleA->center_;
	XMFLOAT3 positionA = circleA->pGameObject_->GetWorldPosition();
	XMFLOAT3 centerB = circleB->center_;
	XMFLOAT3 positionB = circleB->pGameObject_->GetWorldPosition();

	XMVECTOR v = (XMLoadFloat3(&centerA) + XMLoadFloat3(&positionA))
		- (XMLoadFloat3(&centerB) + XMLoadFloat3(&positionB));

	if (XMVector3Length(v).m128_f32[0] <= circleA->size_.x + circleB->size_.x)
	{
		return true;
	}

	return false;
}

//�e�X�g�\���p�̘g��`��
//�����Fposition	�I�u�W�F�N�g�̈ʒu
void Collider::Draw(XMFLOAT3 position)
{
	Transform transform;
	transform.position_ = XMFLOAT3(position.x + center_.x, position.y + center_.y, position.z + center_.z);
	transform.scale_ = size_;
	transform.Calclation();
	Model::SetTransform(hDebugModel_, transform);
	Model::Draw(hDebugModel_);
}

// 2�����̍ŒZ�����i2D�Łj
// s1 : S1(����1)
// s2 : S2(����2)
// p1 : S1���̐����̑��i�߂�l�j
// p2 : S2���̐����̑��i�߂�l�j
// t1 : S1���̃x�N�g���W���i�߂�l�j
// t2 : S2���̃x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
bool Collider::IsHitCapsuleVsCapsule(CapsuleCollider* capsule1, CapsuleCollider* capsule2)
{
/*
float calcSegmentSegmentDist2D(const Segment2D & s1, const Segment2D & s2, Point2D & p1, Point2D & p2, float& t1, float& t2) {

        // S1���k�ނ��Ă���H
        if (s1.v.lengthSq() < _OX_EPSILON_) {
            // S2���k�ށH
            if (s2.v.lengthSq() < _OX_EPSILON_) {
                // �_�Ɠ_�̋����̖��ɋA��
                float len = (s2.p - s1.p).length();
                p1 = s1.p;
                p2 = s2.p;
                t1 = t2 = 0.0f;
                return len;
            }
            else {
                // S1�̎n�_��S2�̍ŒZ���ɋA��
                float len = calcPointSegmentDist2D(s1.p, s2, p2, t2);
                p1 = s1.p;
                t1 = 0.0f;
                clamp01(t2);
                return len;
            }
        }

        // S2���k�ނ��Ă���H
        else if (s2.v.lengthSq() < _OX_EPSILON_) {
            // S2�̎n�_��S1�̍ŒZ���ɋA��
            float len = calcPointSegmentDist2D(s2.p, s1, p1, t1);
            p2 = s2.p;
            clamp01(t1);
            t2 = 0.0f;
            return len;
        }

        //�������m

        // 2���������s�������琂���̒[�_�̈��P1�ɉ�����
    if (s1.v.isParallel(s2.v) == true) {
        t1 = 0.0f;
        p1 = s1.p;
        float len = calcPointSegmentDist2D(s1.p, s2, p2, t2);
        if (0.0f <= t2 && t2 <= 1.0f)
            return len;
    }
    else {
        // �����͂˂���̊֌W
        // 2�����Ԃ̍ŒZ���������߂ĉ���t1,t2�����߂�
        float len = calcLineLineDist2D(s1, s2, p1, p2, t1, t2);
        if (
            0.0f <= t1 && t1 <= 1.0f &&
            0.0f <= t2 && t2 <= 1.0f
            ) {
            return len;
        }
    }

    // �����̑����O�ɂ��鎖������
    // S1����t1��0�`1�̊ԂɃN�����v���Đ������~�낷
    clamp01(t1);
    p1 = s1.getPoint(t1);
    float len = calcPointSegmentDist2D(p1, s2, p2, t2);
    if (0.0f <= t2 && t2 <= 1.0f)
        return len;

    // S2�����O�������̂�S2�����N�����v�AS1�ɐ������~�낷
    clamp01(t2);
    p2 = s2.getPoint(t2);
    len = calcPointSegmentDist2D(p2, s1, p1, t1);
    if (0.0f <= t1 && t1 <= 1.0f)
        return len;

    // �o���̒[�_���ŒZ�Ɣ���
    clamp01(t1);
    p1 = s1.getPoint(t1);
    return (p2 - p1).length();
}
*/
    
}

bool Collider::IsHitCircleVsCapsule(SphereCollider* circle, CapsuleCollider* capsule2)
{
    return false;
}

bool Collider::IsHitCircleVsTriangle(SphereCollider* circle, const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, XMVECTOR& outDistanceVector)
{
    XMVECTOR p = XMLoadFloat3(&circle->center_);
    XMVECTOR q0 = XMLoadFloat3(&v0);
    XMVECTOR q1 = XMLoadFloat3(&v1);
    XMVECTOR q2 = XMLoadFloat3(&v2);

    // �O�p�`�̕Ӄx�N�g�����v�Z
    XMVECTOR edge0 = q1 - q0;
    XMVECTOR edge1 = q2 - q0;

    // �O�p�`�̖@���x�N�g�����v�Z
    XMVECTOR normal = XMVector3Cross(edge0, edge1);
    normal = XMVector3Normalize(normal);

    // ���̒��S���畽�ʂւ̋������v�Z
    float distanceToPlane = XMVectorGetX(XMVector3Dot(normal, p - q0));

    // ���ʂƋ��̒��S�̋��������̔��a���傫���ꍇ�A������Ɣ���
    if (fabs(distanceToPlane) > circle->size_.x) {
        return false;
    }

    // ���̒��S����O�p�`�̕��ʂւ̐����̑�
    XMVECTOR pointOnPlane = p - distanceToPlane * normal;

    // �O�p�`���̓_���p�����g���b�N�ɕ\��
    XMVECTOR v0ToPoint = pointOnPlane - q0;
    float d00 = XMVectorGetX(XMVector3Dot(edge0, edge0));
    float d01 = XMVectorGetX(XMVector3Dot(edge0, edge1));
    float d11 = XMVectorGetX(XMVector3Dot(edge1, edge1));
    float d20 = XMVectorGetX(XMVector3Dot(v0ToPoint, edge0));
    float d21 = XMVectorGetX(XMVector3Dot(v0ToPoint, edge1));
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    // u, v, w�͈̔͂ɂ�锻��
    bool isInsideTriangle = (u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f);

    if (isInsideTriangle) {
        // ���ʏ�̓_���O�p�`�����̏ꍇ
        outDistanceVector = distanceToPlane * normal;
        return fabs(distanceToPlane) <= circle->size_.x;
    }

    // �O�p�`�̊e���_�Ƌ��̒��S�Ƃ̋������v�Z
    float distSqToV0 = XMVectorGetX(XMVector3LengthSq(p - q0));
    float distSqToV1 = XMVectorGetX(XMVector3LengthSq(p - q1));
    float distSqToV2 = XMVectorGetX(XMVector3LengthSq(p - q2));

    float minDistSq = distSqToV0;
    XMVECTOR closestPoint = q0;

    if (distSqToV1 < minDistSq) {
        minDistSq = distSqToV1;
        closestPoint = q1;
    }

    if (distSqToV2 < minDistSq) {
        minDistSq = distSqToV2;
        closestPoint = q2;
    }

    // �e�ӂƋ��̒��S�Ƃ̋������v�Z
    XMVECTOR edgeClosestPoint;
    float edgeDistSq;

    // q0q1
    edgeDistSq = XMVectorGetX(XMVector3LengthSq(XMVectorClamp(p, q0, q1) - p));
    if (edgeDistSq < minDistSq) {
        minDistSq = edgeDistSq;
        edgeClosestPoint = XMVectorClamp(p, q0, q1);
    }

    // q0q2
    edgeDistSq = XMVectorGetX(XMVector3LengthSq(XMVectorClamp(p, q0, q2) - p));
    if (edgeDistSq < minDistSq) {
        minDistSq = edgeDistSq;
        edgeClosestPoint = XMVectorClamp(p, q0, q2);
    }

    // q1q2
    edgeDistSq = XMVectorGetX(XMVector3LengthSq(XMVectorClamp(p, q1, q2) - p));
    if (edgeDistSq < minDistSq) {
        minDistSq = edgeDistSq;
        edgeClosestPoint = XMVectorClamp(p, q1, q2);
    }

    // �ŏ������̌v�Z
    float minDist = sqrtf(minDistSq);
    outDistanceVector = (minDist <= circle->size_.x) ? (p - closestPoint) : XMVectorZero();
    return minDist <= circle->size_.x;
}