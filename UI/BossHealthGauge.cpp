#include "BossHealthGauge.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Global.h"
#include "../Character/Character.h"

namespace {
	static const int ALPHA_VALUE = 30;
}

BossHealthGauge::BossHealthGauge(Character* parent) : HealthGauge(parent)
{
}

BossHealthGauge::~BossHealthGauge()
{
}

void BossHealthGauge::Draw(int index)
{
	transform_.position_.x = offsetPosition_.x;
	transform_.position_.y = offsetPosition_.y;

	//•\Ž¦‚µ‚È‚¢
	if (!isDraw_) {
		SetGaugeAlpha(-ALPHA_VALUE, index);
		if (gaugeAlpha_[index] > 0) {
			Transform t = transform_;
			for (int i = 1; i >= 0; i--) {
				Image::SetTransform(hPict_[i], t);
				Image::Draw(hPict_[i]);

				//2‰ñ–Ú‚ÌŒvŽZ
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

		//2‰ñ–Ú‚ÌŒvŽZ
		t.scale_.x *= parcent_;
	}

}
