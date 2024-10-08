#include "FixedHealthGauge.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Global.h"
#include "../Character/Character.h"

namespace {
	static const int ALPHA_VALUE = 30;
}

FixedHealthGauge::FixedHealthGauge(Character* parent, XMFLOAT2 size) : HealthGauge(parent, size)
{
}

FixedHealthGauge::~FixedHealthGauge()
{
}

void FixedHealthGauge::Draw(int index)
{
	transform_.position_.x = offsetPosition_.x;
	transform_.position_.y = offsetPosition_.y;

	//表示しない
	if (!isDraw_) {
		SetGaugeAlpha(-ALPHA_VALUE, index);
		if (gaugeAlpha_[index] > 0) {
			Transform t = transform_;
			for (int i = 1; i >= 0; i--) {
				Image::SetTransform(hPict_[i], t);
				Image::Draw(hPict_[i]);

				//2回目の計算
				t.scale_.x *= parcent_;
			}
		}
		visuallyTime_[index] = 0;
		return;
	}

	SetGaugeAlpha(ALPHA_VALUE, index);
	Transform t = transform_;
	for (int i = 1; i >= 0; i--) {
		Image::SetTransform(hPict_[i], t);
		Image::Draw(hPict_[i]);

		//2回目の計算
		t.scale_.x *= parcent_;
	}

}
