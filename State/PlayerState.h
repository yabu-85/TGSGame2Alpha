#pragma once
#include "StateBase.h"
#include <DirectXMath.h>
using namespace DirectX;

class PlayerWait : public StateBase
{
public:
	PlayerWait(StateManager* owner);
	const char* GetName() const override { return "Wait"; }
	void Update() override;
};

class PlayerMove : public StateBase
{
public:
	PlayerMove(StateManager* owner);
	const char* GetName() const override { return "Move"; }
	void Update() override;
};

class PlayerJump : public StateBase
{
public:
	PlayerJump(StateManager* owner);
	const char* GetName() const override { return "Jump"; }
	void OnEnter() override;
};

class PlayerClimb : public StateBase
{
public:
	PlayerClimb(StateManager* owner);
	const char* GetName() const override { return "Climb"; }
	void Update() override;
};