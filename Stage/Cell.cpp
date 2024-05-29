#include "Cell.h"
#include "Triangle.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"

#include "../Engine/Collider.h"
#include "../Engine/SphereCollider.h"

Cell::Cell() : length_(0), min_(XMFLOAT3()), max_(XMFLOAT3())
{
}

void Cell::SetPosLeng(XMFLOAT3 pos, float leng)
{
	length_ = leng;

	float tyoitasi = 0.01f;
	min_ = XMFLOAT3(pos.x - tyoitasi, pos.y - tyoitasi, pos.z - tyoitasi);
	max_ = XMFLOAT3(pos.x + leng + tyoitasi, pos.y + leng + tyoitasi, pos.z + leng + tyoitasi);
}

bool Cell::SetTriangle(Triangle& t)
{
	//三角形の頂点がAABBの内部にあるか
	if (IsTriangleInAABB(t)) {
		if (t.IsMovable()) floarTriangles_.push_back(t);
		else wallTriangles_.push_back(t);
		return true;
	}

	//三角形の辺がAABBと交差しているか
	if (IntersectTriangleAABB(t)) {
		if (t.IsMovable()) floarTriangles_.push_back(t);
		else wallTriangles_.push_back(t);
		return true;
	}

	return false;
}

void Cell::ResetTriangles()
{
	floarTriangles_.clear();
	wallTriangles_.clear();
}

bool Cell::SegmentVsFloarTriangle(RayCastData* _data)
{
	bool hit = false;
	float minDist = FBXSDK_FLOAT_MAX;

	for (int i = 0; i < (int)floarTriangles_.size(); i++) {
		floarTriangles_.at(i).RayCast(_data);

		//レイ当たった・最小距離だったら上書き
		if (_data->hit && minDist > _data->dist) {
			minDist = _data->dist;
			hit = true;
		}
	}

	_data->dist = minDist;
	return hit;
}

bool Cell::SegmentVsWallTriangle(RayCastData* _data)
{
	bool hit = false;
	float minDist = FBXSDK_FLOAT_MAX;

	for (int i = 0; i < (int)wallTriangles_.size(); i++) {
		wallTriangles_.at(i).RayCast(_data);

		//レイ当たった・最小距離だったら上書き
		if (_data->hit && minDist > _data->dist) {
			minDist = _data->dist;
			hit = true;
		}
	}

	_data->dist = minDist;
	return hit;
}

#include "../Engine/GameObject.h"
#include "../Engine/Global.h"
bool Cell::SphereVsTriangle(SphereCollider* collid, XMVECTOR& push)
{
	bool hit = false;
	for (int i = 0; i < (int)wallTriangles_.size(); i++) {
		if (collid->IsHitCircleVsTriangle(collid, &wallTriangles_[i], push)) {
			XMFLOAT3 pos = collid->pGameObject_->GetPosition();
			XMFLOAT3 fP = XMFLOAT3();
			XMStoreFloat3(&fP, push);

			pos = Float3Add(pos, fP);
			collid->pGameObject_->SetPosition(pos);
			hit = true;
		}
	}

	return hit;
}

//--------------------------------------------------------------------

bool Cell::IsPointInAABB(XMFLOAT3& point) {
	return (point.x >= min_.x && point.x <= max_.x &&
		point.y >= min_.y && point.y <= max_.y &&
		point.z >= min_.z && point.z <= max_.z);
}

bool Cell::IsTriangleInAABB(Triangle& tri) {
	XMFLOAT3 pos[3];
	for (int i = 0; i < 3; i++) XMStoreFloat3(&pos[i], tri.GetPosition(i));
	return (IsPointInAABB(pos[0]) || IsPointInAABB(pos[1]) || IsPointInAABB(pos[2]));
}

//各辺が入っているか
bool Cell::IntersectSegmentAABB(XMFLOAT3& p0, XMFLOAT3& p1) {
	//逆数の計算（１＝0.5, 4＝0.25）
	XMVECTOR invD = XMVectorReciprocal(XMVectorSubtract(XMLoadFloat3(&p1), XMLoadFloat3(&p0)));
	XMVECTOR t0s = XMVectorMultiply(XMVectorSubtract(XMLoadFloat3(&min_), XMLoadFloat3(&p0)), invD);
	XMVECTOR t1s = XMVectorMultiply(XMVectorSubtract(XMLoadFloat3(&max_), XMLoadFloat3(&p0)), invD);
	XMVECTOR tmin = XMVectorMin(t0s, t1s);
	XMVECTOR tmax = XMVectorMax(t0s, t1s);

	float tminF = max(max(XMVectorGetX(tmin), XMVectorGetY(tmin)), XMVectorGetZ(tmin));
	float tmaxF = min(min(XMVectorGetX(tmax), XMVectorGetY(tmax)), XMVectorGetZ(tmax));
	return tminF <= tmaxF && tmaxF >= 0.0f && tminF <= 1.0f;
}

//三角形の各辺について判定
bool Cell::IntersectTriangleAABB(Triangle& tri) {
	XMFLOAT3 pos[3];
	for (int i = 0; i < 3; i++) XMStoreFloat3(&pos[i], tri.GetPosition(i));
	return (IntersectSegmentAABB(pos[0], pos[1]) || IntersectSegmentAABB(pos[1], pos[2]) || IntersectSegmentAABB(pos[2], pos[0]));
}