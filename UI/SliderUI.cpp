#include "SliderUI.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Engine/Easing.h"

namespace {
	static const float BOUND_SIZE = 1.1f;		//�ʏ�T�C�Y�Ɋ|���ĕ\��
	static const float BUTTON_SIZE = 0.05f;		//IsBound�̔��苗��
	static const float BORDER_POSX = 10000.0f;	//�}�E�X�̃E�B���h�E�͈͊O�Ώ��p

}

SliderUI::SliderUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
	: UIBase(pos, size, onClick, tsize), buttonPosX_(0.0f), gaugePercent_(0.5f), isDragging_(false), frameSize_{0,0}
{
}

SliderUI::~SliderUI()
{
}

void SliderUI::Update()
{
	//�l��ς���
	if (isDragging_) {
		//�}�E�X�̍��{�^���������ꂽ�Ƃ��A�h���b�O���~
		if (Input::IsMouseButtonUp(0)) {
			isDragging_ = false;
			return;
		}

		Dragging();
		return;
	}

	if (IsWithinBound()) {
		//�J�[�\�����d�Ȃ��Ă�Ƃ����Đ�
		if (!isBound_)
		{
			//AudioManager::PlaySoundMa(AUDIO_POINTCURSOR);
			isBound_ = true;
		}

		//�}�E�X�̍��{�^���������ꂽ�Ƃ��A�h���b�O���J�n
		if (Input::IsMouseButtonDown(0)) {
			Dragging();
			isDragging_ = true;
		}
	}
	else {
		isBound_ = false;
	}

}

void SliderUI::Draw()
{
	float scrX = (float)Direct3D::screenWidth_;

	//�Z���ق��̃o�[
	Image::SetTransform(hButtonPict_[1], frameTransform_);
	Image::Draw(hButtonPict_[1]);

	//�����ق��̃o�[
	float NXsize = Image::GetTextureSize(hButtonPict_[0]).x;
	float NYsize = Image::GetTextureSize(hButtonPict_[0]).y;
	Transform slider = frameTransform_;
	slider.position_.x -= (frameSize_.x / scrX) - frameSize_.x / scrX * gaugePercent_;
	Image::SetRect(hButtonPict_[0], 0, 0, (int)(NXsize * gaugePercent_) , NYsize);
	Image::SetTransform(hButtonPict_[0], slider);
	Image::Draw(hButtonPict_[0]);

	//�{�^��
	Transform button = imageTransform_;
	if (isBound_) button.scale_ = Float3Multiply(button.scale_, BOUND_SIZE);
	Image::SetAlpha(hImagePict_, 100);
	Image::SetTransform(hImagePict_, button);
	Image::Draw(hImagePict_);
}

void SliderUI::Initialize(std::string name)
{
	//�摜�f�[�^�ǂݍ���
	std::string fileName[] = { "SliderFrame", "Slider", "SliderButton" };
	for (int i = 0; i < 2; i++) {
		hButtonPict_[i] = Image::Load("Image/" + fileName[i] + ".png");
		assert(hButtonPict_[i] >= 0);
	}
	hImagePict_ = Image::Load("Image/" + fileName[2] + ".png");
	assert(hImagePict_ >= 0);

	//Scale�ɍ��킹�Ē���
	frameSize_ = { Image::GetTextureSize(hButtonPict_[0]).x, Image::GetTextureSize(hButtonPict_[0]).y};
	frameSize_ = { frameSize_.x * frameTransform_.scale_.x, frameSize_.y * frameTransform_.scale_.y };

	//�X���C�_�[�{�^��
	float sizeX = frameSize_.x / (float)Direct3D::screenWidth_;
	imageTransform_.position_.x = frameTransform_.position_.x + (sizeX * 2.0f * gaugePercent_) - sizeX;
	buttonPosX_ = imageTransform_.position_.x;

}

bool SliderUI::IsWithinBound()
{
	//-1�`�P�͈̔͂Ɍv�Z
	float scrX = (float)Direct3D::screenWidth_;
	float scrY = (float)Direct3D::screenHeight_;
	XMFLOAT3 mouse = Input::GetMousePosition();
	mouse = { mouse.x * 2.0f - scrX, mouse.y * 2.0f - scrY, 0.0f };
	mouse = { mouse.x / scrX, -(mouse.y / scrY), 0.0f };

	//���苗�������ۂƓ_�Ŕ���
	XMFLOAT3 button = { buttonPosX_, frameTransform_.position_.y, 0.0f };
	float dist = CalculationDistance(mouse, button);
	return (dist <= BUTTON_SIZE);
}

void SliderUI::Dragging()
{
	float scrX = (float)Direct3D::screenWidth_;
	float scrY = (float)Direct3D::screenHeight_;

	//�E�B���h�E�O�̑Ώ�
	XMFLOAT3 mouse = Input::GetMousePosition();
	if (mouse.x > BORDER_POSX || mouse.x > scrX) {
		isDragging_ = false;
		return;
	}

	//-1�`�P�͈̔͂Ɍv�Z
	mouse = { mouse.x * 2.0f - scrX, mouse.y * 2.0f - scrY, 0.0f };
	mouse = { mouse.x / scrX, -(mouse.y / scrY), 0.0f };

	//�㉺�ŏI������
	static const float HEIGHT_RANGE = 0.3f;
	if (mouse.y > (frameTransform_.position_.y + HEIGHT_RANGE) ||
		mouse.y < (frameTransform_.position_.y - HEIGHT_RANGE))
	{
		isDragging_ = false;
		return;
	}
	
	//�O�`�P�ɕϊ�
	float mousePar = (mouse.x + 1.0f) * 0.5f;
	float sizeX = frameSize_.x / scrX;
	float posE = ((frameTransform_.position_.x + sizeX) + 1.0f) * 0.5f;
	float posS = ((frameTransform_.position_.x - sizeX) + 1.0f) * 0.5f;

	//�Q�[�W�v�Z
	gaugePercent_ = (mousePar - posS) / (posE - posS);
	if (gaugePercent_ < 0.0f) gaugePercent_ = 0.0f;
	else if (gaugePercent_ > 1.0f) gaugePercent_ = 1.0f;

	//�X���C�_�[�{�^��
	imageTransform_.position_.x = frameTransform_.position_.x + (sizeX * 2.0f * gaugePercent_) - sizeX;
	buttonPosX_ = imageTransform_.position_.x;

	//�֐��Ăяo��
	OnClick();
}
