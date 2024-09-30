#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "SegmentCollider.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"
#include "Global.h"
#include "../Stage/Triangle.h"

namespace {
    const float EPSILON = 0.000001f;     //���e�덷�̒�`

}

//�����i�n�܂�ƏI��肪����L���̐��j
struct Segment {
    XMFLOAT3 pos;   //�����̎n�_
    XMVECTOR vec;   //�����̕����x�N�g���i�������j
    Segment(XMFLOAT3 p, XMVECTOR v) : pos(p), vec(v) {};

    XMFLOAT3 GetEndPoint() {
        XMFLOAT3 endPoint;
        XMVECTOR endPointVec = XMVectorAdd(XMLoadFloat3(&pos), vec);
        XMStoreFloat3(&endPoint, endPointVec);
        return endPoint;
    }

    //������̓_���擾����
    XMFLOAT3 GetPosition(float dot) const {
        XMFLOAT3 p = XMFLOAT3();
        XMStoreFloat3(&p, (XMLoadFloat3(&pos) + (vec * dot)));
        return  p;
    }
};

void Clamp01(float& v);
bool IsSharpAngle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3);
float CalcPointSegmentDist(XMFLOAT3& p, Segment& seg, XMFLOAT3& h, float& t);
float CalcLineLineDist(Segment& s1, Segment& s2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2);
float CalcSegmentSegmentDist(Segment& s1, Segment& s2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2);
float CalcPointLineDist(XMFLOAT3& p, Segment& s, XMFLOAT3& h, float& t);

//�R���X�g���N�^
Collider::Collider():
    pGameObject_(nullptr), center_(XMFLOAT3()), size_(XMFLOAT3(1.0f, 1.0f, 1.0f)), hDebugModel_(-1), type_(ColliderType::COLLIDER_BOX)
{
}

//�f�X�g���N�^
Collider::~Collider()
{
}

//�e�X�g�\���p�̘g��`��
//�����Fposition	�I�u�W�F�N�g�̈ʒu
void Collider::Draw(XMFLOAT3 position)
{
#ifdef _DEBUG
    Transform transform;
    transform.position_ = XMFLOAT3(position.x + center_.x, position.y + center_.y, position.z + center_.z);
    transform.scale_ = size_;
    transform.Calclation();
    Model::SetTransform(hDebugModel_, transform);
    Model::Draw(hDebugModel_);
#endif
}

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

//�����̂Ƃ��g��Ȃ�����A�Ƃ肠�����v�������o�������Ȃ��������Ă����AAABB�̕��������
//�܂���ԋ߂��_���߂遨Box�̒��ɓ����Ă��Ȃ����̔��聨�ŋ߂�Box/Sphere���肷��
bool Collider::IsHitBoxVsCapsule(BoxCollider* box, CapsuleCollider* capsule)
{
    return false;
}

//Box/Capsule�̑O�ɂ���������čŋ߂̓��O���肾�����΂悳��
bool Collider::IsHitBoxVsSegment(BoxCollider* box, SegmentCollider* seg)
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool Collider::IsHitCircleVsCircle(SphereCollider* circleA, SphereCollider* circleB)
{
	XMFLOAT3 centerA = circleA->center_;
	XMFLOAT3 positionA = circleA->pGameObject_->GetWorldPosition();
	XMFLOAT3 centerB = circleB->center_;
	XMFLOAT3 positionB = circleB->pGameObject_->GetWorldPosition();

	XMVECTOR v = (XMLoadFloat3(&centerA) + XMLoadFloat3(&positionA)) - (XMLoadFloat3(&centerB) + XMLoadFloat3(&positionB));

	if (XMVector3Length(v).m128_f32[0] <= circleA->size_.x + circleB->size_.x)
	{
		return true;
	}

	return false;
}

bool Collider::IsHitCircleVsCapsule(SphereCollider* circle, CapsuleCollider* capsule)
{
    XMFLOAT3 center = circle->center_;
    XMFLOAT3 position = Float3Add(circle->pGameObject_->GetWorldPosition(), center);

    XMFLOAT3 capPos = Transform::Float3Add(capsule->pGameObject_->GetWorldPosition(), capsule->center_);
    XMVECTOR dir = XMVector3Normalize(capsule->direction_);
    dir *= capsule->height_;
    XMVECTOR vPos = XMLoadFloat3(&capPos) - dir * 0.5f;
    XMStoreFloat3(&capPos, vPos);
    Segment seg1 = Segment(capPos, dir);

    XMFLOAT3 hit = XMFLOAT3();
    float t = 0.0f;
    float dist = CalcPointSegmentDist(position, seg1, hit, t);
    bool b = dist < (circle->size_.x + capsule->size_.x);
    capsule->targetDit_ = dist;
    capsule->targetPos_ = hit;
    return b;
}

bool Collider::IsHitCircleVsSegment(SphereCollider* circle, SegmentCollider* seg)
{
    //���̂̒��S���W
    XMFLOAT3 f = circle->pGameObject_->GetWorldPosition();
    XMVECTOR cCenter = XMLoadFloat3(&f) + XMLoadFloat3(&circle->center_);

    //�����̎n�_�ƏI�_���W
    f = seg->pGameObject_->GetWorldPosition();
    XMVECTOR sStart = XMLoadFloat3(&f) + XMLoadFloat3(&seg->center_);
    XMVECTOR sEnd = sStart + seg->vec_ * seg->size_.x;

    //�����̏I�_�Ƌ��̋������~�͈͓̔��Ȃ瓖�����Ă�
    XMVECTOR vP = cCenter - sEnd;
    if (circle->size_.x > XMVectorGetX(XMVector3Length(vP))) return true;

    //�����̎n�_�Ƌ��̋������~�͈͓̔��Ȃ瓖�����Ă�
    vP = cCenter - sStart;
    if (circle->size_.x > XMVectorGetX(XMVector3Length(vP))) return true;

    //���ς�0���傫���A�����x�N�g���̑傫����菬�����Ȃ�
    float dot = XMVectorGetX(XMVector3Dot(seg->vec_, vP));
    if (dot > 0 && dot < seg->size_.x) {
        //dot�̒����̃x�N�g��
        XMVECTOR vec = seg->vec_ * dot;

        //���̂����ԋ߂�segment�̓_�Ƌ��̂܂ł̋��������̂̔��a�̂Q���菬������Γ������Ă�
        float range = XMVectorGetX(XMVector3Length(vP - vec));
        if (circle->size_.x > range) return true;
    }
    return false;
}

bool Collider::IsHitCircleVsTriangle(SphereCollider* circle, Triangle* triangle, XMVECTOR& outDistanceVector)
{
    XMFLOAT3 pos = Float3Add(circle->center_, circle->pGameObject_->GetWorldPosition());
    XMVECTOR p = XMLoadFloat3(&pos);

    XMVECTOR q0 = triangle->GetPosition(0);
    XMVECTOR q1 = triangle->GetPosition(1);
    XMVECTOR q2 = triangle->GetPosition(2);
    XMVECTOR normal = triangle->GetNormal();

    //���̒��S���畽�ʂւ̋������v�Z
    float distanceToPlane = XMVectorGetX(XMVector3Dot(normal, p - q0));

    //���ʂƋ��̒��S�̋��������̔��a���傫���ꍇ�A������Ɣ���
    if (fabs(distanceToPlane) > circle->size_.x) {
        return false;
    }

    // ���̒��S����O�p�`�̕��ʂւ̐����̑�
    XMVECTOR pointOnPlane = p - distanceToPlane * normal;

    //�O�p�`�̕Ӄx�N�g�����v�Z
    XMVECTOR edge0 = q1 - q0;
    XMVECTOR edge1 = q2 - q0;

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

    //���ʏ�̓_���O�p�`�������ǂ���
    if ((u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f)) {
        outDistanceVector = (circle->size_.x - distanceToPlane) * normal;
        bool output = fabs(distanceToPlane) < circle->size_.x;
        return output;
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

    // �e�ӂƋ��̒��S�Ƃ̍ŏ��������v�Z
    auto updateClosestPoint = [&](XMVECTOR a, XMVECTOR b) {
        XMVECTOR ab = b - a;
        float t = max(0.0f, min(1.0f, XMVectorGetX(XMVector3Dot(p - a, ab) / XMVector3Dot(ab, ab))));
        XMVECTOR point = a + t * ab;
        float distSq = XMVectorGetX(XMVector3LengthSq(point - p));
        if (distSq < minDistSq) {
            minDistSq = distSq;
            closestPoint = point;
        }
        };

    updateClosestPoint(q0, q1);
    updateClosestPoint(q0, q2);
    updateClosestPoint(q1, q2);

    //�ŏ����������̔��a�ȓ����ǂ���
    float minDist = sqrtf(minDistSq);
    bool hit = minDist < circle->size_.x;

    if (hit) {
        XMVECTOR vec = p - closestPoint;
        outDistanceVector = normal * (circle->size_.x - XMVectorGetX(XMVector3Length(vec)));
        //   outDistanceVector = (circle->size_.x - distanceToPlane) * normal;
    }
    else outDistanceVector = XMVectorZero();

    return hit;
}

//-------------------------------------------------------------------------------------------

bool Collider::IsHitCapsuleVsCapsule(CapsuleCollider* capsule1, CapsuleCollider* capsule2) {
    XMFLOAT3 capPos1 = Transform::Float3Add(capsule1->pGameObject_->GetWorldPosition(), capsule1->center_);
    XMFLOAT3 capPos2 = Transform::Float3Add(capsule2->pGameObject_->GetWorldPosition(), capsule2->center_);
    XMVECTOR dir1 = XMVector3Normalize(capsule1->direction_);
    XMVECTOR dir2 = XMVector3Normalize(capsule2->direction_);
    dir1 *= capsule1->height_;
    dir2 *= capsule2->height_;

    XMVECTOR vPos1 = XMLoadFloat3(&capPos1) - dir1 * 0.5f;
    XMVECTOR vPos2 = XMLoadFloat3(&capPos2) - dir2 * 0.5f;
    XMStoreFloat3(&capPos1, vPos1);
    XMStoreFloat3(&capPos2, vPos2);

    Segment seg1 = Segment(capPos1, dir1);
    Segment seg2 = Segment(capPos2, dir2);
    XMFLOAT3 p1 = XMFLOAT3(), p2 = XMFLOAT3();
    float t1 = 0.0f, t2 = 0.0f;

    float d = CalcSegmentSegmentDist(seg1, seg2, p1, p2, t1, t2);
    bool out = (d <= capsule1->size_.x + capsule2->size_.x);
    capsule1->targetDit_ = d;
    capsule1->targetPos_ = p1;
    capsule2->targetDit_ = d;
    capsule2->targetPos_ = p2;
    return out;
}

//����Ԉ���Ă�Ǝv��Todo�F����
bool Collider::IsHitCapsuleVsSegment(CapsuleCollider* capsule, SegmentCollider* seg)
{
    XMFLOAT3 capPos = Transform::Float3Add(capsule->pGameObject_->GetWorldPosition(), capsule->center_);
    XMFLOAT3 segPos = Transform::Float3Add(seg->pGameObject_->GetWorldPosition(), seg->center_);
    XMVECTOR dir1 = XMVector3Normalize(capsule->direction_);
    XMVECTOR dir2 = seg->vec_;
    dir1 *= capsule->height_;
    dir2 *= seg->size_.x;

    XMVECTOR vPos1 = XMLoadFloat3(&capPos) - dir1 * 0.5f;
    XMStoreFloat3(&capPos, vPos1);

    Segment seg1 = Segment(capPos, dir1);
    Segment seg2 = Segment(segPos, dir2);
    XMFLOAT3 p1 = XMFLOAT3(), p2 = XMFLOAT3();
    float t1 = 0.0f, t2 = 0.0f;

    float d = CalcSegmentSegmentDist(seg1, seg2, p1, p2, t1, t2);
    bool out = (d <= capsule->size_.x);
    capsule->targetDit_ = d;
    capsule->targetPos_ = p1;
    seg->targetDit_ = d;
    seg->targetPos_ = p2;

    return out;
}

//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[����Triangle�Ɏg���[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

// 0�`1�̊ԂɃN�����v
void Clamp01(float& v) {
    if (v < 0.0f) v = 0.0f;
    else if (v > 1.0f) v = 1.0f;
}

#include <math.h>

// �_�ƒ����̍ŒZ����
// p : �_
// l : ����
// h : �_���牺�낵�������̑��i�߂�l�j
// t :�x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float CalcPointLineDist(XMFLOAT3& p, Segment& s, XMFLOAT3& h, float& t) {
    t = 0.0f;
    float dvv = powf(XMVectorGetX(XMVector3Length(s.vec)), 2.0f);
    if (dvv > -EPSILON) {
        XMVECTOR v = XMLoadFloat3(&p) - XMLoadFloat3(&s.pos);
        t = XMVectorGetX(XMVector3Dot(s.vec, v)) / dvv;
    }

    h = s.GetPosition(t);
    XMVECTOR v = XMLoadFloat3(&p) - XMLoadFloat3(&h);
    return XMVectorGetX(XMVector3Length(v));
}

// ��p1p2p3�͉s�p�H
bool IsSharpAngle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3) {
    XMVECTOR v21 = XMLoadFloat3(&p1) - XMLoadFloat3(&p2);
    XMVECTOR v23 = XMLoadFloat3(&p3) - XMLoadFloat3(&p2);

    //�Q�̐��̓��ς��v�Z
    float dotProduct = XMVectorGetX(XMVector3Dot(XMVector3Normalize(v21), XMVector3Normalize(v23)));
    float length1 = XMVectorGetX(XMVector3Length(v21));
    float length2 = XMVectorGetX(XMVector3Length(v23));

    //�p�x�v�Z���ēx�ɕϊ�
    float angle = acosf(dotProduct / (length1 * length2));
    angle = XMConvertToDegrees(angle);

    //90�x��菬�����Ȃ�s�p
    return angle < 90.0f + EPSILON;
}

// �_�Ɛ����̍ŒZ����
// p : �_
// seg : ����
// h : �ŒZ�����ƂȂ�[�_�i�߂�l�j
// t : �[�_�ʒu�i t < 0: �n�_�̊O, 0 <= t <= 1: ������, t > 1: �I�_�̊O �j
// �߂�l: �ŒZ����
float CalcPointSegmentDist(XMFLOAT3& p, Segment& seg, XMFLOAT3& h, float& t) {
    XMFLOAT3 e = seg.GetEndPoint();

    // �����̒����A�����̑��̍��W�y��t���Z�o
    float len = CalcPointLineDist(p, seg, h, t);

    if (!IsSharpAngle(p, seg.pos, e)) {
        // �n�_���̊O��
        h = seg.pos;
        return XMVectorGetX(XMVector3Length(XMLoadFloat3(&seg.pos) - XMLoadFloat3(&p)));
    }
    else if (!IsSharpAngle(p, e, seg.pos)) {
        // �I�_���̊O��
        h = e;
        return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p) - XMLoadFloat3(&e)));
    }

    return len;
}

// 2�����̍ŒZ����
// l1 : L1
// l2 : L2
// p1 : L1���̐����̑��i�߂�l�j
// p2 : L2���̐����̑��i�߂�l�j
// t1 : L1���̃x�N�g���W���i�߂�l�j
// t2 : L2���̃x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float CalcLineLineDist(Segment& s1, Segment& s2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    //2���������s�����ׂ�
    //2�̃x�N�g���̊O�ς̒������`�F�b�N���A���e�덷��菬�������ǂ������m�F
    float crossLeng = XMVectorGetX(XMVector3Length(XMVector3Cross(XMVector3Normalize(s1.vec), XMVector3Normalize(s2.vec))));
    crossLeng = crossLeng * crossLeng + 0.0000001f;
    if (crossLeng < EPSILON) {
        //����1�̎n�_���璼��2�܂ł̍ŒZ�������ɋA������
        t1 = 0.0f;
        p1 = s1.pos;
        return CalcPointLineDist(s1.pos, s2, p2, t2);
    }

    //�������m�̂��݂��������ɂȂ�_�����߂�
    //����2�_�ŋ��������߂�
    XMVECTOR vecN1 = XMVector3Normalize(s1.vec);
    XMVECTOR vecN2 = XMVector3Normalize(s2.vec);
    float DV1V2 = XMVectorGetX(XMVector3Dot(s1.vec, s2.vec));
    float DV1V1 = powf(XMVectorGetX(XMVector3Length(s1.vec)), 2.0f);
    float DV2V2 = powf(XMVectorGetX(XMVector3Length(s2.vec)), 2.0f);
    XMVECTOR VP2P1 = XMLoadFloat3(&s1.pos) - XMLoadFloat3(&s2.pos);
    float t1D1 = DV1V2 * XMVectorGetX(XMVector3Dot(s2.vec, VP2P1));
    float t1D2 = DV2V2 * XMVectorGetX(XMVector3Dot(s1.vec, VP2P1));
    float t1D3 = (DV1V1 * DV2V2 - DV1V2 * DV1V2);
    
    t1 = (t1D1 - t1D2) / t1D3;
    p1 = s1.GetPosition(t1);
    t2 = XMVectorGetX(XMVector3Dot(s2.vec, (XMLoadFloat3(&p1) - XMLoadFloat3(&s2.pos)))) / DV2V2;
    p2 = s2.GetPosition(t2);
    return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p2) - XMLoadFloat3(&p1)));
}

//https://www.youtube.com/watch?v=oCg5T5Xyg-o
// 2�����̍ŒZ����
// s1 : S1(����1)
// s2 : S2(����2)
// p1 : S1���̐����̑��i�߂�l�j
// p2 : S2���̐����̑��i�߂�l�j
// t1 : S1���̃x�N�g���W���i�߂�l�j
// t2 : S2���̃x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float CalcSegmentSegmentDist(Segment& s1, Segment& s2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    float dist = 0.0f;

    //----------------------------------------------------------------
    //�Ƃ肠����2�����Ԃ̍ŒZ����,mp1,mp2,t1,t2�����߂Ă݂�
    dist = CalcLineLineDist(s1, s2, p1, p2, t1, t2);
    if (0.0f <= t1 && t1 <= 1.0f && 0.0f <= t2 && t2 <= 1.0f) {
        //mp1,mp2�������Ƃ��������ɂ�����
        return dist;
    }
    //mp1,mp2�̗����A�܂��͂ǂ��炩���������ɂȂ������̂Ŏ���

    //----------------------------------------------------------------
    //mp1,t1�����ߒ��� �� t2��0�`1�ɃN�����v����mp2����s1.v�ɐ������~�낵�Ă݂�
    Clamp01(t2);
    p2 = s2.GetPosition(t2);
    dist = CalcPointLineDist(p2, s1, p1, t1);
    if (0.0f <= t1 && t1 <= 1.0f) {
        //mp1���������ɂ�����
        return dist;
    }
    //mp1���������ɂȂ������̂Ŏ���

    //----------------------------------------------------------------
    //mp2,t2�����ߒ��� �� t1��0�`1�ɃN�����v����mp1����s2.v�ɐ������~�낵�Ă݂�
    Clamp01(t1);
    p1 = s1.GetPosition(t1);
    dist = CalcPointLineDist(p1, s2, p2, t2);
    if (0.0f <= t2 && t2 <= 1.0f) {
        //mp2���������ɂ�����
        return dist;
    }
    //mp2���������ɂȂ������̂Ŏ���

    //----------------------------------------------------------------
    //t2���N�����v����mp2���Čv�Z����ƁAmp1����mp2�܂ł��ŒZ
    Clamp01(t2);
    p2 = s2.GetPosition(t2);
    dist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&p2) - XMLoadFloat3(&p1)));
    return dist;
    //----------------------------------------------------------------

}
