#pragma once
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;
class BoxCollider;
class SphereCollider;
struct RayCastData;

class Triangle
{
	XMVECTOR position[3];
	XMVECTOR normal;
public:
	Triangle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3);

	//ポジション取得 0～2
	XMVECTOR GetPosition(int index);

	//ノーマル取得
	XMVECTOR GetNormal();

	//移動可能かどうかを判定
	bool IsMovable();

	//RayCast
	void RayCast(RayCastData* data);

};