#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
class Triangle;
struct RayCastData;

class Cell
{
	//�������̍��W
	XMFLOAT3 position_;

	//cube�̈�ӂ̒���
	float length_ = 0;

	//���_�ʒu posision
	XMFLOAT3 verPos_[8];

	//Triangles�̃��X�g
	std::vector<Triangle> floarTriangles_;
	std::vector<Triangle> wallTriangles_;

public:
	Cell();

	void SetPosLeng(XMFLOAT3 pos, float leng);

	//�O�p�|���S����AAB�ɐG��Ă����炱��CELL�ɓo�^
	bool SetTriangle(Triangle& t);

	//FloarTriangle�ɓ��������ꍇ�̍ŏ�������Ԃ�
	bool SegmentVsFloarTriangle(RayCastData* _data);
};