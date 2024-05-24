#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
class Triangle;
struct RayCastData;

class Cell
{
	//左奥下の座標
	XMFLOAT3 position_;

	//cubeの一辺の長さ
	float length_ = 0;

	//頂点位置 posision
	XMFLOAT3 verPos_[8];

	//Trianglesのリスト
	std::vector<Triangle> floarTriangles_;
	std::vector<Triangle> wallTriangles_;

public:
	Cell();

	void SetPosLeng(XMFLOAT3 pos, float leng);

	//三角ポリゴンがAABに触れていたらこのCELLに登録
	bool SetTriangle(Triangle& t);

	//FloarTriangleに当たった場合の最小距離を返す
	bool SegmentVsFloarTriangle(RayCastData* _data);
};