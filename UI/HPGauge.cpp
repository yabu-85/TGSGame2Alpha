#include "HPGauge.h"
#include "../Engine/Image.h"
#include "../Player/Player.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Global.h"

//#include "EnemyBase.h"

namespace {
	static const float MAX_DRAW_LENGTH = 50.0f;	//HPの最大描画距離
	static const float DEFAULT_SIZE_X = 2.0f;	//HPの描画サイズ
	static const float DEFAULT_SIZE_Y = 0.5f;	//HPの描画サイズ
	static const float DRAW_RANGE = 0.9f;		//HPの描画範囲（スクリーン）
	static const int MAX_ALPHA = 255;
	static const int ALPHA_VALUE = 30;
	static const float PngSizeX = 256.0f;
	static float halfSize = 0.0f;
}

//これ処理減らせるのでは
void HPGauge::SetGaugeAlpha(int value)
{
	gaugeAlpha_ += value;
	if (gaugeAlpha_ > MAX_ALPHA) {
		gaugeAlpha_ = MAX_ALPHA;
	}
	else if (gaugeAlpha_ < 0) {
		gaugeAlpha_ = 0;
	}
	Image::SetAlpha(hPict_[0], gaugeAlpha_);
	Image::SetAlpha(hPict_[1], gaugeAlpha_);
}

HPGauge::HPGauge(EnemyBase* parent)
	: pParent_(parent), parcent(1.0f), height_(0.0f), gaugeAlpha_(0), isDraw_(true)
{
	for (int i = 0; i < PNG_INDEX::MAX; i++) hPict_[i] = -1;
}

HPGauge::~HPGauge()
{
}

void HPGauge::Initialize(float height)
{
    height_ = height;

	std::string fileName[] = { "Gauge", "GaugeFrame" };
	for (int i = 0; i < MAX; i++) {
		hPict_[i] = Image::Load("Image/" + fileName[i] + ".png");
		assert(hPict_[i] >= 0);
	}

	halfSize = PngSizeX / (float)Direct3D::screenWidth_;

	transform_[0].scale_.x = DEFAULT_SIZE_X;
	transform_[0].scale_.y = DEFAULT_SIZE_Y;
	transform_[1] = transform_[0];
}

void HPGauge::Draw()
{
	//表示しない
	if (!isDraw_) {
		SetGaugeAlpha(-ALPHA_VALUE);
		if (gaugeAlpha_ > 0) {
			for (int i = 0; i < 2; i++) {
				Image::SetTransform(hPict_[i], transform_[i]);
				Image::Draw(hPict_[i]);
			}
		}
		return;
	}

	//敵の位置
	XMFLOAT3 pos = pParent_->GetPosition();
	pos.y += height_; 
	
	//スクリーンポジション
	XMFLOAT3 scrPos = Camera::CalcScreenPosition(pos);

	//HP最大の場合・後ろに表示されている場合処理終わり
	if (parcent >= 1.0f) return;

	//画角制限する
	if (!Camera::IsScreenPositionWithinScreen(scrPos, DRAW_RANGE)) {
		SetGaugeAlpha(-ALPHA_VALUE);
	}
	//距離で制限
	else {
		float dist = CalculationDistance(GameManager::GetPlayer()->GetPosition(), pos);
		if (dist < MAX_DRAW_LENGTH) SetGaugeAlpha(ALPHA_VALUE);
		else SetGaugeAlpha(-ALPHA_VALUE);
	}

	//透明度１以上ならHPGauge表示
	if (gaugeAlpha_ > 0) {
		for (int i = 0; i < 2; i++) {
			transform_[i].position_.x = scrPos.x - halfSize;
			transform_[i].position_.y = scrPos.y;
			Image::SetTransform(hPict_[i], transform_[i]);
			Image::Draw(hPict_[i]);
		}
	}
}

void HPGauge::SetParcent(float f)
{
	parcent = f;
	transform_[0].scale_.x = parcent * DEFAULT_SIZE_X;
}

void HPGauge::SetIsDraw(bool b)
{
	isDraw_ = b;
}

void HPGauge::SetAlphaMax()
{
	gaugeAlpha_ = MAX_ALPHA;
}
