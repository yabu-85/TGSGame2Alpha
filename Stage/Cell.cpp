#include "Cell.h"
#include "Triangle.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/GameObject.h"
#include "../Engine/Global.h"
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
	XMVECTOR boxExtents = (XMLoadFloat3(&max_) - XMLoadFloat3(&min_)) * 0.5f;
	XMVECTOR boxCentre = XMLoadFloat3(&min_) + boxExtents;
	
	if (AABB_Tri_Intersect(t.GetPosition(0), t.GetPosition(1), t.GetPosition(2), boxCentre, boxExtents)) {
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
	float minDist = 99999.0f;

	for (int i = 0; i < (int)floarTriangles_.size(); i++) {
		floarTriangles_.at(i).RayCast(_data);

		//レイ当たった・最小距離だったら上書き
		if (_data->hit && minDist > _data->dist) {
			minDist = _data->dist;
			hit = true;
		}
	}

	_data->dist = minDist;
	_data->hit = hit;
	return hit;
}

bool Cell::SegmentVsWallTriangle(RayCastData* _data)
{
	bool hit = false;
	float minDist = 99999.0f;

	for (int i = 0; i < (int)wallTriangles_.size(); i++) {
		wallTriangles_.at(i).RayCast(_data);

		//レイ当たった・最小距離だったら上書き
		if (_data->hit && minDist > _data->dist) {
			minDist = _data->dist;
			hit = true;
		}
	}

	_data->dist = minDist;
	_data->hit = hit;
	return hit;
}

bool Cell::SphereVsTriangle(SphereCollider* collid, XMVECTOR& push)
{
	bool hit = false;
	for (int i = 0; i < (int)floarTriangles_.size(); i++) {
		if (collid->IsHitCircleVsTriangle(collid, &floarTriangles_[i], push)) {
			XMFLOAT3 pos = collid->pGameObject_->GetPosition();
			XMFLOAT3 fP = XMFLOAT3();
			XMStoreFloat3(&fP, push);

			pos = Float3Add(pos, fP);
			collid->pGameObject_->SetPosition(pos);
			hit = true;
		}
	}
	
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

//https://gist.github.com/zvonicek/fe73ba9903f49d57314cf7e8e0f05dcf
XMVECTOR xDir = XMVectorSet(1, 0, 0, 0);
XMVECTOR yDir = XMVectorSet(0, 1, 0, 0);
XMVECTOR zDir = XMVectorSet(0, 0, 1, 0);
bool Cell::AABB_Tri_SAT(const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2, const XMVECTOR& aabbExtents, const XMVECTOR& axis) {
	float p0 = XMVectorGetX(XMVector3Dot(v0, axis));
	float p1 = XMVectorGetX(XMVector3Dot(v1, axis));
	float p2 = XMVectorGetX(XMVector3Dot(v2, axis));
	float r = XMVectorGetX(aabbExtents) * abs(XMVectorGetX(XMVector3Dot(xDir, axis))) +
		      XMVectorGetY(aabbExtents) * abs(XMVectorGetX(XMVector3Dot(yDir, axis))) +
			  XMVectorGetZ(aabbExtents) * abs(XMVectorGetX(XMVector3Dot(zDir, axis)));

	float maxP = max(p0, max(p1, p2));
	float minP = min(p0, min(p1, p2));

	return !(max(-maxP, minP) > r);
}

bool Cell::AABB_Tri_Intersect(const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2, const XMVECTOR& aabbCentre, const XMVECTOR& aabbExtents) {
	XMVECTOR v0Rel = v0 - aabbCentre;
	XMVECTOR v1Rel = v1 - aabbCentre;
	XMVECTOR v2Rel = v2 - aabbCentre;

	XMVECTOR ab = XMVector3Normalize(v1Rel - v0Rel);
	XMVECTOR bc = XMVector3Normalize(v2Rel - v1Rel);
	XMVECTOR ca = XMVector3Normalize(v0Rel - v2Rel);

	XMVECTOR a00 = XMVectorSet(0.0f, XMVectorGetZ(-ab), XMVectorGetY(ab), 0.0f);
	XMVECTOR a01 = XMVectorSet(0.0f, XMVectorGetZ(-bc), XMVectorGetY(bc), 0.0f);
	XMVECTOR a02 = XMVectorSet(0.0f, XMVectorGetZ(-ca), XMVectorGetY(ca), 0.0f);

	XMVECTOR a10 = XMVectorSet(XMVectorGetZ(ab), 0.0f, XMVectorGetX(-ab), 0.0f);
	XMVECTOR a11 = XMVectorSet(XMVectorGetZ(bc), 0.0f, XMVectorGetX(-bc), 0.0f);
	XMVECTOR a12 = XMVectorSet(XMVectorGetZ(ca), 0.0f, XMVectorGetX(-ca), 0.0f);

	XMVECTOR a20 = XMVectorSet(XMVectorGetY(-ab), XMVectorGetX(ab), 0.0f, 0.0f);
	XMVECTOR a21 = XMVectorSet(XMVectorGetY(-bc), XMVectorGetX(bc), 0.0f, 0.0f);
	XMVECTOR a22 = XMVectorSet(XMVectorGetY(-ca), XMVectorGetX(ca), 0.0f, 0.0f);

	if (!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a00) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a01) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a02) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a10) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a11) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a12) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a20) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a21) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, a22) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, xDir) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, yDir) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, zDir) ||
		!AABB_Tri_SAT(v0Rel, v1Rel, v2Rel, aabbExtents, XMVector3Dot(ab, bc))) {
		return false;
	}

	return true;
}