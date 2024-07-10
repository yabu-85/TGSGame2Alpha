#include "HealthGauge.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Global.h"
#include "../Character/Character.h"

namespace {
	static const float MAX_DRAW_LENGTH = 50.0f;	//HP�̍ő�`�拗��
	static const float DEFAULT_SIZE_X = 0.6f;	//HP�̕`��T�C�Y
	static const float DEFAULT_SIZE_Y = 0.2f;	//HP�̕`��T�C�Y
	static const float DRAW_RANGE = 0.2f;		//HP�̕`��͈́i�X�N���[���j
	static const int MAX_ALPHA = 255;
	static const int ALPHA_VALUE = 30;
	static const float PngSizeX = 256.0f;
	static float halfSize = 0.0f;
}

//���ꏈ�����点��̂ł�
void HealthGauge::SetGaugeAlpha(int value, int index)
{
	gaugeAlpha_[index] += value;
	if (gaugeAlpha_[index] > MAX_ALPHA) {
		gaugeAlpha_[index] = MAX_ALPHA;
	}
	else if (gaugeAlpha_[index] < 0) {
		gaugeAlpha_[index] = 0;
	}
	Image::SetAlpha(hPict_[0], gaugeAlpha_[index]);
	Image::SetAlpha(hPict_[1], gaugeAlpha_[index]);
}

HealthGauge::HealthGauge(Character* parent)
	: parcent_(1.0f), height_(0.0f), gaugeAlpha_{ 0, 0 }, isDraw_(true), pParent_(parent), visuallyTime_{0, 0}
{
	std::string fileName[] = { "Gauge", "GaugeFrame" };
	for (int i = 0; i < MAX; i++) {
		hPict_[i] = Image::Load("Image/" + fileName[i] + ".png");
		assert(hPict_[i] >= 0);
	}

	halfSize = PngSizeX / (float)Direct3D::screenWidth_ * (DEFAULT_SIZE_X / 2.0f);
	transform_[0].scale_.x = DEFAULT_SIZE_X;
	transform_[0].scale_.y = DEFAULT_SIZE_Y;
	transform_[1] = transform_[0];
}

HealthGauge::~HealthGauge()
{
}

void HealthGauge::Draw(int index)
{
	//�\�����Ȃ�
	if (!isDraw_) {
		SetGaugeAlpha(-ALPHA_VALUE, index);
		if (gaugeAlpha_[index] > 0) {
			for (int i = 0; i < 2; i++) {
				Image::SetTransform(hPict_[i], transform_[i]);
				Image::Draw(hPict_[i]);
			}
		}
		visuallyTime_[index] = 0;
		return;
	}

	//�G�̒����_�ʒu
	XMFLOAT3 pos = pParent_->GetPosition();
	pos.y += pParent_->GetBodyHeightHalf();

	//�X�N���[���|�W�V����
	XMFLOAT3 scrPos = Camera::CalcScreenPosition(pos);

	//��p��������
	if (!Camera::IsScreenPositionWithinScreen(scrPos, DRAW_RANGE)) {
		SetGaugeAlpha(-ALPHA_VALUE, index);
	}
	else {
		SetGaugeAlpha(ALPHA_VALUE, index);
	}

	//�����x�P�ȏ�Ȃ�HealthGauge�\��
	if (gaugeAlpha_[index] > 0) {
		//�\���ʒu�̌v�Z
		pos = pParent_->GetPosition();
		pos.y += height_;
		scrPos = Camera::CalcScreenPosition(pos);

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
	parcent_ = f;
	transform_[0].scale_.x = parcent_ * DEFAULT_SIZE_X;
}

void HealthGauge::SetIsDraw(bool b)
{
	isDraw_ = b;
}