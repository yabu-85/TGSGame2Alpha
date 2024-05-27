#pragma once
#include <unordered_map>
#include <string>

class StateBase;
class GameObject;

class StateManager
{
	//���X�e�[�g���A�|�C���^��
	std::unordered_map<std::string, StateBase*> stateMap_;
	StateBase* currentState_;
	GameObject* owner_;
	
public:
	StateManager(GameObject* owner);
	~StateManager();
	void Update();

	//�V�����X�e�[�g��map�ɒǉ�
	void AddState(StateBase* state);
	
	//name�̓o�^����Ă�State�ɐ���
	void ChangeState(const std::string& name);

	GameObject* GetGameObject() { return owner_; }
	std::string GetName();
};

