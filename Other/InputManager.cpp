#include "InputManager.h"
#include "../Engine/Input.h"
#include <array>

namespace InputManager {
	std::array<std::pair<TYPE, int>, CMD_MAX> commandList;

	//enumÇÃèáî‘Ç≈
	const std::array<std::pair<TYPE, int>, CMD_MAX> DEFAULT_COMMAND {
		std::make_pair(KEY, DIK_A),	//MOVE_LEFT
		std::make_pair(KEY, DIK_D),	//MOVE_RIGHT
		std::make_pair(KEY, DIK_W),	//MOVE_UP
		std::make_pair(KEY, DIK_S),	//MOVE_DOWN

		std::make_pair(KEY, DIK_E),		//ACTION
		std::make_pair(KEY, DIK_SPACE),	//JUMP
	};
}

void InputManager::SetDefaultKeyConfig()
{
	//DEFAULT_COMMANDÇ…ê›íË
	for (int i = 0; i < CMD_MAX; i++) {
		commandList[i].first = DEFAULT_COMMAND[i].first;
		commandList[i].second = DEFAULT_COMMAND[i].second;
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

bool InputManager::IsCmd(COMMAND cmd)
{
	if (commandList[cmd].first == KEY) return Input::IsKey(commandList[cmd].second);
	else if (commandList[cmd].first == MOUSE) return Input::IsMouseButton(commandList[cmd].second);
	else if (commandList[cmd].first == SCROLL_UP) return Input::IsUpScroll();
	else if (commandList[cmd].first == SCROLL_DOWN) return Input::IsDownScroll();
	return false;
}

bool InputManager::IsCmdUp(COMMAND cmd)
{
	if (commandList[cmd].first == KEY) return Input::IsKeyUp(commandList[cmd].second);
	else if (commandList[cmd].first == MOUSE) return Input::IsMouseButtonUp(commandList[cmd].second);
	else if (commandList[cmd].first == SCROLL_UP) return Input::IsUpScroll();
	else if (commandList[cmd].first == SCROLL_DOWN) return Input::IsDownScroll();
	return false;
}

bool InputManager::IsCmdDown(COMMAND cmd)
{
	if (commandList[cmd].first == KEY) return Input::IsKeyDown(commandList[cmd].second);
	else if (commandList[cmd].first == MOUSE) return Input::IsMouseButtonDown(commandList[cmd].second);
	else if (commandList[cmd].first == SCROLL_UP) return Input::IsUpScroll();
	else if (commandList[cmd].first == SCROLL_DOWN) return Input::IsDownScroll();
	return false;
}

bool InputManager::CmdWalk() {
	bool up = IsCmd(MOVE_UP);
	bool down = IsCmd(MOVE_DOWN);
	bool right = IsCmd(MOVE_RIGHT);
	bool left = IsCmd(MOVE_LEFT);

	bool walk = up || down || right || left;
	if (!walk) return false;

	//ï°êîâüÇµÇƒÇ¢ÇÈèÍçáÇÃèàóù
	if (up && down && !left && !right) return false;
	else if (left && right && !up && !down) return false;
	else if (up && down && left && right) return false;
	
	return true;
}