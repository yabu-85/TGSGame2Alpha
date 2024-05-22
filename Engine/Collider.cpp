#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"

//コンストラクタ
Collider::Collider():
    pGameObject_(nullptr), center_(XMFLOAT3()), size_(XMFLOAT3(1.0f, 1.0f, 1.0f))
{
}

//デストラクタ
Collider::~Collider()
{
}

//箱型同士の衝突判定
//引数：boxA	１つ目の箱型判定
//引数：boxB	２つ目の箱型判定
//戻値：接触していればtrue
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

//箱型と球体の衝突判定
//引数：box	箱型判定
//引数：sphere	２つ目の箱型判定
//戻値：接触していればtrue
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

//球体同士の衝突判定
//引数：circleA	１つ目の球体判定
//引数：circleB	２つ目の球体判定
//戻値：接触していればtrue
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

//テスト表示用の枠を描画
//引数：position	オブジェクトの位置
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
const float _OX_EPSILON_ = 0.0001f;

//線分（始まりと終わりがある有限の線）
struct Segment {
    XMFLOAT3 pos;   //線分の始点
    XMVECTOR vec;   //線分の方向ベクトル
    Segment(XMFLOAT3 p, XMVECTOR v) : pos(p), vec(v) {};
    XMFLOAT3 GetEndPoint() {
        XMFLOAT3 endPoint;
        XMVECTOR endPointVec = XMVectorAdd(XMLoadFloat3(&pos), vec);
        XMStoreFloat3(&endPoint, endPointVec);
        return endPoint;
    }
};

//半直線（始まりと向きだけの無限の線）
struct Line {
    XMFLOAT3 pos;   //線の始点
    XMVECTOR vec;   //線の方向ベクトル
    Line(XMFLOAT3 p, XMVECTOR v) : pos(p), vec(v) {};
};

// 0～1の間にクランプ
void Clamp01(float& v) {
    if (v < 0.0f)
        v = 0.0f;
    else if (v > 1.0f)
        v = 1.0f;
}

// 点と直線の最短距離
float CalcPointLineDist(XMFLOAT3& p, Line& l, XMFLOAT3& h, float& t) {
    XMVECTOR pVec = XMLoadFloat3(&p);
    XMVECTOR p0 = XMLoadFloat3(&l.pos);
    XMVECTOR direction = l.vec;

    float lenSqV = XMVectorGetX(XMVector3LengthSq(direction));
    t = 0.0f;
    if (lenSqV > 0.0f)
        t = XMVectorGetX(XMVector3Dot(XMVectorSubtract(pVec, p0), direction)) / lenSqV;

    XMVECTOR pointOnLine = XMVectorAdd(p0, XMVectorScale(direction, t));
    XMStoreFloat3(&h, pointOnLine);

    return XMVectorGetX(XMVector3Length(XMVectorSubtract(pointOnLine, pVec)));
}

bool IsSharpAngle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3) {
    XMVECTOR v1 = XMLoadFloat3(&p1);
    XMVECTOR v2 = XMLoadFloat3(&p2);
    XMVECTOR v3 = XMLoadFloat3(&p3);
    XMVECTOR v21 = XMVectorSubtract(v1, v2);
    XMVECTOR v23 = XMVectorSubtract(v3, v2);

    //２つの線の内積を計算
    float dotProduct = XMVectorGetX(XMVector3Dot(v21, v23));
    float length1 = XMVectorGetX(XMVector3Length(v21));
    float length2 = XMVectorGetX(XMVector3Length(v23));

    //角度計算して度に変換
    float angle = acosf(dotProduct / (length1 * length2));
    angle = XMConvertToDegrees(angle);

    //90度より小さいなら鋭角
    return angle < 90.0f;
}

float CalcPointSegmentDist(XMFLOAT3& p, Segment& seg, XMFLOAT3& h, float& t) {
    XMFLOAT3 e = seg.GetEndPoint();
    XMVECTOR v = XMLoadFloat3(&seg.pos);
    XMVECTOR w = XMLoadFloat3(&e);
    XMVECTOR point = XMLoadFloat3(&p);
    XMVECTOR direction = XMVectorSubtract(w, v);
    Line line = Line(seg.pos, direction);
    float len = CalcPointLineDist(p, line, h, t);

    if (!IsSharpAngle(p, seg.pos, e)) {
        h = seg.pos;
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(point, XMLoadFloat3(&seg.pos))));
    }
    else if (!IsSharpAngle(p, e, seg.pos)) {
        h = e;
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(point, XMLoadFloat3(&e))));
    }

    return len;
}

// 2直線の最短距離
// 2線分の最短距離
// s1 : S1(線分1)
// s2 : S2(線分2)
// p1 : S1側の垂線の足（戻り値）
// p2 : S2側の垂線の足（戻り値）
// t1 : S1側のベクトル係数（戻り値）
// t2 : S2側のベクトル係数（戻り値）
// 戻り値: 最短距離
float CalcLineLineDist(const Line& l1, const Line& l2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    XMVECTOR v1 = l1.vec;
    XMVECTOR v2 = l2.vec;
    XMVECTOR p0MinusP1 = XMVectorSubtract(XMLoadFloat3(&l1.pos), XMLoadFloat3(&l2.pos));

    float DV1V2 = XMVectorGetX(XMVector3Dot(v1, v2));
    float DV1V1 = XMVectorGetX(XMVector3LengthSq(v1));
    float DV2V2 = XMVectorGetX(XMVector3LengthSq(v2));

    t1 = (DV1V2 * XMVectorGetX(XMVector3Dot(v2, p0MinusP1)) - DV2V2 * XMVectorGetX(XMVector3Dot(v1, p0MinusP1))) / (DV1V1 * DV2V2 - DV1V2 * DV1V2);
    t2 = XMVectorGetX(XMVector3Dot(XMVectorSubtract(XMLoadFloat3(&l1.pos), XMLoadFloat3(&l2.pos)), v2)) / DV2V2;

    XMVECTOR pointOnLine1 = XMVectorAdd(XMLoadFloat3(&l1.pos), XMVectorScale(v1, t1));
    XMVECTOR pointOnLine2 = XMVectorAdd(XMLoadFloat3(&l2.pos), XMVectorScale(v2, t2));

    XMStoreFloat3(&p1, pointOnLine1);
    XMStoreFloat3(&p2, pointOnLine2);

    return XMVectorGetX(XMVector3Length(XMVectorSubtract(pointOnLine2, pointOnLine1)));
}

// 2線分の最短距離
float CalcSegmentSegmentDist(Segment& s1, Segment& s2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    if (XMVectorGetX(XMVector3LengthSq(s1.vec)) < _OX_EPSILON_) {
        if (XMVectorGetX(XMVector3LengthSq(s2.vec)) < _OX_EPSILON_) {
            float len = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&s2.pos), XMLoadFloat3(&s1.pos))));
            p1 = s1.pos;
            p2 = s2.pos;
            t1 = t2 = 0.0f;
            return len;
        }
        else {
            float len = CalcPointSegmentDist(s1.pos, s2, p2, t2);
            p1 = s1.pos;
            t1 = 0.0f;
            Clamp01(t2);
            return len;
        }
    }
    else if (XMVectorGetX(XMVector3LengthSq(s2.vec)) < _OX_EPSILON_) {
        float len = CalcPointSegmentDist(s2.pos, s1, p1, t1);
        p2 = s2.pos;
        Clamp01(t1);
        t2 = 0.0f;
        return len;
    }

    if (XMVectorGetX(XMVector3Dot(s1.vec, s2.vec)) == 1.0f) {
        t1 = 0.0f;
        p1 = s1.pos;
        float len = CalcPointSegmentDist(s1.pos, s2, p2, t2);
        if (0.0f <= t2 && t2 <= 1.0f) return len;
    }
    else {
        Line l1 = Line(s1.pos, s1.vec);
        Line l2 = Line(s2.pos, s2.vec);
        float len = CalcLineLineDist(l1, l2, p1, p2, t1, t2);
        if (0.0f <= t1 && t1 <= 1.0f && 0.0f <= t2 && t2 <= 1.0f) return len;
    }

    Clamp01(t1);
    XMVECTOR pointOnS1 = XMVectorAdd(XMLoadFloat3(&s1.pos), XMVectorScale(s1.vec, t1));
    XMStoreFloat3(&p1, pointOnS1);
    float len = CalcPointSegmentDist(p1, s2, p2, t2);
    if (0.0f <= t2 && t2 <= 1.0f) return len;

    Clamp01(t2);
    XMVECTOR pointOnS2 = XMVectorAdd(XMLoadFloat3(&s2.pos), XMVectorScale(s2.vec, t2));
    XMStoreFloat3(&p2, pointOnS2);
    len = CalcPointSegmentDist(p2, s1, p1, t1);
    if (0.0f <= t1 && t1 <= 1.0f) return len;

    XMStoreFloat3(&p1, pointOnS1);
    XMStoreFloat3(&p2, pointOnS2);
    return XMVectorGetX(XMVector3Length(XMVectorSubtract(pointOnS2, pointOnS1)));
}

bool Collider::IsHitCapsuleVsCapsule(CapsuleCollider* capsule1, CapsuleCollider* capsule2) {
    XMFLOAT3 p1, p2;
    float t1, t2;

    XMFLOAT3 capPos1 = Transform::Float3Add(capsule1->pGameObject_->GetWorldPosition(), capsule1->center_);
    XMFLOAT3 capPos2 = Transform::Float3Add(capsule2->pGameObject_->GetWorldPosition(), capsule2->center_);
    Segment seg1 = Segment(capPos1, capsule1->direction_);
    Segment seg2 = Segment(capPos2, capsule2->direction_);
    float d = CalcSegmentSegmentDist(seg1, seg2, p1, p2, t1, t2);
    bool out = (d <= capsule1->size_.x + capsule2->size_.x);
    if (out) {
        int a = 0;
    }
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

    // 三角形の辺ベクトルを計算
    XMVECTOR edge0 = q1 - q0;
    XMVECTOR edge1 = q2 - q0;

    // 三角形の法線ベクトルを計算
    XMVECTOR normal = XMVector3Cross(edge0, edge1);
    normal = XMVector3Normalize(normal);

    // 球の中心から平面への距離を計算
    float distanceToPlane = XMVectorGetX(XMVector3Dot(normal, p - q0));

    // 平面と球の中心の距離が球の半径より大きい場合、非交差と判定
    if (fabs(distanceToPlane) > circle->size_.x) {
        return false;
    }

    // 球の中心から三角形の平面への垂線の足
    XMVECTOR pointOnPlane = p - distanceToPlane * normal;

    // 三角形内の点をパラメトリックに表現
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

    // u, v, wの範囲による判定
    bool isInsideTriangle = (u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f);

    if (isInsideTriangle) {
        // 平面上の点が三角形内部の場合
        outDistanceVector = distanceToPlane * normal;
        return fabs(distanceToPlane) <= circle->size_.x;
    }

    // 三角形の各頂点と球の中心との距離を計算
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

    // 各辺と球の中心との距離を計算
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

    // 最小距離の計算
    float minDist = sqrtf(minDistSq);
    outDistanceVector = (minDist <= circle->size_.x) ? (p - closestPoint) : XMVectorZero();
    return minDist <= circle->size_.x;
}