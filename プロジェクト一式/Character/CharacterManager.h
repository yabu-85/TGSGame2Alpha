#pragma once
#include <vector>
class Character;

namespace CharacterManager
{
	std::vector<Character*> GetCharacterList();
	void AddCharacter(Character* c);
	void RemoveCharacter(Character* c);

};

