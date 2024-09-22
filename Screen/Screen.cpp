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
	
	const float GAMEPAD_DEAD_ZONE = 0.3f;
	const float PC_CTRL_THRESHOLD = 0.1f;

}

//�ŏ���GAMEPAD��
bool Screen::latestPCCtrl_ = false;

Screen::Screen() : state_(SCREEN_STATE::DRAW), selectCoolTime_(0), latestSelectUIIndex_(0)
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

	//PCCtrl�̏ꍇ
	if (latestPCCtrl_) {
		GameManager::SetCursorMode(true);

		//
		UIBase* boundUI = nullptr;
		int index = 0;
		for (auto u : uiList_) {
			index++;
			u->SetSelect(false);
			u->SetRedyBound(true);
			if (u->GetBound()) {
				boundUI = u;
				latestSelectUIIndex_ = index-1;
			}
		}
	
		//
		if (boundUI) {
			boundUI->SetSelect(true);
		}

		return;
	}

	GameManager::SetCursorMode(false);

	//
	UIBase* selectUI = uiList_.at(latestSelectUIIndex_);
	for (auto u : uiList_) {
		u->SetRedyBound(false);
	}

	//�Z���N�g�ړ��_���Ȃ�I���
	if (!selectUI->GetSelectPossible()) {
		return;
	}

	//Select�̈ړ��i���{�^��
	int dir = -1;
	if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT, 0)) dir = 0;
	else if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_LEFT, 0)) dir = 1;
	else if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_UP, 0)) dir = 2;
	else if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN, 0)) dir = 3;

	//Select�̈ړ��i�X�e�B�b�N
	selectCoolTime_--;
	if (dir <= -1 && selectCoolTime_ <= 0) {
		XMFLOAT3 stick = Input::GetPadStickL(0);
		if (stick.x > GAMEPAD_DEAD_ZONE) dir = 0;		//�E
		else if (stick.x < -GAMEPAD_DEAD_ZONE) dir = 1;	//��
		else if (stick.y > GAMEPAD_DEAD_ZONE) dir = 2;	//��
		else if (stick.y < -GAMEPAD_DEAD_ZONE) dir = 3;	//��
		//�I��������
		if (dir >= 0) selectCoolTime_ = 10;
	}

	//�ړ�����ꏊ����
	if (dir >= 0) {
		int minUIIndex = -1;
		float minDist = 9999.9f;
		int cIndex = 0;
		for (auto u : uiList_) {
			cIndex++;
			//�����̓X�L�b�v
			if (u == selectUI) continue;

			XMFLOAT3 sUIPos = selectUI->GetFramePosition();
			XMFLOAT3 tUIPos = u->GetFramePosition();
			XMFLOAT3 vecUI = Float3Sub(tUIPos, sUIPos);

			//���͕��������Ă��邩�m�F
			if (dir == 0 && (vecUI.x > DIR_LIST[dir].x) ||
				dir == 1 && (vecUI.x < DIR_LIST[dir].x) ||
				dir == 2 && (vecUI.y > DIR_LIST[dir].y) ||
				dir == 3 && (vecUI.y < DIR_LIST[dir].y))
			{
				//���͕����ň�ԋ߂���I��
				XMFLOAT2 vecDist = XMFLOAT2(vecUI.x * DIR_MULTI[dir].x, vecUI.y * DIR_MULTI[dir].y);
				float dist = sqrt(vecDist.x * vecDist.x + vecDist.y * vecDist.y);
				if (dist < minDist) {
					minUIIndex = cIndex - 1;
					minDist = dist;
				}
			}
		}

		//�I�ׂ�����X�V
		if (minUIIndex >= 0) {
			selectUI = uiList_.at(minUIIndex);
			latestSelectUIIndex_ = minUIIndex;
		}
	}

	for (auto u : uiList_) u->SetSelect(false);
	selectUI->SetSelect(true);

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
