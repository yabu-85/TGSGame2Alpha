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
		//範囲内に入り始めたら音再生
		//if (!isBound_) AudioManager::Play(AUDIO_TYPE::BUTTON_WITHIN);

		isBound_ = true;
		return true;
	}

	isBound_ = false;
	return false;
}

void ButtonUI::Initialize()
{
	XMFLOAT3 txtSi = Image::GetTextureSize(hButtonPict_[0]);
	frameHalfSize_ = XMFLOAT2(txtSi.x * buttonTransform_.scale_.x / 2.0f, txtSi.y * buttonTransform_.scale_.y / 2.0f);

	float screenWidth = (float)Direct3D::screenWidth_;		//スクリーンの幅
	float screenHeight = (float)Direct3D::screenHeight_;	//スクリーンの高さ
	widePos_.x = screenWidth / 2.0f + screenWidth / 2.0f * buttonTransform_.position_.x;
	widePos_.y = screenHeight / 2.0f + screenHeight / 2.0f * -buttonTransform_.position_.y;

}
