#include "StateManager.h"
#include "StateBase.h"

StateManager::StateManager(GameObject* owner)
	: owner_(owner), currentState_(nullptr)
{
}

StateManager::~StateManager()
{
	for (auto& entry : stateMap_) {
		delete entry.second;
	}
	currentState_ = nullptr;
	stateMap_.clear();

}

void StateManager::Update()
{
    if (currentState_) {
        currentState_->Update();
    }

}

void StateManager::ChangeState(const std::string& name)
{
	// ���݂̃X�e�[�g����o��
	if (currentState_)
	{
		currentState_->OnExit();
	}

	// map����V�����X�e�[�g��T��
	auto iter = stateMap_.find(name);
	if (iter != stateMap_.end())
	{
		currentState_ = iter->second;
		// �V������Ԃɓ���
		currentState_->OnEnter();
	}
	else
	{
		currentState_ = nullptr;
	}

}

void StateManager::AddState(StateBase* state)
{
	stateMap_.emplace(state->GetName(), state);
}

std::string StateManager::GetName()
{
	if (currentState_) return currentState_->GetName();
	else return std::string();
}
