#include "Cell.h"
#include "Triangle.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"

namespace {
	bool boxDraw = false;
}

Cell::Cell()
	:position_(0,-99999,0), length_(0)
{
	for (int i = 0; i < 8; i++) verPos_[i] = XMFLOAT3(0, 0, 0);
}

void Cell::SetPosLeng(XMFLOAT3 pos, float leng)
{
	position_ = pos;
	length_ = leng;
	
	//�����v��荶��X�^�[�g�i�ォ�王�_�j
	verPos_[0] = XMFLOAT3(pos.x, pos.y, pos.z + leng);
	verPos_[1] = XMFLOAT3(pos.x + leng, pos.y, pos.z + leng);
	verPos_[2] = XMFLOAT3(pos.x + leng, pos.y, pos.z);
	verPos_[3] = XMFLOAT3(pos.x, pos.y, pos.z);

	//�㎞�v���
	verPos_[4] = XMFLOAT3(pos.x, pos.y + leng, pos.z + leng);
	verPos_[5] = XMFLOAT3(pos.x + leng, pos.y + leng, pos.z + leng);
	verPos_[6] = XMFLOAT3(pos.x + leng, pos.y + leng, pos.z);
	verPos_[7] = XMFLOAT3(pos.x, pos.y + leng, pos.z);
}

bool Cell::SetTriangle(Triangle& t)
{
	std::vector<XMFLOAT3> tp = t.GetAllPosition();

	//�S���_�� Cell�̒��_�ʒu���O���ɂ���ꍇ�́A�֐����I���
	if (tp[0].x < verPos_[7].x && tp[1].x < verPos_[7].x && tp[2].x < verPos_[7].x || //�S���_���E
		tp[0].x > verPos_[6].x && tp[1].x > verPos_[6].x && tp[2].x > verPos_[6].x || //�S���_����
		
		tp[0].y < verPos_[3].y && tp[1].y < verPos_[3].y && tp[2].y < verPos_[3].y || //�S���_����
		tp[0].y > verPos_[7].y && tp[1].y > verPos_[7].y && tp[2].y > verPos_[7].y || //�S���_����

		tp[0].z < verPos_[7].z && tp[1].z < verPos_[7].z && tp[2].z < verPos_[7].z || //�S���_����
		tp[0].z > verPos_[4].z && tp[1].z > verPos_[4].z && tp[2].z > verPos_[4].z )  //�S���_���O
	{
		return false;
	}

	if (t.IsMovable()) floarTriangles_.push_back(t);
	else wallTriangles_.push_back(t);
	return true;
}

bool Cell::SegmentVsFloarTriangle(RayCastData* _data)
{
	bool hit = false;
	float minDist = FBXSDK_FLOAT_MAX;

	for (int i = 0; i < (int)floarTriangles_.size(); i++) {
		floarTriangles_.at(i).RayCast(_data);

		//���C���������E�ŏ�������������㏑��
		if (_data->hit && minDist > _data->dist) {
			minDist = _data->dist;
			hit = true;
		}
	}

	_data->dist = minDist;
	return hit;
}
