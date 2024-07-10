#include "ButtonUI.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"

ButtonUI::ButtonUI() : UIBase(), widePos_(0.0f, 0.0f), frameHalfSize_(0.0f, 0.0f)
{
}

ButtonUI::~ButtonUI()
{
}

bool ButtonUI::IsWithinBound()
{
	XMFLOAT3 mouse = Input::GetMousePosition();

	if (mouse.y < widePos_.y + frameHalfSize_.y && mouse.y > widePos_.y - frameHalfSize_.y &&
		mouse.x < widePos_.x + frameHalfSize_.x && mouse.x > widePos_.x - frameHalfSize_.x)
	{
		//�͈͓��ɓ���n�߂��特�Đ�
		//if (!isBound_) AudioManager::Play(AUDIO_TYPE::BUTTON_WITHIN);

		isBound_ = true;
		return true;
	}

	isBound_ = false;
	return false;
}

void ButtonUI::Initialize()
{
	//�摜�f�[�^�ǂݍ���
	const std::string fileName[] = { "Image/ButtonFrame1.png", "Image/ButtonFrame2.png" };
	for (int i = 0; i < 2; i++) {
		hButtonPict_[i] = Image::Load(fileName[i]);
		assert(hButtonPict_[i] >= 0);
	}

	hImagePict_ = Image::Load("Image/" + name + ".png");
	assert(hImagePict_ >= 0);

	//�g�����X�t�H�[���Z�b�g
	Image::SetTransform(hButtonPict_[0], buttonTransform_);
	Image::SetTransform(hButtonPict_[1], buttonTransform_);
	Image::SetTransform(hImagePict_, imageTransform_);

	//�T�C�Y����
	XMFLOAT3 txtSi = Image::GetTextureSize(hButtonPict_[0]);
	frameHalfSize_ = XMFLOAT2(txtSi.x * buttonTransform_.scale_.x / 2.0f, txtSi.y * buttonTransform_.scale_.y / 2.0f);

	float screenWidth = (float)Direct3D::screenWidth_;		//�X�N���[���̕�
	float screenHeight = (float)Direct3D::screenHeight_;	//�X�N���[���̍���
	widePos_.x = screenWidth / 2.0f + screenWidth / 2.0f * buttonTransform_.position_.x;
	widePos_.y = screenHeight / 2.0f + screenHeight / 2.0f * -buttonTransform_.position_.y;

}
