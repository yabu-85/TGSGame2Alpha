#include "ButtonUI.h"
#include "../Other/InputManager.h"
#include "../Other/AudioManager.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"

ButtonUI::ButtonUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
	: UIBase(pos, size, onClick, tsize), widePos_(0.0f, 0.0f), frameHalfSize_(0.0f, 0.0f), isFrameDraw_(true)
{
}

ButtonUI::~ButtonUI()
{
}

void ButtonUI::Update()
{
	SelectAnimUpdate();

	if (redyBound_ && IsWithinBound()) {
		//重なった時の音再生
		if (!isBound_) AudioManager::Play(AUDIO_TYPE::BUTTON_WITHIN, 0.4f);

		//離したら関数呼び出し
		if (Input::IsMouseButtonUp(0)) OnClick();

		isBound_ = true;
	}
	else {
		isBound_ = false;

		//Ctrl
		if (isSelect_) {
			if (Input::IsPadButtonDown(XINPUT_GAMEPAD_A, 0)) OnClick();
		}
	}
}

void ButtonUI::Draw()
{
	if (isFrameDraw_) {
		//通常
		Image::SetTransform(hButtonPict_[0], frameTransform_);
		Image::Draw(hButtonPict_[0]);

		//セレクト
		Image::SetTransform(hButtonPict_[1], frameTransform_);
		Image::SetAlpha(hButtonPict_[1], (int)(255.0f *selectAnim_));
		Image::Draw(hButtonPict_[1]);
	}

	//テキストの表示
	Image::SetTransform(hImagePict_, imageTransform_);
	Image::Draw(hImagePict_);
}

void ButtonUI::Initialize(std::string name)
{
	//画像データ読み込み
	const std::string fileName[] = { "Image/ButtonFrame1.png", "Image/ButtonFrame2.png" };
	for (int i = 0; i < 2; i++) {
		hButtonPict_[i] = Image::Load(fileName[i]);
		assert(hButtonPict_[i] >= 0);
	}

	hImagePict_ = Image::Load("Image/" + name + ".png");
	assert(hImagePict_ >= 0);

	//トランスフォームセット
	Image::SetTransform(hButtonPict_[0], frameTransform_);
	Image::SetTransform(hButtonPict_[1], frameTransform_);
	Image::SetTransform(hImagePict_, imageTransform_);

	//サイズ調整
	XMFLOAT3 txtSi = Image::GetTextureSize(hButtonPict_[0]);
	frameHalfSize_ = XMFLOAT2(txtSi.x * frameTransform_.scale_.x / 2.0f, txtSi.y * frameTransform_.scale_.y / 2.0f);

	float screenWidth = (float)Direct3D::screenWidth_;		//スクリーンの幅
	float screenHeight = (float)Direct3D::screenHeight_;	//スクリーンの高さ
	widePos_.x = screenWidth / 2.0f + screenWidth / 2.0f * frameTransform_.position_.x;
	widePos_.y = screenHeight / 2.0f + screenHeight / 2.0f * -frameTransform_.position_.y;

}

bool ButtonUI::IsWithinBound()
{
	XMFLOAT3 mouse = Input::GetMousePosition();
	if (mouse.y < widePos_.y + frameHalfSize_.y && mouse.y > widePos_.y - frameHalfSize_.y &&
		mouse.x < widePos_.x + frameHalfSize_.x && mouse.x > widePos_.x - frameHalfSize_.x)
	{
		return true;
	}

	return false;
}

void ButtonUI::SelectUpdate()
{
	if (isSelect_) {
		isBound_ = true;

	}
}
