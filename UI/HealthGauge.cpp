#include "HealthGauge.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Global.h"
#include "../Player/Player.h"

namespace {
	static const float MAX_DRAW_LENGTH = 50.0f;	//HP�̍ő�`�拗��
	static const float DEFAULT_SIZE_X = 2.0f;	//HP�̕`��T�C�Y
	static const float DEFAULT_SIZE_Y = 0.5f;	//HP�̕`��T�C�Y
	static const float DRAW_RANGE = 0.9f;		//HP�̕`��͈́i�X�N���[���j
	static const int MAX_ALPHA = 255;
	static const int ALPHA_VALUE = 30;
	static const float PngSizeX = 256.0f;
	static float halfSize = 0.0f;
}

//���ꏈ�����点��̂ł�
void HealthGauge::SetGaugeAlpha(int value)
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

HealthGauge::HealthGauge(GameObject* parent) : parcent(1.0f), height_(0.0f), gaugeAlpha_(0), isDraw_(true), pParent_(parent)
{
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

HealthGauge::~HealthGauge()
{
}

void HealthGauge::Draw()
{
	//�\�����Ȃ�
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

	//�G�̈ʒu
	XMFLOAT3 pos = pParent_->GetPosition();
	pos.y += 0.3f;

	//�X�N���[���|�W�V����
	XMFLOAT3 scrPos = Camera::CalcScreenPosition(pos);

	//��p��������
	if (!Camera::IsScreenPositionWithinScreen(scrPos, DRAW_RANGE)) {
		SetGaugeAlpha(-ALPHA_VALUE);
	}
	//�����Ő���
	else {
		SetGaugeAlpha(ALPHA_VALUE);
		/*
		float dist = CalculationDistance(Camera::GetTarget(), pos);
		if (dist < MAX_DRAW_LENGTH) SetGaugeAlpha(ALPHA_VALUE);
		else SetGaugeAlpha(-ALPHA_VALUE);
		*/

	}

	//�����x�P�ȏ�Ȃ�HealthGauge�\��
	if (gaugeAlpha_ > 0) {
		for (int i = 0; i < 2; i++) {
			transform_[i].position_.x = scrPos.x - halfSize;
			transform_[i].position_.y = scrPos.y;
			Image::SetTransform(hPict_[i], transform_[i]);
			Image::Draw(hPict_[i]);
		}
	}
}

void HealthGauge::SetHeight(float h)
{
	height_ = h;
}

void HealthGauge::SetParcent(float f)
{
	parcent = f;
	transform_[0].scale_.x = parcent * DEFAULT_SIZE_X;
}

void HealthGauge::SetIsDraw(bool b)
{
	isDraw_ = b;
}

void HealthGauge::SetAlphaMax()
{
	gaugeAlpha_ = MAX_ALPHA;
}

//--------------------------------------------------------------------------------

BossGauge::BossGauge(GameObject* parent) : HealthGauge(parent)
{
}

BossGauge::~BossGauge()
{
}
