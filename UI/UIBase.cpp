#include "UIBase.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"

namespace {
	static const float SELECT_ANIM_FLUCTUATION = 0.02f;			//�I���A�j���[�V�������̕ϓ��l
	static const float NOT_SELECT_ANIM_FLUCTUATION = 0.03f;		//�I�����ĂȂ��A�j���[�V�������̕ϓ��l

}

UIBase::UIBase(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
	: hButtonPict_{-1, -1}, hImagePict_(-1), alpha_{255}, isBound_(false), isSelect_(false), selectAnim_(0.0f), selectPlus_(true), screenSelectPossible_(true)
{
	//�֐��o�^
	onClick_ = onClick;

	//�g�����X�t�H�[��
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

void UIBase::SelectAnimUpdate()
{
	//�I��
	//�O�`1�̔����ϓ�
	if (isSelect_) {
		if (selectPlus_) {
			selectAnim_ += SELECT_ANIM_FLUCTUATION;
			if (selectAnim_ > 1.0f) {
				selectPlus_ = !selectPlus_;
				selectAnim_ = 1.0f;
			}
		}
		else {
			selectAnim_ -= SELECT_ANIM_FLUCTUATION;
			if (selectAnim_ < 0.0f) {
				selectPlus_ = !selectPlus_;
				selectAnim_ = 0.0f;
			}
		}
	}

	//�I�����ĂȂ�
	//�i�X����(0)�ɂȂ��Ă����悤��
	else {
		selectAnim_ -= NOT_SELECT_ANIM_FLUCTUATION;
		if (selectAnim_ < 0.0f) {
			selectAnim_ = 0.0f;
		}
		selectPlus_ = false;
	}
}
