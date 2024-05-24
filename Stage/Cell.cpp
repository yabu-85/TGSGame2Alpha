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
	
	//下時計回り左上スタート（上から視点）
	verPos_[0] = XMFLOAT3(pos.x, pos.y, pos.z + leng);
	verPos_[1] = XMFLOAT3(pos.x + leng, pos.y, pos.z + leng);
	verPos_[2] = XMFLOAT3(pos.x + leng, pos.y, pos.z);
	verPos_[3] = XMFLOAT3(pos.x, pos.y, pos.z);

	//上時計回り
	verPos_[4] = XMFLOAT3(pos.x, pos.y + leng, pos.z + leng);
	verPos_[5] = XMFLOAT3(pos.x + leng, pos.y + leng, pos.z + leng);
	verPos_[6] = XMFLOAT3(pos.x + leng, pos.y + leng, pos.z);
	verPos_[7] = XMFLOAT3(pos.x, pos.y + leng, pos.z);
}

bool Cell::SetTriangle(Triangle& t)
{
	std::vector<XMFLOAT3> tp = t.GetAllPosition();

	//全頂点が Cellの頂点位置より外側にある場合は、関数を終わる
	if (tp[0].x < verPos_[7].x && tp[1].x < verPos_[7].x && tp[2].x < verPos_[7].x || //全頂点が右
		tp[0].x > verPos_[6].x && tp[1].x > verPos_[6].x && tp[2].x > verPos_[6].x || //全頂点が左
		
		tp[0].y < verPos_[3].y && tp[1].y < verPos_[3].y && tp[2].y < verPos_[3].y || //全頂点が下
		tp[0].y > verPos_[7].y && tp[1].y > verPos_[7].y && tp[2].y > verPos_[7].y || //全頂点が上

		tp[0].z < verPos_[7].z && tp[1].z < verPos_[7].z && tp[2].z < verPos_[7].z || //全頂点が奥
		tp[0].z > verPos_[4].z && tp[1].z > verPos_[4].z && tp[2].z > verPos_[4].z )  //全頂点が前
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

		//レイ当たった・最小距離だったら上書き
		if (_data->hit && minDist > _data->dist) {
			minDist = _data->dist;
			hit = true;
		}
	}

	_data->dist = minDist;
	return hit;
}
