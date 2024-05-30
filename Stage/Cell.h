#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
class Triangle;
struct RayCastData;
class SphereCollider;

class Cell
{
	//cube�̈�ӂ̒���
	float length_ = 0;

	XMFLOAT3 min_;
	XMFLOAT3 max_;

	//Triangles�̃��X�g
	std::vector<Triangle> floarTriangles_;
	std::vector<Triangle> wallTriangles_;
	
	//Triangle���͈͓������ׂ�֐�
	bool IsPointInAABB(XMFLOAT3& point);
	bool IsTriangleInAABB(Triangle& tri);
	bool IntersectSegmentAABB(XMFLOAT3& p0, XMFLOAT3& p1);
	bool IntersectTriangleAABB(Triangle& tri);

public:
	Cell();
	
	//Cell�̊�{�f�[�^�Z�b�g
	void SetPosLeng(XMFLOAT3 pos, float leng);

	//�O�p�|���S����AAB�ɐG��Ă����炱��CELL�ɓo�^�i�_�������ʂ�Ȃ��ꍇ�o�^�ł��Ȃ��j
	bool SetTriangle(Triangle& t);

	//�o�^����Ă�Triangles������
	void ResetTriangles();

	//FloarTriangle�ɓ��������ꍇ�̍ŏ�������Ԃ�
	bool SegmentVsFloarTriangle(RayCastData* _data);
	//Wall�Ƃ��
	bool SegmentVsWallTriangle(RayCastData* _data);

	//���ƃ|���S�����肵�āA�����o�������֐�
	bool SphereVsTriangle(SphereCollider* collid, XMVECTOR& push);

	std::vector<Triangle> GetFloarTriangles() { return floarTriangles_; }
	std::vector<Triangle> GetWallTriangles() { return wallTriangles_; }

};