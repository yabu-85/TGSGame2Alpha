#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
class Triangle;
struct RayCastData;
class SphereCollider;

class Cell
{
	//cubeの一辺の長さ
	float length_ = 0;

	XMFLOAT3 min_;
	XMFLOAT3 max_;

	//Trianglesのリスト
	std::vector<Triangle> floarTriangles_;
	std::vector<Triangle> wallTriangles_;
	
	//Triangleが範囲内か調べる関数
	bool IsPointInAABB(XMFLOAT3& point);
	bool IsTriangleInAABB(Triangle& tri);
	bool IntersectSegmentAABB(XMFLOAT3& p0, XMFLOAT3& p1);
	bool IsPointInTriangle(XMFLOAT3& pt, XMVECTOR& v0, XMVECTOR& v1, XMVECTOR& v2);
	bool IsAABBInsideTriangle(Triangle& triangle);
	bool IntersectTriangleAABB(Triangle& tri);

public:
	Cell();
	
	//Cellの基本データセット
	void SetPosLeng(XMFLOAT3 pos, float leng);

	//三角ポリゴンがAABに触れていたらこのCELLに登録
	bool SetTriangle(Triangle& t);

	//登録されてるTriangles初期化
	void ResetTriangles();

	//FloarTriangleに当たった場合の最小距離を返す
	bool SegmentVsFloarTriangle(RayCastData* _data);
	//Wallとやる
	bool SegmentVsWallTriangle(RayCastData* _data);

	bool SphereVsTriangle(SphereCollider* collid, XMVECTOR& push);

};