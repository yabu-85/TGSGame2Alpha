#pragma once
#include <unordered_map>
#include <string>

class StateBase;
class GameObject;

class StateManager
{
	//＜ステート名、ポインタ＞
	std::unordered_map<std::string, StateBase*> stateMap_;
	StateBase* currentState_;
	GameObject* owner_;
	
public:
	StateManager(GameObject* owner);
	~StateManager();
	void Update();

	//新しいステートをmapに追加
	void AddState(StateBase* state);
	
	//nameの登録されてるStateに推移
	void ChangeState(const std::string& name);

	GameObject* GetGameObject() { return owner_; }
	std::string GetName();
};

