#include "ModelButtonUI.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"
#include "../Engine/Model.h"

ModelButtonUI::ModelButtonUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
	: UIBase(pos, size, onClick, tsize), widePos_(0.0f, 0.0f), frameHalfSize_(0.0f, 0.0f), hModel_(-1)
{
}

ModelButtonUI::~ModelButtonUI()
{
}

void ModelButtonUI::Update()
{
	SelectAnimUpdate();

	if (IsWithinBound()) {
		//重なった時の音再生
		//if (!isBound_) AudioManager::Play(AUDIO_TYPE::BUTTON_WITHIN);

		//離したら関数呼び出し
		if (Input::IsMouseButtonUp(0) || Input::IsPadButtonDown(XINPUT_GAMEPAD_B, 0)) OnClick();

		isBound_ = true;
	}
	else {
		isBound_ = false;
	
	}
}

void ModelButtonUI::Draw()
{
	//通常
	Image::Draw(hButtonPict_[0]);

	Model::SetTransform(hModel_, imageTransform_);
	Model::Draw(hModel_);

}

void ModelButtonUI::Initialize(std::string name)
{
	hModel_ = Model::Load(name);
	assert(hModel_ >= 0);

	//画像データ読み込み
	const std::string fileName[] = { "Image/modelFrame.jpg" };
	hButtonPict_[0] = Image::Load(fileName[0]);
	assert(hButtonPict_[0] >= 0);

	//トランスフォームセット
	Image::SetTransform(hButtonPict_[0], frameTransform_);

	//サイズ調整
	XMFLOAT3 txtSi = Image::GetTextureSize(hButtonPict_[0]);
	frameHalfSize_ = XMFLOAT2(txtSi.x * frameTransform_.scale_.x / 2.0f, txtSi.y * frameTransform_.scale_.y / 2.0f);

	float screenWidth = (float)Direct3D::screenWidth_;		//スクリーンの幅
	float screenHeight = (float)Direct3D::screenHeight_;	//スクリーンの高さ
	widePos_.x = screenWidth / 2.0f + screenWidth / 2.0f * frameTransform_.position_.x;
	widePos_.y = screenHeight / 2.0f + screenHeight / 2.0f * -frameTransform_.position_.y;

}

bool ModelButtonUI::IsWithinBound()
{
	XMFLOAT3 mouse = Input::GetMousePosition();
	if (mouse.y < widePos_.y + frameHalfSize_.y && mouse.y > widePos_.y - frameHalfSize_.y &&
		mouse.x < widePos_.x + frameHalfSize_.x && mouse.x > widePos_.x - frameHalfSize_.x)
	{
		return true;
	}

	return false;
}

void ModelButtonUI::SelectUpdate()
{
	if (isSelect_) {
		isBound_ = true;

	}
}