#include "AimCursor.h"
#include "../Engine/Image.h"
#include "../Engine/Easing.h"
#include "../Engine/Global.h"
#include "../Other/GameManager.h"

namespace {
	//サイズのあれこれーーーーーーーーーーーーーーーーーーーーーーーーーーー
	XMFLOAT3 ONE_PLAYER_SIZE = XMFLOAT3(0.15f, 0.12f, 0.0f);
	XMFLOAT3 TWO_PLAYER_SIZE = XMFLOAT3(0.25f, 0.15f, 0.0f);
	XMFLOAT3 TWO_PLAYER_SIZE_REV = XMFLOAT3(TWO_PLAYER_SIZE.y, TWO_PLAYER_SIZE.x, TWO_PLAYER_SIZE.z);
	XMFLOAT3 TWO_PLAYER_SIZE_LIST[4] = { TWO_PLAYER_SIZE, TWO_PLAYER_SIZE_REV, TWO_PLAYER_SIZE, TWO_PLAYER_SIZE_REV };

	//移動方向ーーーーーーーーーーーーーーーーーーーーーーーーーーー
	XMFLOAT3 ONE_UP_VECTOR = XMFLOAT3(0.0f, 0.8f, 0.0f);
	XMFLOAT3 ONE_RIGHT_VECTOR = XMFLOAT3(0.43f, 0.0f, 0.0f);
	XMFLOAT3 ONE_DOWN_VECTOR = XMFLOAT3(0.0f, -0.8f, 0.0f);
	XMFLOAT3 ONE_LEFT_VECTOR = XMFLOAT3(-0.43f, 0.0f, 0.0f);
	XMFLOAT3 ONE_VECTOR_ARRAY[4] = { ONE_UP_VECTOR, ONE_RIGHT_VECTOR, ONE_DOWN_VECTOR, ONE_LEFT_VECTOR };

	XMFLOAT3 TWO_UP_VECTOR = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 TWO_RIGHT_VECTOR = XMFLOAT3(1.3f, 0.0f, 0.0f);
	XMFLOAT3 TWO_DOWN_VECTOR = XMFLOAT3(0.0f, -1.0f, 0.0f);
	XMFLOAT3 TWO_LEFT_VECTOR = XMFLOAT3(-1.3f, 0.0f, 0.0f);
	XMFLOAT3 TWO_VECTOR_ARRAY[4] = { TWO_UP_VECTOR, TWO_RIGHT_VECTOR, TWO_DOWN_VECTOR, TWO_LEFT_VECTOR };

	//回転リスト
	float ROTATE_ARRAY[4] = { 0, 90, 0, -90 };

	//移動量
	float ACCURACY_MAX_MOVE = 0.40f;
	float ACCURACY_MIN_MOVE = 0.00f;

	//HitCursorーーーーーーーーーーーーーーーーーーーーーーーーーーー
	//移動方向
	XMFLOAT3 ONE_UP_VECTOR_HIT = XMFLOAT3(0.34f, 0.64f, 0.0f);
	XMFLOAT3 ONE_RIGHT_VECTOR_HIT = XMFLOAT3(0.34f, -0.64f, 0.0f);
	XMFLOAT3 ONE_DOWN_VECTOR_HIT = XMFLOAT3(-0.34f, -0.64f, 0.0f);
	XMFLOAT3 ONE_LEFT_VECTOR_HIT = XMFLOAT3(-0.34f, 0.64f, 0.0f);
	XMFLOAT3 ONE_VECTOR_ARRAY_HIT[4] = { ONE_UP_VECTOR_HIT, ONE_RIGHT_VECTOR_HIT, ONE_DOWN_VECTOR_HIT, ONE_LEFT_VECTOR_HIT };

	XMFLOAT3 TWO_UP_VECTOR_HIT = XMFLOAT3(1.3f, 1.0f, 0.0f);
	XMFLOAT3 TWO_RIGHT_VECTOR_HIT = XMFLOAT3(1.3f, -1.0f, 0.0f);
	XMFLOAT3 TWO_DOWN_VECTOR_HIT = XMFLOAT3(-1.3f, -1.0f, 0.0f);
	XMFLOAT3 TWO_LEFT_VECTOR_HIT = XMFLOAT3(-1.3f, 1.0f, 0.0f);
	XMFLOAT3 TWO_VECTOR_ARRAY_HIT[4] = { TWO_UP_VECTOR_HIT, TWO_RIGHT_VECTOR_HIT, TWO_DOWN_VECTOR_HIT, TWO_LEFT_VECTOR_HIT };
	
	float ONE_ROTATE_ARRAY_HIT[4] = { 135, 45, -45, -135 };		//右上、右下、左下、左上
	float TWO_ROTATE_ARRAY_HIT[4] = { 120, 55, -55, -120 };

	//移動量
	float HIT_MAX_MOVE = 0.20f;
	float HIT_MIN_MOVE = 0.01f;

}

AimCursor::AimCursor() : hPict_(-1), hHitPict_(-1), hitParce_(0.0f), hitDurace_(0.05f), accuracyParce_(0.0f)
{
	hPict_ = Image::Load("Image/Cross.png");
	hHitPict_ = Image::Load("Image/HitCross.png");
	assert(hPict_ >= 0);
	assert(hHitPict_ >= 0);

}

AimCursor::~AimCursor()
{
}

void AimCursor::Update()
{
	hitParce_ -= hitDurace_;
	if (hitParce_ < 0.0f) hitParce_ = 0.0f;

}

void AimCursor::Draw()
{
	//大きさの時間経過Parcent
	for (int i = 0; i < 4; i++) {
		Transform t;
		t.rotate_.z = ROTATE_ARRAY[i];

		//人数によって大きさ調整
		if (GameManager::IsOnePlayer()) {
			t.position_ = Float3Multiply(ONE_VECTOR_ARRAY[i], (ACCURACY_MAX_MOVE * accuracyParce_) + ACCURACY_MIN_MOVE);
			t.scale_ = ONE_PLAYER_SIZE;
		}
		else {
			t.position_ = Float3Multiply(TWO_VECTOR_ARRAY[i], (ACCURACY_MAX_MOVE * accuracyParce_) + ACCURACY_MIN_MOVE);
			t.scale_ = TWO_PLAYER_SIZE_LIST[i];
		}

		Image::SetTransform(hPict_, t);
		Image::Draw(hPict_);
	}

	//Hitの描画
	if (hitParce_ > 0.0f) {
		Transform t;
		
		for (int i = 0; i < 4; i++) {
			if (GameManager::IsOnePlayer()) {
				t.position_ = Float3Multiply(ONE_VECTOR_ARRAY_HIT[i], (HIT_MAX_MOVE * hitParce_) + HIT_MIN_MOVE);
				t.scale_ = ONE_PLAYER_SIZE;
				t.rotate_.z = ONE_ROTATE_ARRAY_HIT[i];
			}
			else {
				t.position_ = Float3Multiply(TWO_VECTOR_ARRAY_HIT[i], (HIT_MAX_MOVE * hitParce_) + HIT_MIN_MOVE);
				t.scale_ = TWO_PLAYER_SIZE_LIST[i];
				t.rotate_.z = TWO_ROTATE_ARRAY_HIT[i];
			}

			Image::SetTransform(hHitPict_, t);
			Image::SetAlpha(hHitPict_, (int)(255.0f * hitParce_ + 50));
			Image::Draw(hHitPict_);
		}
	}

}
void AimCursor::Hit()
{
	hitParce_ = 1.0f;
}