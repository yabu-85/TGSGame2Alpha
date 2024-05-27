#pragma once
class StateManager;

class StateBase
{
protected:
	StateManager* owner_;
public:
	StateBase(StateManager* owner) : owner_(owner) {}
	virtual ~StateBase() {};
	virtual const char* GetName() const = 0;
	virtual void Update() {};
	virtual void OnEnter() {};
	virtual void OnExit() {};
};