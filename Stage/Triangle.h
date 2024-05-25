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

	//�|�W�V�����擾 0�`2
	XMVECTOR GetPosition(int index);

	//�m�[�}���擾
	XMVECTOR GetNormal();

	//�ړ��\���ǂ����𔻒�
	bool IsMovable();

	//RayCast
	void RayCast(RayCastData* data);

};