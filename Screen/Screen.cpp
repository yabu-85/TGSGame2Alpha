#include "Screen.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"
#include "../UI/SliderUI.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"

namespace {
	static const float DIR_DIST = 0.01f;
	static const XMFLOAT2 DIR_LIST[4] = { XMFLOAT2(DIR_DIST, 10.0f), XMFLOAT2(-DIR_DIST, 10.0f), XMFLOAT2(10.0f, DIR_DIST), XMFLOAT2(10.0f, -DIR_DIST) };
	static const XMFLOAT2 DIR_MULTI[4] = { XMFLOAT2(0.5, 1.0f), XMFLOAT2(0.5, 1.0f), XMFLOAT2(1.0f, 0.5f), XMFLOAT2(1.0f, 0.5f) };
	static const float DEAD_ZONE = 0.5f;
}


Screen::Screen() : state_(SCREEN_STATE::DRAW), selectCoolTime_(0)
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
	//�Ō�ɑI��������̓n�C���C�g�\������
	UIBase* boundUI = nullptr;
	UIBase* selectUI = nullptr;
	for (auto u : uiList_) {
		u->Update();
		if (u->GetBound()) boundUI = u;
		if (u->GetSelect()) selectUI = u;
	}

	//�Z���N�g�ړ��_���Ȃ�A�b�v�f�[�g���ďI���
	if (!selectUI->GetSelectPossible()) {
		selectUI->SelectUpdate();
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
		if (stick.x > DEAD_ZONE) dir = 0;		//�E
		else if (stick.x < -DEAD_ZONE) dir = 1;	//��
		else if (stick.y > DEAD_ZONE) dir = 2;		//��
		else if (stick.y < -DEAD_ZONE) dir = 3;	//��
		//�I��������
		if (dir >= 0) selectCoolTime_ = 10;
	}

	//�ړ�����ꏊ����
	if (dir >= 0) {
		UIBase* minUI = selectUI;
		float minDist = 9999.9f;
		for (auto u : uiList_) {
			//�����̓X�L�b�v
			if (u == selectUI) continue;

			XMFLOAT3 sUIPos = selectUI->GetPosition();
			XMFLOAT3 tUIPos = u->GetPosition();
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
					minUI = u;
					minDist = dist;
				}
			}
		}

		//�I�ׂ�����X�V
		if (minUI != selectUI) {
			selectUI->SetSelect(false);
			minUI->SetSelect(true);
			selectUI = minUI;
		}
	}

	if (boundUI) {
		//�d�Ȃ��Ă�����������ABound���ĂȂ����Select���O��
		for (auto u : uiList_) {
			if (boundUI != u) {
				u->SetSelect(false);
			}
		}
		boundUI->SetSelect(true);
		boundUI->SelectUpdate();
	}
	else {
		//�Ō�ɑI����������n�C���C�g�\��
		selectUI->SelectUpdate();

	}

}

void Screen::Draw()
{
	for (auto u : uiList_)
	{
		u->Draw();
	}
}

UIBase* Screen::AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, std::function<void()> onClick)
{
	XMFLOAT2 size = { 1.0f, 1.0f };
	return AddUI(name, type, pos, size, onClick);
}

UIBase* Screen::AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick)
{
	return AddUI(name, type, pos, size, onClick, XMFLOAT2(1.0f, 1.0f));
}

UIBase* Screen::AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
{
	//�C���X�^���X����
	UIBase* ui = nullptr;
	switch (type)
	{
	case UI_BUTTON: ui = new ButtonUI(pos, size, onClick, tsize); break;
	case UI_SLIDER: ui = new SliderUI(pos, size, onClick, tsize); break;
	}
	if (!ui) return nullptr;

	//�����ł�������UI�̏������ƒǉ�
	ui->Initialize(name);
	uiList_.push_back(ui);
	return ui;
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
