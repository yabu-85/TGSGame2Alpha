#include "AimCursor.h"
#include "../Engine/Image.h"
#include "../Engine/Easing.h"
#include "../Engine/Global.h"
#include "../Other/GameManager.h"

namespace {
	//サイズのあれこれ
	XMFLOAT3 ONE_PLAYER_SIZE = XMFLOAT3(0.3f, 0.3f, 0.3f);
	XMFLOAT3 TWO_PLAYER_SIZE = XMFLOAT3(0.55f, 0.3f, 0.3f);
	XMFLOAT3 TWO_PLAYER_SIZE_REV = XMFLOAT3(TWO_PLAYER_SIZE.y, TWO_PLAYER_SIZE.x, TWO_PLAYER_SIZE.z);
	XMFLOAT3 TWO_PLAYER_SIZE_LIST[4] = { TWO_PLAYER_SIZE, TWO_PLAYER_SIZE_REV, TWO_PLAYER_SIZE, TWO_PLAYER_SIZE_REV };

	//移動方向
	XMFLOAT3 UP_VECTOR = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 RIGHT_VECTOR = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 DOWN_VECTOR = XMFLOAT3(0.0f, -1.0f, 0.0f);
	XMFLOAT3 LEFT_VECTOR = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	XMFLOAT3 VECTOR_ARRAY[4] = { UP_VECTOR, RIGHT_VECTOR, DOWN_VECTOR, LEFT_VECTOR };

	//回転リスト
	float ROTATE_ARRAY[4] = { 0, 90, 0, 90 };

	//移動量
	float MAX_MOVE = 0.08f;
	float MIN_MOVE = 0.01f;

}

AimCursor::AimCursor() : hPict_(-1), shotParce_(0.0f), shotDurace_(0.002f)
{
	hPict_ = Image::Load("Image/cross.png");
	assert(hPict_ >= 0);
}

AimCursor::~AimCursor()
{
}

void AimCursor::Update()
{
	shotParce_ -= shotDurace_;
	if (shotParce_ < 0.0f) shotParce_ = 0.0f;

}

void AimCursor::Draw()
{
	//大きさの時間経過Parcent
	float parce = Easing::EaseInQuint(shotParce_);

	for (int i = 0; i < 4; i++) {
		Transform t;
		
		//人数によって大きさ調整
		if (GameManager::IsOnePlayer()) t.scale_ = ONE_PLAYER_SIZE;
		else t.scale_ = TWO_PLAYER_SIZE_LIST[i];

		t.position_ = Float3Multiply(VECTOR_ARRAY[i], (MAX_MOVE * parce) + MIN_MOVE);
		t.scale_ = Float3Add(t.scale_, Float3Multiply(t.scale_, parce));
		t.rotate_.z = ROTATE_ARRAY[i];

		Image::SetTransform(hPict_, t);
		Image::Draw(hPict_);
	}

}

void AimCursor::Shot()
{
	static const float ADD_POWER = 0.1f;
	shotParce_ += ADD_POWER;
	if (shotParce_ > 1.0f) shotParce_ = 1.0f;
}

float AimCursor::GetBurePower()
{
	float parce = Easing::EaseInQuint(shotParce_);
	return (MAX_MOVE * parce) + MIN_MOVE;
}
