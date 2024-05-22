#include "BoxCollider.h"
#include "SphereCollider.h"
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

// 2線分の最短距離（2D版）
// s1 : S1(線分1)
// s2 : S2(線分2)
// p1 : S1側の垂線の足（戻り値）
// p2 : S2側の垂線の足（戻り値）
// t1 : S1側のベクトル係数（戻り値）
// t2 : S2側のベクトル係数（戻り値）
// 戻り値: 最短距離
bool Collider::IsHitCapsuleVsCapsule(CapsuleCollider* capsule1, CapsuleCollider* capsule2)
{
/*
float calcSegmentSegmentDist2D(const Segment2D & s1, const Segment2D & s2, Point2D & p1, Point2D & p2, float& t1, float& t2) {

        // S1が縮退している？
        if (s1.v.lengthSq() < _OX_EPSILON_) {
            // S2も縮退？
            if (s2.v.lengthSq() < _OX_EPSILON_) {
                // 点と点の距離の問題に帰着
                float len = (s2.p - s1.p).length();
                p1 = s1.p;
                p2 = s2.p;
                t1 = t2 = 0.0f;
                return len;
            }
            else {
                // S1の始点とS2の最短問題に帰着
                float len = calcPointSegmentDist2D(s1.p, s2, p2, t2);
                p1 = s1.p;
                t1 = 0.0f;
                clamp01(t2);
                return len;
            }
        }

        // S2が縮退している？
        else if (s2.v.lengthSq() < _OX_EPSILON_) {
            // S2の始点とS1の最短問題に帰着
            float len = calcPointSegmentDist2D(s2.p, s1, p1, t1);
            p2 = s2.p;
            clamp01(t1);
            t2 = 0.0f;
            return len;
        }

        //線分同士

        // 2線分が平行だったら垂線の端点の一つをP1に仮決定
    if (s1.v.isParallel(s2.v) == true) {
        t1 = 0.0f;
        p1 = s1.p;
        float len = calcPointSegmentDist2D(s1.p, s2, p2, t2);
        if (0.0f <= t2 && t2 <= 1.0f)
            return len;
    }
    else {
        // 線分はねじれの関係
        // 2直線間の最短距離を求めて仮のt1,t2を求める
        float len = calcLineLineDist2D(s1, s2, p1, p2, t1, t2);
        if (
            0.0f <= t1 && t1 <= 1.0f &&
            0.0f <= t2 && t2 <= 1.0f
            ) {
            return len;
        }
    }

    // 垂線の足が外にある事が判明
    // S1側のt1を0～1の間にクランプして垂線を降ろす
    clamp01(t1);
    p1 = s1.getPoint(t1);
    float len = calcPointSegmentDist2D(p1, s2, p2, t2);
    if (0.0f <= t2 && t2 <= 1.0f)
        return len;

    // S2側が外だったのでS2側をクランプ、S1に垂線を降ろす
    clamp01(t2);
    p2 = s2.getPoint(t2);
    len = calcPointSegmentDist2D(p2, s1, p1, t1);
    if (0.0f <= t1 && t1 <= 1.0f)
        return len;

    // 双方の端点が最短と判明
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