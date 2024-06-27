#include "InputManager.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include <array>

namespace InputManager {
	std::array<std::pair<TYPE, int>, CMD_MAX> commandList;
	std::array<std::pair<TYPE, int>, CMD_MAX> debugCommandList;

	//enumÇÃèáî‘Ç≈
	const std::array<std::pair<TYPE, int>, CMD_MAX> DEFAULT_DEBUG_COMMAND {
		std::make_pair(KEY, DIK_A),	//MOVE_LEFT
		std::make_pair(KEY, DIK_D),	//MOVE_RIGHT
		std::make_pair(KEY, DIK_W),	//MOVE_UP
		std::make_pair(KEY, DIK_S),	//MOVE_DOWN

		std::make_pair(MOUSE, 0),		//ATTACK
		std::make_pair(MOUSE, 1),		//AIM
		std::make_pair(KEY, DIK_E),		//ACTION
		std::make_pair(KEY, DIK_SPACE),	//JUMP
	};

	const std::array<std::pair<TYPE, int>, CMD_MAX> DEFAULT_COMMAND {
		std::make_pair(STICKL, 0),	//MOVE_LEFT
		std::make_pair(STICKL, 0),	//MOVE_RIGHT
		std::make_pair(STICKL, 0),	//MOVE_UP
		std::make_pair(STICKL, 0),	//MOVE_DOWN

		std::make_pair(TRRIGERR, 0),					//ATTACK
		std::make_pair(TRRIGERL, 1),					//AIM
		std::make_pair(CONTROLLER, 0),					//ACTION
		std::make_pair(CONTROLLER, XINPUT_GAMEPAD_B),	//JUMP
	};

}

void InputManager::Initialize()
{
	SetDefaultKeyConfig();

}

void InputManager::SetDefaultKeyConfig()
{
	//DEFAULT_COMMANDÇ…ê›íË
	for (int i = 0; i < CMD_MAX; i++) {
		commandList[i].first = DEFAULT_COMMAND[i].first;
		commandList[i].second = DEFAULT_COMMAND[i].second;

		debugCommandList[i].first = DEFAULT_DEBUG_COMMAND[i].first;
		debugCommandList[i].second = DEFAULT_DEBUG_COMMAND[i].second;
	}

}

void InputManager::ChangeCmd(COMMAND cmd, TYPE type, int num)
{
	//Ç∑Ç≈Ç…Ç†ÇÈKeyÇ™Ç†ÇÈèÍçáÇÕëŒè€Çèâä˙âª
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
	if (id == 0) {
		if (debugCommandList[cmd].first == KEY && Input::IsKey(debugCommandList[cmd].second)) return true;
		else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButton(debugCommandList[cmd].second)) return true;
		else if (debugCommandList[cmd].first == SCROLL_UP && Input::IsUpScroll()) return true;
		else if (debugCommandList[cmd].first == SCROLL_DOWN && Input::IsDownScroll()) return true;
		return false;
	}

	static const float DEAD_ZONE = 0.01f;
	if (commandList[cmd].first == CONTROLLER) return Input::IsPadButton(commandList[cmd].second, 0);
	else if (commandList[cmd].first == TRRIGERL) return (Input::GetPadTrrigerL(0) >= DEAD_ZONE);
	else if (commandList[cmd].first == TRRIGERR) return (Input::GetPadTrrigerR(0) >= DEAD_ZONE);
	else if (commandList[cmd].first == STICKL) return (CalculationDistance(Input::GetPadStickL(0)) >= DEAD_ZONE);
	else if (commandList[cmd].first == STICKR) return (CalculationDistance(Input::GetPadStickR(0)) >= DEAD_ZONE);

}

bool InputManager::IsCmdUp(COMMAND cmd, int id)
{
	if (id == 0) {
		if (debugCommandList[cmd].first == KEY && Input::IsKeyUp(debugCommandList[cmd].second)) return true;
		else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButtonUp(debugCommandList[cmd].second)) return true;
		return false;
	}

	if (commandList[cmd].first == CONTROLLER) return Input::IsPadButtonUp(commandList[cmd].second, 0);

}

bool InputManager::IsCmdDown(COMMAND cmd, int id)
{
	if (id == 0) {
		if (debugCommandList[cmd].first == KEY && Input::IsKeyDown(debugCommandList[cmd].second)) return true;
		else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButtonDown(debugCommandList[cmd].second)) return true;
		return false;
	}

	if (commandList[cmd].first == CONTROLLER) return Input::IsPadButtonDown(commandList[cmd].second, 0);
}

bool InputManager::CmdWalk(int id) {
	bool up, down, right, left;
	if (id == 0) {
		up = IsCmd(MOVE_UP, id);
		down = IsCmd(MOVE_DOWN, id);
		right = IsCmd(MOVE_RIGHT, id);
		left = IsCmd(MOVE_LEFT, id);

		bool walk = up || down || right || left;
		if (!walk) return false;

		//ï°êîâüÇµÇƒÇ¢ÇÈèÍçáÇÃèàóù
		if (up && down && !left && !right) return false;
		else if (left && right && !up && !down) return false;
		else if (up && down && left && right) return false;
		return true;
	}
	
	XMFLOAT3 rStick = Input::GetPadStickL(0);
	if (CalculationDistance(rStick) >= 0.01f) return true;

	return false;
}