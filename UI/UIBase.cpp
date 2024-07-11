#include "UIBase.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"

UIBase::UIBase(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize) 
	: hButtonPict_{-1, -1}, hImagePict_(-1), alpha_{255}, isBound_(false), isSelect_(false)
{
	//関数登録
	onClick_ = onClick;

	//トランスフォーム
	frameTransform_.scale_ = XMFLOAT3(size.x, size.y, 1.0f);
	frameTransform_.position_.x = pos.x;
	frameTransform_.position_.y = pos.y;

	imageTransform_.scale_ = XMFLOAT3(tsize.x, tsize.y, 1.0f);
	imageTransform_.position_ = frameTransform_.position_;
}

UIBase::~UIBase()
{
}

void UIBase::Update()
{
}

void UIBase::Draw()
{

}

void UIBase::OnClick()
{
	if (onClick_) onClick_();
}