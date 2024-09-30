#include "InputManager.h"
#include "GameManager.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Engine/Direct3D.h"
#include <array>

namespace InputManager {
	static const float STICK_DEAD_ZONE = 0.01f;		//�X�e�B�b�N�f�b�h�]�[��
	static const float TRRIGERR_DEAD_ZONE = 0.2f;	//�g���K�[�f�b�h�]�[��

	std::array<std::pair<TYPE, int>, CMD_MAX> commandList;
	std::array<std::pair<TYPE, int>, CMD_MAX> debugCommandList;

	//enum�̏��Ԃ�
	const std::array<std::pair<TYPE, int>, CMD_MAX> DEFAULT_DEBUG_COMMAND {
		std::make_pair(KEY, DIK_A),	//MOVE_LEFT
		std::make_pair(KEY, DIK_D),	//MOVE_RIGHT
		std::make_pair(KEY, DIK_W),	//MOVE_UP
		std::make_pair(KEY, DIK_S),	//MOVE_DOWN

		std::make_pair(MOUSE, 0),		//ATTACK
		std::make_pair(MOUSE, 1),		//AIM
		std::make_pair(KEY, DIK_R),		//RELOAD
		std::make_pair(KEY, DIK_E),		//ACTION
		std::make_pair(KEY, DIK_SPACE),	//JUMP

		std::make_pair(MOUSE, 0),		//SELECT
		std::make_pair(MOUSE, 1),		//RETURN
	};

	const std::array<std::pair<TYPE, int>, CMD_MAX> DEFAULT_COMMAND {
		std::make_pair(STICKL, 0),	//MOVE_LEFT
		std::make_pair(STICKL, 0),	//MOVE_RIGHT
		std::make_pair(STICKL, 0),	//MOVE_UP
		std::make_pair(STICKL, 0),	//MOVE_DOWN

		std::make_pair(TRRIGERR, 0),						//ATTACK
		std::make_pair(TRRIGERL, 0),						//AIM
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_X),		//RELOAD
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_X),		//ACTION
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_B),		//JUMP
	
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
	//DEFAULT_COMMAND�ɐݒ�
	for (int i = 0; i < CMD_MAX; i++) {
		commandList[i].first = DEFAULT_COMMAND[i].first;
		commandList[i].second = DEFAULT_COMMAND[i].second;

		debugCommandList[i].first = DEFAULT_DEBUG_COMMAND[i].first;
		debugCommandList[i].second = DEFAULT_DEBUG_COMMAND[i].second;
	}

}

void InputManager::ChangeCmd(COMMAND cmd, TYPE type, int num)
{
	//���łɂ���Key������ꍇ�͑Ώۂ�������
	for (int i = 0; i < CMD_MAX; i++) {
		if (commandList[i].first == type && commandList[i].second == num) {
			commandList[i].first = KEY;
			commandList[i].second = 0;
			break;
		}
	}

	commandList[cmd].first = type;
	commandList[cmd].second = num;
}

bool InputManager::IsCmd(COMMAND cmd, int id)
{
	if (GameManager::IsPCCtrl()) {
		if(id == GameManager::GetPCCtrlNumber()) {
			if (debugCommandList[cmd].first == KEY && Input::IsKey(debugCommandList[cmd].second)) return true;
			else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButton(debugCommandList[cmd].second)) return true;
			return false;
		}
		else {
			//PC�ƃR���g���[���[����̏ꍇ�R���g���[���[�̔ԍ���0������id��0��
			id = 0;
		}
	}

	if (commandList[cmd].first == CONTROLLER) return Input::IsPadButton(commandList[cmd].second, id);
	else if (commandList[cmd].first == TRRIGERL) return (Input::GetPadTrrigerL(id) >= TRRIGERR_DEAD_ZONE);
	else if (commandList[cmd].first == TRRIGERR) return (Input::GetPadTrrigerR(id) >= TRRIGERR_DEAD_ZONE);
	else if (commandList[cmd].first == STICKL) return (CalculationDistance(Input::GetPadStickL(id)) >= STICK_DEAD_ZONE);
	else if (commandList[cmd].first == STICKR) return (CalculationDistance(Input::GetPadStickR(id)) >= STICK_DEAD_ZONE);
	return false;
}

bool InputManager::IsCmdUp(COMMAND cmd, int id)
{
	if (GameManager::IsPCCtrl()) {
		if (id == GameManager::GetPCCtrlNumber()) {
			//PC����
			if (debugCommandList[cmd].first == KEY && Input::IsKeyUp(debugCommandList[cmd].second)) return true;
			else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButtonUp(debugCommandList[cmd].second)) return true;
			return false;
		}
		else {
			//�R���g���[���[�̂���
			id = 0;
		}
	}

	if (commandList[cmd].first == CONTROLLER) return Input::IsPadButtonUp(commandList[cmd].second, id);
	else if (commandList[cmd].first == TRRIGERL) return (Input::GetPadTrrigerL(id) <= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerL(id) >= TRRIGERR_DEAD_ZONE);
	else if (commandList[cmd].first == TRRIGERR) return (Input::GetPadTrrigerR(id) <= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerR(id) >= TRRIGERR_DEAD_ZONE);
	else if (commandList[cmd].first == STICKL) return (CalculationDistance(Input::GetPadStickL(id)) <= STICK_DEAD_ZONE);
	else if (commandList[cmd].first == STICKR) return (CalculationDistance(Input::GetPadStickR(id)) <= STICK_DEAD_ZONE);
	return false;
}

bool InputManager::IsCmdDown(COMMAND cmd, int id)
{
	if (GameManager::IsPCCtrl()) {
		if (id == GameManager::GetPCCtrlNumber()) {
			if (debugCommandList[cmd].first == KEY && Input::IsKeyDown(debugCommandList[cmd].second)) return true;
			else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButtonDown(debugCommandList[cmd].second)) return true;
			return false;
		}
		else {
			//�R���g���[���[�̂���
			id = 0;
		}
	}

	//Input�̕���Trriger��UPDown������ق�������������
	if (commandList[cmd].first == CONTROLLER) return Input::IsPadButtonDown(commandList[cmd].second, id);
	else if (commandList[cmd].first == TRRIGERL) return (Input::GetPadTrrigerL(id) >= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerL(id) <= TRRIGERR_DEAD_ZONE);
	else if (commandList[cmd].first == TRRIGERR) return (Input::GetPadTrrigerR(id) >= TRRIGERR_DEAD_ZONE && Input::GetPrePadTrrigerR(id) <= TRRIGERR_DEAD_ZONE);
	else if (commandList[cmd].first == STICKL) return (CalculationDistance(Input::GetPadStickL(id)) >= STICK_DEAD_ZONE);
	else if (commandList[cmd].first == STICKR) return (CalculationDistance(Input::GetPadStickR(id)) >= STICK_DEAD_ZONE);
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

			//���������Ă���ꍇ�̏���
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