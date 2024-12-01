#include "InputManager.h"
#include "GameManager.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Engine/Direct3D.h"
#include <array>

namespace InputManager {
	static const float STICK_DEAD_ZONE = 0.01f;		//スティックデッドゾーン
	static const float TRRIGERR_DEAD_ZONE = 0.2f;	//トリガーデッドゾーン

	std::array<std::pair<TYPE, int>, CMD_MAX> padCommandList;
	std::array<std::pair<TYPE, int>, CMD_MAX> keyMouseCommandList;

	//enumの順番で
	const std::array<std::pair<TYPE, int>, CMD_MAX> DEFAULT_KEYMOUSE_COMMAND {
		std::make_pair(KEY, DIK_A),	//MOVE_LEFT
		std::make_pair(KEY, DIK_D),	//MOVE_RIGHT
		std::make_pair(KEY, DIK_W),	//MOVE_UP
		std::make_pair(KEY, DIK_S),	//MOVE_DOWN

		std::make_pair(MOUSE, 0),		//ATTACK
		std::make_pair(MOUSE, 1),		//AIM
		std::make_pair(KEY, DIK_R),		//RELOAD
		std::make_pair(KEY, DIK_E),		//ACTION
		std::make_pair(KEY, DIK_SPACE),	//JUMP
		std::make_pair(KEY, DIK_F),		//VIEW

		std::make_pair(MOUSE, 0),		//SELECT
		std::make_pair(MOUSE, 1),		//RETURN
	};

	const std::array<std::pair<TYPE, int>, CMD_MAX> DEFAULT_PAD_COMMAND {
		std::make_pair(STICKL, 0),	//MOVE_LEFT
		std::make_pair(STICKL, 0),	//MOVE_RIGHT
		std::make_pair(STICKL, 0),	//MOVE_UP
		std::make_pair(STICKL, 0),	//MOVE_DOWN
			
		std::make_pair(TRRIGERR, 0),						//ATTACK
		std::make_pair(TRRIGERL, 0),						//AIM
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_X),		//RELOAD
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_X),		//ACTION
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_B),		//JUMP
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_DPAD_UP),	//VIEW
	
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_A),		//SELECT
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_B),		//RETURN
	};

}

void InputManager::Initialize()
{
	SetDefaultKeyConfig();
}

void InputManager::SetDefaultKeyConfig()
{
	//DEFAULT_COMMANDに設定
	for (int i = 0; i < CMD_MAX; i++) {
		padCommandList[i].first = DEFAULT_PAD_COMMAND[i].first;
		padCommandList[i].second = DEFAULT_PAD_COMMAND[i].second;

		keyMouseCommandList[i].first = DEFAULT_KEYMOUSE_COMMAND[i].first;
		keyMouseCommandList[i].second = DEFAULT_KEYMOUSE_COMMAND[i].second;
	}

}

void InputManager::ChangeCmd(COMMAND cmd, TYPE type, int num)
{
	//すでにあるKeyがある場合は対象を初期化
	for (int i = 0; i < CMD_MAX; i++) {
		if (padCommandList[i].first == type && padCommandList[i].second == num) {
			padCommandList[i].first = KEY;
			padCommandList[i].second = 0;
			break;
		}
	}

	padCommandList[cmd].first = type;
	padCommandList[cmd].second = num;
}

bool InputManager::IsCmd(COMMAND cmd, int id)
{
	if (GameManager::IsPCCtrl()) {
		if(id == GameManager::GetPCCtrlNumber()) {
			if (keyMouseCommandList[cmd].first == KEY && Input::IsKey(keyMouseCommandList[cmd].second)) return true;
			else if (keyMouseCommandList[cmd].first == MOUSE && Input::IsMouseButton(keyMouseCommandList[cmd].second)) return true;
			return false;
		}
		else {
			//PCとコントローラー操作の場合コントローラーの番号は0だからidは0に
			id = 0;
		}
	}

	if (padCommandList[cmd].first == CONTROLLER) return Input::IsPadButton(padCommandList[cmd].second, id);
	else if (padCommandList[cmd].first == TRRIGERL) return (Input::GetPadTrrigerL(id) >= TRRIGERR_DEAD_ZONE);
	else if (padCommandList[cmd].first == TRRIGERR) return (Input::GetPadTrrigerR(id) >= TRRIGERR_DEAD_ZONE);
	else if (padCommandList[cmd].first == STICKL) return (CalculationDistance(Input::GetPadStickL(id)) >= STICK_DEAD_ZONE);
	else if (padCommandList[cmd].first == STICKR) return (CalculationDistance(Input::GetPadStickR(id)) >= STICK_DEAD_ZONE);
	return false;
}

bool InputManager::IsCmdUp(COMMAND cmd, int id)
{
	if (GameManager::IsPCCtrl()) {
		if (id == GameManager::GetPCCtrlNumber()) {
			//PC判定
			if (keyMouseCommandList[cmd].first == KEY && Input::IsKeyUp(keyMouseCommandList[cmd].second)) return true;
			else if (keyMouseCommandList[cmd].first == MOUSE && Input::IsMouseButtonUp(keyMouseCommandList[cmd].second)) return true;
			return false;
		}
		else {
			//コントローラーのため
			id = 0;
		}
	}

	if (padCommandList[cmd].first == CONTROLLER) return Input::IsPadButtonUp(padCommandList[cmd].second, id);
	else if (padCommandList[cmd].first == TRRIGERL) return (Input::GetPadTrrigerL(id) <= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerL(id) >= TRRIGERR_DEAD_ZONE);
	else if (padCommandList[cmd].first == TRRIGERR) return (Input::GetPadTrrigerR(id) <= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerR(id) >= TRRIGERR_DEAD_ZONE);
	else if (padCommandList[cmd].first == STICKL) return (CalculationDistance(Input::GetPadStickL(id)) <= STICK_DEAD_ZONE);
	else if (padCommandList[cmd].first == STICKR) return (CalculationDistance(Input::GetPadStickR(id)) <= STICK_DEAD_ZONE);
	return false;
}

bool InputManager::IsCmdDown(COMMAND cmd, int id)
{
	if (GameManager::IsPCCtrl()) {
		if (id == GameManager::GetPCCtrlNumber()) {
			if (keyMouseCommandList[cmd].first == KEY && Input::IsKeyDown(keyMouseCommandList[cmd].second)) return true;
			else if (keyMouseCommandList[cmd].first == MOUSE && Input::IsMouseButtonDown(keyMouseCommandList[cmd].second)) return true;
			return false;
		}
		else {
			//コントローラーのため
			id = 0;
		}
	}

	//Inputの方にTrrigerのUPDown作ったほうがいいかもね
	if (padCommandList[cmd].first == CONTROLLER) return Input::IsPadButtonDown(padCommandList[cmd].second, id);
	else if (padCommandList[cmd].first == TRRIGERL) return (Input::GetPadTrrigerL(id) >= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerL(id) <= TRRIGERR_DEAD_ZONE);
	else if (padCommandList[cmd].first == TRRIGERR) return (Input::GetPadTrrigerR(id) >= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerR(id) <= TRRIGERR_DEAD_ZONE);
	else if (padCommandList[cmd].first == STICKL) return (CalculationDistance(Input::GetPadStickL(id)) >= STICK_DEAD_ZONE);
	else if (padCommandList[cmd].first == STICKR) return (CalculationDistance(Input::GetPadStickR(id)) >= STICK_DEAD_ZONE);
	return false;
}

bool InputManager::CmdWalk(int id) {
	if (GameManager::IsPCCtrl()) {
		if (id == GameManager::GetPCCtrlNumber()) {
			bool up, down, right, left;
			up = IsCmd(MOVE_UP, id);
			down = IsCmd(MOVE_DOWN, id);
			right = IsCmd(MOVE_RIGHT, id);
			left = IsCmd(MOVE_LEFT, id);

			bool walk = up || down || right || left;
			if (!walk) return false;

			//複数押している場合の処理
			if (up && down && !left && !right) return false;
			else if (left && right && !up && !down) return false;
			else if (up && down && left && right) return false;
			return true;
		}
		else {
			id = 0;
		}
	}

	XMFLOAT3 rStick = Input::GetPadStickL(id);
	if (CalculationDistance(rStick) >= 0.01f) return true;
	
	return false;
}