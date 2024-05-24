#pragma once
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;
class BoxCollider;
class SphereCollider;
struct RayCastData;

class Triangle
{
	XMFLOAT3 position[3];
	XMVECTOR normal;
public:
	Triangle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3);

	//�|�W�V�����擾
	std::vector<XMFLOAT3> GetAllPosition();

	//�ړ��\���ǂ����𔻒�
	bool IsMovable();

	//RayCast
	void RayCast(RayCastData* data);

};