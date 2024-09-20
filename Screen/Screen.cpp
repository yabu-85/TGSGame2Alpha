#include "Screen.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"
#include "../UI/SliderUI.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Other/GameManager.h"

namespace {
	const float DIR_DIST = 0.01f;
	const XMFLOAT2 DIR_LIST[4] = { XMFLOAT2(DIR_DIST, 10.0f), XMFLOAT2(-DIR_DIST, 10.0f), XMFLOAT2(10.0f, DIR_DIST), XMFLOAT2(10.0f, -DIR_DIST) };
	const XMFLOAT2 DIR_MULTI[4] = { XMFLOAT2(0.5, 1.0f), XMFLOAT2(0.5, 1.0f), XMFLOAT2(1.0f, 0.5f), XMFLOAT2(1.0f, 0.5f) };
	
	const float GAMEPAD_DEAD_ZONE = 0.5f;
	const float PC_CTRL_THRESHOLD = 0.1f;

}

Screen::Screen() : state_(SCREEN_STATE::DRAW), selectCoolTime_(0), latestPCCtrl_(false), latestSelectUIIndex_(0)
{
}

Screen::~Screen()
{
	for (auto u : uiList_)
	{
		delete u;
	}
	uiList_.clear();
}

void Screen::Update()
{
	//UI�Ȃ��ꍇ�͏I��
	if (uiList_.empty()) return;

	//�ǂ̎�ނő��삵�Ă��邩����
	if (latestPCCtrl_) {
		//���{�^�� || ���X�e�B�b�N��������؂�ւ�
		XMFLOAT3 stick = Input::GetPadStickL(0);
		if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT, 0) ||
			Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_LEFT, 0) ||
			Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_UP, 0) ||
			Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN, 0) ||

			stick.x > GAMEPAD_DEAD_ZONE ||
			stick.x < -GAMEPAD_DEAD_ZONE ||
			stick.y > GAMEPAD_DEAD_ZONE ||
			stick.y < -GAMEPAD_DEAD_ZONE) 
		{
			latestPCCtrl_ = false;

			//�ŐV�̂�؂�ւ�
			uiList_.at(latestSelectUIIndex_)->SetSelect(true);
		}
	}
	else {
		//�}�E�X�𓮂�������؂�ւ�
		if (abs(Input::GetMouseMove().x) > PC_CTRL_THRESHOLD || abs(Input::GetMouseMove().y) > PC_CTRL_THRESHOLD) {
			latestPCCtrl_ = true;
			
			//SelectUIIndex�v�Z
			for (int i = 0; i < uiList_.size(); i++) {
				if (uiList_.at(i)->GetSelect()) {
					latestSelectUIIndex_ = i;
					break;
				}
			}
		}
	}

	//�SUI��Update
	for (auto u : uiList_) {
		u->Update();
	}

	//�Ō�ɑI��������̓n�C���C�g�\������
	UIBase* boundUI = nullptr;
	UIBase* selectUI = nullptr;
	for (auto u : uiList_) {
		if (u->GetBound()) boundUI = u;
		if (u->GetSelect()) selectUI = u;
	}

	//Select���Ă�UI�Ȃ�������
	if (!selectUI) {
		selectUI = uiList_[latestSelectUIIndex_];
	}

	//PCCtrl�̏ꍇ
	if (latestPCCtrl_) {
		//�J�[�\���\��
		GameManager::SetCursorMode(true);

		for (auto u : uiList_) {
			if (boundUI != u) {
				u->SetSelect(false);
			}
		}
		
		//�d�Ȃ��Ă�
		if (boundUI) {
			boundUI->SetSelect(true);
			boundUI->SelectUpdate();
		}

		return;
	}

}

void Screen::Draw()
{
	//PCCtrl�̏ꍇ
	if (latestPCCtrl_) {
		for (auto u : uiList_)
		{
			u->SetBound(false);
			u->Draw();
		}
		return;
	}

	//�Q�[���p�b�h�̏ꍇ
	for (auto u : uiList_)
	{
		u->Draw();
	}
}

void Screen::AddUI(UIBase* ui)
{
	uiList_.push_back(ui);
}

bool Screen::DeleteUI(UIBase* ui)
{
	for (auto iter = uiList_.begin(); iter != uiList_.end();) {
		if ((*iter) == ui)
		{
			delete* iter;
			iter = uiList_.erase(iter);
			return true;
		}
	}
	return false;
}
