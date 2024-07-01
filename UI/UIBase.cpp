#include "UIBase.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"

UIBase::UIBase() : hButtonPict_{-1, -1}, hImagePict_(-1), alpha_{255}, isBound_(false)
{
}

UIBase::~UIBase()
{
	Image::Release(hButtonPict_[0]);
	Image::Release(hButtonPict_[1]);
	Image::Release(hImagePict_);

}

void UIBase::Draw()
{
	Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);

	//押してない時はfalse(0)だから１が表示される
	Image::Draw(hButtonPict_[isBound_]);

	//テキストの表示
	Image::Draw(hImagePict_);
}

void UIBase::OnClick()
{
	if (onClick_) {
		onClick_();
	}
}

void UIBase::Initialize(std::string name, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
{
	//画像データ読み込み
	const std::string fileName[] = { "Image/ButtonFrame1.png", "Image/ButtonFrame2.png" };
	for (int i = 0; i < 2; i++) {
		hButtonPict_[i] = Image::Load(fileName[i]);
		assert(hButtonPict_[i] >= 0);
	}

	hImagePict_ = Image::Load("Image/" + name + ".png");
	assert(hImagePict_ >= 0);

	//関数登録
	onClick_ = onClick;

	//トランスフォーム
	buttonTransform_.scale_ = XMFLOAT3(size.x, size.y, 1.0f);
	buttonTransform_.position_.x = pos.x;
	buttonTransform_.position_.y = pos.y;
	Image::SetTransform(hButtonPict_[0], buttonTransform_);
	Image::SetTransform(hButtonPict_[1], buttonTransform_);

	imageTransform_.scale_ = XMFLOAT3(tsize.x, tsize.y, 1.0f);
	imageTransform_.position_ = buttonTransform_.position_;
	Image::SetTransform(hImagePict_, imageTransform_);

	//継承先クラスのInitialize
	Initialize();
}