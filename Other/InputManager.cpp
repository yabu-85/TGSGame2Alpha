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
		std::make_pair(CONTROLLER, DIK_A),	//MOVE_LEFT
		std::make_pair(CONTROLLER, DIK_D),	//MOVE_RIGHT
		std::make_pair(CONTROLLER, DIK_W),	//MOVE_UP
		std::make_pair(CONTROLLER, DIK_S),	//MOVE_DOWN

		std::make_pair(CONTROLLER, 0),			//ATTACK
		std::make_pair(CONTROLLER, 1),			//AIM
		std::make_pair(CONTROLLER, DIK_E),		//ACTION
		std::make_pair(CONTROLLER, DIK_SPACE),	//JUMP
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
	}

	if (commandList[cmd].first == KEY) return Input::IsKey(commandList[cmd].second);
	else if (commandList[cmd].first == MOUSE) return Input::IsMouseButton(commandList[cmd].second);
	else if (commandList[cmd].first == SCROLL_UP) return Input::IsUpScroll();
	else if (commandList[cmd].first == SCROLL_DOWN) return Input::IsDownScroll();
	return false;
}

bool InputManager::IsCmdUp(COMMAND cmd, int id)
{
	if (id == 0) {
		if (debugCommandList[cmd].first == KEY && Input::IsKeyUp(debugCommandList[cmd].second)) return true;
		else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButtonUp(debugCommandList[cmd].second)) return true;
	}

	if (commandList[cmd].first == KEY) return Input::IsKeyUp(commandList[cmd].second);
	else if (commandList[cmd].first == MOUSE) return Input::IsMouseButtonUp(commandList[cmd].second);
	return false;
}

bool InputManager::IsCmdDown(COMMAND cmd, int id)
{
	if (id == 0) {
		if (debugCommandList[cmd].first == KEY && Input::IsKeyDown(debugCommandList[cmd].second)) return true;
		else if (debugCommandList[cmd].first == MOUSE && Input::IsMouseButtonDown(debugCommandList[cmd].second)) return true;
	}

	if (commandList[cmd].first == KEY) return Input::IsKeyDown(commandList[cmd].second);
	else if (commandList[cmd].first == MOUSE) return Input::IsMouseButtonDown(commandList[cmd].second);
	else if (commandList[cmd].first == SCROLL_UP) return Input::IsUpScroll();
	else if (commandList[cmd].first == SCROLL_DOWN) return Input::IsDownScroll();
	return false;
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
	
	XMFLOAT3 rStick = Input::GetPadStickR(id);
	if (CalculationDistance(rStick) >= 0.01f) return true;

	return false;
}