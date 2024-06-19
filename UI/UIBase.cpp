#include "UIBase.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"

UIBase::UIBase() : hButtonPict_{-1, -1}, hImagePict_(-1), alpha_{255}, isBound_(false)
{
}

UIBase::~UIBase()
{
}

void UIBase::Draw()
{
	Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);

	//押してない時はfalse(0)だから１が表示される
	Image::SetTransform(hButtonPict_[isBound_], buttonTransform_);
	Image::Draw(hButtonPict_[isBound_]);

	//テキストの表示
	Image::SetTransform(hImagePict_, uiTransform_);
	Image::Draw(hImagePict_);

	Direct3D::SetBlendMode(Direct3D::BLEND_ADD);
}

void UIBase::OnClick()
{
	if (onClick_) {
		onClick_();
	}
}

void UIBase::Initialize(std::string name, std::function<void()> onClick)
{
	const std::string fileName[] = { "Image/ButtonFrame1.png", "Image/ButtonFrame2.png" };
	for (int i = 0; i < 2; i++) {
		hButtonPict_[i] = Image::Load(fileName[i]);
		assert(hButtonPict_[i] >= 0);
	}

	hImagePict_ = Image::Load("Image/" + name + ".png");
	assert(hImagePict_ >= 0);

	onClick_ = onClick;
	Initialize(name, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), onClick);

}

void UIBase::Initialize(std::string name, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick)
{
	buttonTransform_.scale_ = XMFLOAT3(size.x, size.y, 1.0f);
	buttonTransform_.position_.x = pos.x;
	buttonTransform_.position_.y = pos.y;

	Initialize();
}
