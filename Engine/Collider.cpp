#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"
#include "Global.h"

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

bool Collider::IsHitBoxVsCapsule(BoxCollider* box, CapsuleCollider* capsule)
{
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

//-------------------------------------------------------------------------------------------

// ���e�덷�̒�`
const float EPSILON = 0.0001f;

//�����i�n�܂�ƏI��肪����L���̐��j
struct Segment {
    XMFLOAT3 pos;   //�����̎n�_
    XMVECTOR vec;   //�����̕����x�N�g��
    Segment(XMFLOAT3 p, XMVECTOR v) : pos(p), vec(v) {};

    XMFLOAT3 GetEndPoint() {
        XMFLOAT3 endPoint;
        XMVECTOR endPointVec = XMVectorAdd(XMLoadFloat3(&pos), vec);
        XMStoreFloat3(&endPoint, endPointVec);
        return endPoint;
    }
};

//�����i������̈�_�ƕ����x�N�g���j
struct Line {
    XMFLOAT3 pos;   //�n
    XMVECTOR vec;   //���̕����x�N�g��
    Line(XMFLOAT3 p, XMVECTOR v) : pos(p), vec(v) {};
    
    //������̓_���擾����
    XMFLOAT3 GetPosition(float dot) const {
        XMFLOAT3 pos = XMFLOAT3();
        XMStoreFloat3(&pos, (XMLoadFloat3(&pos) + (vec * dot)) );
        return  pos;
    }
};

// 0�`1�̊ԂɃN�����v
void Clamp01(float& v) {
    if (v < 0.0f) v = 0.0f;
    else if (v > 1.0f) v = 1.0f;
}

// �_�ƒ����̍ŒZ����
// p : �_
// l : ����
// h : �_���牺�낵�������̑��i�߂�l�j
// t :�x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float CalcPointLineDist(XMFLOAT3& p, Line& l, XMFLOAT3& h, float& t) {
    float lenSqV = XMVectorGetX(XMVector3LengthSq(l.vec));
    t = 0.0f;
    if (lenSqV > (0.0f - EPSILON)) {
        t = XMVectorGetX(XMVector3Dot(XMVector3Normalize(l.vec), XMVector3Normalize(XMLoadFloat3(&p) - XMLoadFloat3(&l.pos)))) / lenSqV;
    }

    XMVECTOR pointOnLine = XMLoadFloat3(&l.pos) + (t * l.vec);
    XMStoreFloat3(&h, pointOnLine);
    return XMVectorGetX(XMVector3Length(pointOnLine - XMLoadFloat3(&p)));
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
    Line line = Line(seg.pos, XMLoadFloat3(&e) - XMLoadFloat3(&seg.pos));
    float len = CalcPointLineDist(p, line, h, t);

    if (!IsSharpAngle(p, seg.pos, e)) {
        h = seg.pos;
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&p), XMLoadFloat3(&seg.pos))));
    }
    else if (!IsSharpAngle(p, e, seg.pos)) {
        h = e;
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&p), XMLoadFloat3(&e))));
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
float CalcLineLineDist(Line& l1, Line& l2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    //2���������s�����ׂ�
    //2�̃x�N�g���̊O�ς̒������`�F�b�N���A���e�덷��菬�������ǂ������m�F
    XMVECTOR crossProduct = XMVector3Cross(l1.vec, l2.vec);
    float crossLength = XMVectorGetX(XMVector3Length(crossProduct));
    if (std::abs(crossLength) < EPSILON) {
        // �_P11�ƒ���L2�̍ŒZ�����̖��ɋA��
        float len = CalcPointLineDist(l1.pos, l2, p2, t2);
        p1 = l1.pos;
        t1 = 0.0f;
        return len;
    }

    //2�����͂˂���̊֌W
    float DV1V2 = XMVectorGetX(XMVector3Dot(XMVector3Normalize(l1.vec), XMVector3Normalize(l2.vec)));

    OutputDebugStringA(std::to_string(DV1V2).c_str());
    OutputDebugString(" , ");

    float DV1V1 = XMVectorGetX(XMVector3LengthSq(l1.vec));
    float DV2V2 = XMVectorGetX(XMVector3LengthSq(l2.vec));
    XMVECTOR P21P11 = XMLoadFloat3(&l1.pos) - XMLoadFloat3(&l2.pos);

    t1 = (DV1V2 * XMVectorGetX(XMVector3Dot(XMVector3Normalize(l2.vec), XMVector3Normalize(P21P11))) 
        - DV2V2 * XMVectorGetX(XMVector3Dot(XMVector3Normalize(l1.vec), P21P11)) / (DV1V1 * DV2V2 - DV1V2 * DV1V2));
    p1 = l1.GetPosition(t1);
    t2 = XMVectorGetX(XMVector3Dot(XMVector3Normalize(l2.vec), XMVector3Normalize((XMLoadFloat3(&p1) - XMLoadFloat3(&l2.pos))))) / DV2V2;
    p2 = l2.GetPosition(t2);
    return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p2) - XMLoadFloat3(&p1)));
}

// 2�����̍ŒZ����
// s1 : S1(����1)
// s2 : S2(����2)
// p1 : S1���̐����̑��i�߂�l�j
// p2 : S2���̐����̑��i�߂�l�j
// t1 : S1���̃x�N�g���W���i�߂�l�j
// t2 : S2���̃x�N�g���W���i�߂�l�j
// �߂�l: �ŒZ����
float CalcSegmentSegmentDist(Segment& s1, Segment& s2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    
    // S1���k�ނ��Ă���H
    // (�k�ށF�����̗��[����v���Ĉ�_�ɂȂ�A�����ƌĂׂȂ��Ȃ���)
    if (XMVectorGetX(XMVector3LengthSq(s1.vec)) < EPSILON) {
        if (XMVectorGetX(XMVector3LengthSq(s2.vec)) < EPSILON) {
            float len = XMVectorGetX(XMVector3Length(XMLoadFloat3(&s2.pos) - XMLoadFloat3(&s1.pos)));
            p1 = s1.pos;
            p2 = s2.pos;
            t1 = t2 = 0.0f;
            OutputDebugString("1\t");
            return len;
        }
        else {
            float len = CalcPointSegmentDist(s1.pos, s2, p2, t2);
            p1 = s1.pos;
            t1 = 0.0f;
            Clamp01(t2);
            OutputDebugString("2\t");
            return len;
        }
    }
    // S2���k�ނ��Ă���H
    else if (XMVectorGetX(XMVector3LengthSq(s2.vec)) < EPSILON) {
        float len = CalcPointSegmentDist(s2.pos, s1, p1, t1);
        p2 = s2.pos;
        Clamp01(t1);
        t2 = 0.0f; 
        OutputDebugString("3\t");
        return len;
    }

    /* �������m */
    // 2���������s�������琂���̒[�_�̈��P1�ɉ�����
    float vecDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(s1.vec), XMVector3Normalize(s2.vec)));
    if (vecDot == 1.0f) {
        t1 = 0.0f;
        p1 = s1.pos;
        float len = CalcPointSegmentDist(s1.pos, s2, p2, t2);
        if (t2 >= 0.0f - EPSILON && t2 <= 1.0f + EPSILON) {
            OutputDebugString("4\t");
            return len;
        }
    }
    else {
        // �����͂˂���̊֌W�i�˂���F���s�łȂ������Ȃ�2�̒����̈ʒu�֌W�j
        // 2�����Ԃ̍ŒZ���������߂ĉ���t1,t2�����߂�
        Line l1 = Line(s1.pos, s1.vec);
        Line l2 = Line(s2.pos, s2.vec);
        float len = CalcLineLineDist(l1, l2, p1, p2, t1, t2);
        if (0.0f <= t1 && t1 <= 1.0f && 0.0f <= t2 && t2 <= 1.0f) {
            OutputDebugString("5\t");
            return len;
        }
    }

    // �����̑����O�ɂ��鎖������
    // S1����t1��0�`1�̊ԂɃN�����v���Đ������~�낷
    Clamp01(t1);
    XMVECTOR pointOnS1 = XMLoadFloat3(&s1.pos) + (s1.vec * t1);
    XMStoreFloat3(&p1, pointOnS1);
    float len = CalcPointSegmentDist(p1, s2, p2, t2);
    if (t2 >= -EPSILON && t2 <= 1.0f + EPSILON) {
        OutputDebugString("6\t");
        return len;
    }

    // S2�����O�������̂�S2�����N�����v�AS1�ɐ������~�낷
    Clamp01(t2);
    XMVECTOR pointOnS2 = XMLoadFloat3(&s2.pos) + (s2.vec * t2);
    XMStoreFloat3(&p2, pointOnS2);
    len = CalcPointSegmentDist(p2, s1, p1, t1);
    if (0.0f <= t1 && t1 <= 1.0f) {
        OutputDebugString("7\t");
        return len;
    }

    // �o���̒[�_���ŒZ�Ɣ���
    XMStoreFloat3(&p1, pointOnS1);
    XMStoreFloat3(&p2, pointOnS2);
    OutputDebugString("8\t");
    float d = XMVectorGetX(XMVector3Length(pointOnS2 - pointOnS1));
    return d;
}

bool Collider::IsHitCapsuleVsCapsule(CapsuleCollider* capsule1, CapsuleCollider* capsule2) {
    XMFLOAT3 p1 = XMFLOAT3(), p2 = XMFLOAT3();
    float t1 = 0.0f,t2 = 0.0f;

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

    float d = CalcSegmentSegmentDist(seg1, seg2, p1, p2, t1, t2);
    bool out = (d <= capsule1->size_.x + capsule2->size_.x);
    if (out) OutputDebugStringA(("Capsule    hit" + std::to_string(d) + "\n").c_str());
    else OutputDebugStringA(("Capsule no hit" + std::to_string(d) + "\n").c_str());

    capsule1->targetDit_ = d;
    capsule2->targetDit_ = d;

    return out;
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