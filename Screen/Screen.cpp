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

//最初はGAMEPADで
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
	//UIない場合は終了
	if (uiList_.empty()) return;

	//どの種類で操作しているか判定
	if (latestPCCtrl_) {
		//矢印ボタン || 左スティック押したら切り替え
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

			//最新のやつ切り替え
			uiList_.at(latestSelectUIIndex_)->SetSelect(true);
		}
	}
	else {
		//マウスを動かしたら切り替え
		if (abs(Input::GetMouseMove().x) > PC_CTRL_THRESHOLD || abs(Input::GetMouseMove().y) > PC_CTRL_THRESHOLD) {
			latestPCCtrl_ = true;
			
			//SelectUIIndex計算
			for (int i = 0; i < uiList_.size(); i++) {
				if (uiList_.at(i)->GetSelect()) {
					latestSelectUIIndex_ = i;
					break;
				}
			}
		}
	}

	//全UIのUpdate
	for (auto u : uiList_) {
		u->Update();
	}

	//PCCtrlの場合
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

	//セレクト移動ダメなら終わり
	if (!selectUI->GetSelectPossible()) {
		return;
	}

	//Selectの移動（矢印ボタン
	int dir = -1;
	if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT, 0)) dir = 0;
	else if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_LEFT, 0)) dir = 1;
	else if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_UP, 0)) dir = 2;
	else if (Input::IsPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN, 0)) dir = 3;

	//Selectの移動（スティック
	selectCoolTime_--;
	if (dir <= -1 && selectCoolTime_ <= 0) {
		XMFLOAT3 stick = Input::GetPadStickL(0);
		if (stick.x > GAMEPAD_DEAD_ZONE) dir = 0;		//右
		else if (stick.x < -GAMEPAD_DEAD_ZONE) dir = 1;	//左
		else if (stick.y > GAMEPAD_DEAD_ZONE) dir = 2;	//上
		else if (stick.y < -GAMEPAD_DEAD_ZONE) dir = 3;	//下
		//選択したら
		if (dir >= 0) selectCoolTime_ = 10;
	}

	//移動する場所検索
	if (dir >= 0) {
		int minUIIndex = -1;
		float minDist = 9999.9f;
		int cIndex = 0;
		for (auto u : uiList_) {
			cIndex++;
			//自分はスキップ
			if (u == selectUI) continue;

			XMFLOAT3 sUIPos = selectUI->GetFramePosition();
			XMFLOAT3 tUIPos = u->GetFramePosition();
			XMFLOAT3 vecUI = Float3Sub(tUIPos, sUIPos);

			//入力方向あっているか確認
			if (dir == 0 && (vecUI.x > DIR_LIST[dir].x) ||
				dir == 1 && (vecUI.x < DIR_LIST[dir].x) ||
				dir == 2 && (vecUI.y > DIR_LIST[dir].y) ||
				dir == 3 && (vecUI.y < DIR_LIST[dir].y))
			{
				//入力方向で一番近いやつ選ぶ
				XMFLOAT2 vecDist = XMFLOAT2(vecUI.x * DIR_MULTI[dir].x, vecUI.y * DIR_MULTI[dir].y);
				float dist = sqrt(vecDist.x * vecDist.x + vecDist.y * vecDist.y);
				if (dist < minDist) {
					minUIIndex = cIndex - 1;
					minDist = dist;
				}
			}
		}

		//選べたから更新
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
	//PCCtrlの場合
	if (latestPCCtrl_) {
		for (auto u : uiList_)
		{
			u->SetBound(false);
			u->Draw();
		}
		return;
	}

	//ゲームパッドの場合
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
