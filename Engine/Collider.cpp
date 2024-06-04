#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "SegmentCollider.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"
#include "Global.h"
#include "../Stage/Triangle.h"

//コンストラクタ
Collider::Collider():
    pGameObject_(nullptr), center_(XMFLOAT3()), size_(XMFLOAT3(1.0f, 1.0f, 1.0f)), hDebugModel_(-1)
{
}

//デストラクタ
Collider::~Collider()
{
}

//テスト表示用の枠を描画
//引数：position	オブジェクトの位置
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

bool Collider::IsHitBoxVsSegment(BoxCollider* box, SegmentCollider* seg)
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

	XMVECTOR v = (XMLoadFloat3(&centerA) + XMLoadFloat3(&positionA)) - (XMLoadFloat3(&centerB) + XMLoadFloat3(&positionB));

	if (XMVector3Length(v).m128_f32[0] <= circleA->size_.x + circleB->size_.x)
	{
		return true;
	}

	return false;
}

bool Collider::IsHitCircleVsSegment(SphereCollider* circle, SegmentCollider* seg)
{
    //球体の中心座標
    XMFLOAT3 f = circle->pGameObject_->GetWorldPosition();
    XMVECTOR cCenter = XMLoadFloat3(&f) + XMLoadFloat3(&circle->center_);

    //線分の始点と終点座標
    f = seg->pGameObject_->GetWorldPosition();
    XMVECTOR sStart = XMLoadFloat3(&f) + XMLoadFloat3(&seg->center_);
    XMVECTOR sEnd = sStart + seg->vec_ * seg->size_.x;

    //線分の終点と球の距離が円の範囲内なら当たってる
    XMVECTOR vP = cCenter - sEnd;
    if (circle->size_.x > XMVectorGetX(XMVector3Length(vP))) return true;

    //線分の始点と球の距離が円の範囲内なら当たってる
    vP = cCenter - sStart;
    if (circle->size_.x > XMVectorGetX(XMVector3Length(vP))) return true;

    //内積が0より大きく、線分ベクトルの大きさより小さいなら
    float dot = XMVectorGetX(XMVector3Dot(seg->vec_, vP));
    if (dot > 0 && dot < seg->size_.x) {
        //dotの長さのベクトル
        XMVECTOR vec = seg->vec_ * dot;

        //球体から一番近いsegmentの点と球体までの距離が球体の半径の２乗より小さければ当たってる
        float range = XMVectorGetX(XMVector3Length(vP - vec));
        if (circle->size_.x > range) return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------

// 許容誤差の定義
const float EPSILON = 0.00001f;

// 0～1の間にクランプ
void Clamp01(float& v) {
    if (v < 0.0f) v = 0.0f;
    else if (v > 1.0f) v = 1.0f;
}

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
    
    //直線上の点を取得する
    XMFLOAT3 GetPosition(float dot) const {
        XMFLOAT3 p = XMFLOAT3();
        XMStoreFloat3(&p, (XMLoadFloat3(&pos) + (vec * dot)));
        return  p;
    }
};

//直線（直線上の一点と方向ベクトル）
struct Line {
    XMFLOAT3 pos;   //始
    XMVECTOR vec;   //線の方向ベクトル
    Line(XMFLOAT3 p, XMVECTOR v) : pos(p), vec(v) {};
    
    //直線上の点を取得する
    XMFLOAT3 GetPosition(float dot) const {
        XMFLOAT3 p = XMFLOAT3();
        XMStoreFloat3(&p, (XMLoadFloat3(&pos) + (vec * dot)) );
        return  p;
    }
};

// 点と直線の最短距離
// p : 点
// l : 直線
// h : 点から下ろした垂線の足（戻り値）
// t :ベクトル係数（戻り値）
// 戻り値: 最短距離
float CalcPointLineDist(XMFLOAT3& p, Line& l, XMFLOAT3& h, float& t) {
    float lenSqV = XMVectorGetX(XMVector3LengthSq(l.vec));
    t = 0.0f;
    if (lenSqV > (-EPSILON)) {
        XMVECTOR v = XMLoadFloat3(&p) - XMLoadFloat3(&l.pos);
        t = XMVectorGetX(XMVector3Dot(l.vec, v)) / lenSqV;
    }

    XMVECTOR pointOnLine = XMLoadFloat3(&l.pos) + (t * l.vec);
    XMStoreFloat3(&h, pointOnLine);
    return abs(XMVectorGetX(XMVector3Length(pointOnLine - XMLoadFloat3(&p))));
}

// ∠p1p2p3は鋭角？
bool IsSharpAngle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3) {
    XMVECTOR v21 = XMLoadFloat3(&p1) - XMLoadFloat3(&p2);
    XMVECTOR v23 = XMLoadFloat3(&p3) - XMLoadFloat3(&p2);

    //２つの線の内積を計算
    float dotProduct = XMVectorGetX(XMVector3Dot(XMVector3Normalize(v21), XMVector3Normalize(v23)));
    float length1 = XMVectorGetX(XMVector3Length(v21));
    float length2 = XMVectorGetX(XMVector3Length(v23));

    //角度計算して度に変換
    float angle = acosf(dotProduct / (length1 * length2));
    angle = XMConvertToDegrees(angle);

    //90度より小さいなら鋭角
    return angle < 90.0f + EPSILON;
}

// 点と線分の最短距離
// p : 点
// seg : 線分
// h : 最短距離となる端点（戻り値）
// t : 端点位置（ t < 0: 始点の外, 0 <= t <= 1: 線分内, t > 1: 終点の外 ）
// 戻り値: 最短距離
float CalcPointSegmentDist(XMFLOAT3& p, Segment& seg, XMFLOAT3& h, float& t) {
    XMFLOAT3 e = seg.GetEndPoint();

    // 垂線の長さ、垂線の足の座標及びtを算出
    Line line = Line(seg.pos, XMLoadFloat3(&e) - XMLoadFloat3(&seg.pos));
    float len = CalcPointLineDist(p, line, h, t);

    if (!IsSharpAngle(p, seg.pos, e)) {
        // 始点側の外側
        h = seg.pos;
        return XMVectorGetX(XMVector3Length(XMLoadFloat3(&seg.pos) - XMLoadFloat3(&p)));
    }
    else if (!IsSharpAngle(p, e, seg.pos)) {
        // 終点側の外側
        h = e;
        return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p) - XMLoadFloat3(&e)));
    }

    return len;
}


// 2直線の最短距離
// l1 : L1
// l2 : L2
// p1 : L1側の垂線の足（戻り値）
// p2 : L2側の垂線の足（戻り値）
// t1 : L1側のベクトル係数（戻り値）
// t2 : L2側のベクトル係数（戻り値）
// 戻り値: 最短距離
float CalcLineLineDist(Line& l1, Line& l2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    //2直線が平行か調べる
    //2つのベクトルの外積の長さをチェックし、許容誤差より小さいかどうかを確認
    float crossLength = XMVectorGetX(XMVector3Length(
        XMVector3Cross(XMVector3Normalize(l1.vec), XMVector3Normalize(l2.vec))));
    if (crossLength < EPSILON) {
        // 点P11と直線L2の最短距離の問題に帰着
        float len = CalcPointLineDist(l1.pos, l2, p2, t2);
        p1 = l1.pos;
        t1 = 0.0f;
        return len;
    }

    XMVECTOR vecN1 = XMVector3Normalize(l1.vec);
    XMVECTOR vecN2 = XMVector3Normalize(l2.vec);

    XMVECTOR w0 = XMLoadFloat3(&l1.pos) - XMLoadFloat3(&l2.pos);
    float a = XMVectorGetX(XMVector3Dot(vecN1, vecN1));
    float b = XMVectorGetX(XMVector3Dot(vecN1, vecN2));
    float c = XMVectorGetX(XMVector3Dot(vecN2, vecN2));
    float d = XMVectorGetX(XMVector3Dot(vecN1, w0));
    float e = XMVectorGetX(XMVector3Dot(vecN2, w0));
    float denom = a * c - b * b;
    t1 = (b * e - c * d) / denom;
    t2 = (a * e - b * d) / denom;
    p1 = l1.GetPosition(t1);
    p2 = l2.GetPosition(t2);
    return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p2) - XMLoadFloat3(&p1)));


    //2直線はねじれの関係
    float DV1V2 = XMVectorGetX(XMVector3Dot(vecN1, vecN2));
    float DV1V1 = XMVectorGetX(XMVector3Length(l1.vec));
    float DV2V2 = XMVectorGetX(XMVector3Length(l2.vec));
    XMVECTOR P21P11 = XMLoadFloat3(&l1.pos) - XMLoadFloat3(&l2.pos);

    float t1D1 = DV1V2 * XMVectorGetX(XMVector3Dot(vecN2, P21P11));
    float t1D2 = DV2V2 * XMVectorGetX(XMVector3Dot(vecN1, P21P11));
    float t1D3 = (DV1V1 * DV2V2 - DV1V2 * DV1V2);
    
    t1 = t1D1 - t1D2 / t1D3;
    p1 = l1.GetPosition(t1);
    t2 = XMVectorGetX(XMVector3Dot(vecN2, (XMLoadFloat3(&p1) - XMLoadFloat3(&l2.pos)))) / DV2V2;
    p2 = l2.GetPosition(t2);
    return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p2) - XMLoadFloat3(&p1)));
}

// 2線分の最短距離
// s1 : S1(線分1)
// s2 : S2(線分2)
// p1 : S1側の垂線の足（戻り値）
// p2 : S2側の垂線の足（戻り値）
// t1 : S1側のベクトル係数（戻り値）
// t2 : S2側のベクトル係数（戻り値）
// 戻り値: 最短距離
float CalcSegmentSegmentDist(Segment& s1, Segment& s2, XMFLOAT3& p1, XMFLOAT3& p2, float& t1, float& t2) {
    
    // S1が縮退している？
    // (縮退：線分の両端が一致して一点になり、線分と呼べなくなった)
    if (XMVectorGetX(XMVector3LengthSq(s1.vec)) < EPSILON) {
        if (XMVectorGetX(XMVector3LengthSq(s2.vec)) < EPSILON) {
            float len = XMVectorGetX(XMVector3Length(XMLoadFloat3(&s2.pos) - XMLoadFloat3(&s1.pos)));
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
    // S2が縮退している？
    else if (XMVectorGetX(XMVector3LengthSq(s2.vec)) < EPSILON) {
        float len = CalcPointSegmentDist(s2.pos, s1, p1, t1);
        p2 = s2.pos;
        Clamp01(t1);
        t2 = 0.0f; 
        return len;
    }

    /* 線分同士 */
    // 2線分が平行だったら垂線の端点の一つをP1に仮決定
    float vecDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(s1.vec), XMVector3Normalize(s2.vec)));
    if (vecDot == 1.0f) {
        t1 = 0.0f;
        p1 = s1.pos;
        float len = CalcPointSegmentDist(s1.pos, s2, p2, t2);
        float t = abs(t2);

        if (t2 >= -EPSILON && t2 <= 1.0f + EPSILON) {
            return len;
        }
    }
    else {
        // 線分はねじれの関係（ねじれ：平行でなく交わらない2つの直線の位置関係）
        // 2直線間の最短距離を求めて仮のt1,t2を求める
        Line l1 = Line(s1.pos, s1.vec);
        Line l2 = Line(s2.pos, s2.vec);
        float len = CalcLineLineDist(l1, l2, p1, p2, t1, t2);
        if (0.0f <= t1 && t1 <= 1.0f && 0.0f <= t2 && t2 <= 1.0f) {
            return len;
        }
    }

    // 垂線の足が外にある事が判明
    // S1側のt1を0～1の間にクランプして垂線を降ろす
    Clamp01(t1);
    p1 = s1.GetPosition(t1);
    float len = CalcPointSegmentDist(p1, s2, p2, t2);
    if (t2 >= -EPSILON && t2 <= 1.0f + EPSILON) {
        return len;
    }

    // S2側が外だったのでS2側をクランプ、S1に垂線を降ろす
    Clamp01(t2);
    p2 = s2.GetPosition(t2);
    len = CalcPointSegmentDist(p2, s1, p1, t1);
    if (t1 >= -EPSILON && t1 <= 1.0f + EPSILON) {
        return len;
    }

    Clamp01(t1);
    p1 = s1.GetPosition(t1);
    return XMVectorGetX(XMVector3Length(XMLoadFloat3(&p2) - XMLoadFloat3(&p1)));
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
    capsule1->targetDit_ = d;
    capsule2->targetDit_ = d;
    return out;
}

bool Collider::IsHitCapsuleVsSegment(CapsuleCollider* capsule, SegmentCollider* seg)
{
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
    return b;
}

bool Collider::IsHitCircleVsTriangle(SphereCollider* circle, Triangle* triangle, XMVECTOR& outDistanceVector)
{
    XMFLOAT3 pos = Float3Add(circle->center_, circle->pGameObject_->GetWorldPosition());
    XMVECTOR p = XMLoadFloat3(&pos);

    XMVECTOR q0 = triangle->GetPosition(0);
    XMVECTOR q1 = triangle->GetPosition(1);
    XMVECTOR q2 = triangle->GetPosition(2);
    XMVECTOR normal = triangle->GetNormal();

    //球の中心から平面への距離を計算
    float distanceToPlane = XMVectorGetX(XMVector3Dot(normal, p - q0));

    //平面と球の中心の距離が球の半径より大きい場合、非交差と判定
    if (fabs(distanceToPlane) > circle->size_.x) {
        return false;
    }

    // 球の中心から三角形の平面への垂線の足
    XMVECTOR pointOnPlane = p - distanceToPlane * normal;

    //三角形の辺ベクトルを計算
    XMVECTOR edge0 = q1 - q0;
    XMVECTOR edge1 = q2 - q0;

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

    //平面上の点が三角形内部かどうか
    if ((u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f)) {
        outDistanceVector = (circle->size_.x - distanceToPlane) * normal;
        bool output = fabs(distanceToPlane) < circle->size_.x;
        return output;
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

    // 各辺と球の中心との最小距離を計算
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

    //最小距離が球の半径以内かどうか
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