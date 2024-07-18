#pragma once
#include "StateBase.h"
#include <DirectXMath.h>
using namespace DirectX;

class Player;

class PlayerWait : public StateBase
{
	Player* pPlayer_;
public:
	PlayerWait(StateManager* owner);
	const char* GetName() const override { return "Wait"; }
	void Update() override;
};

class PlayerMove : public StateBase
{
	Player* pPlayer_;
public:
	PlayerMove(StateManager* owner);
	const char* GetName() const override { return "Move"; }
	void Update() override;
};

class PlayerJump : public StateBase
{
	Player* pPlayer_;
public:
	PlayerJump(StateManager* owner);
	const char* GetName() const override { return "Jump"; }
	void OnEnter() override;
};

class PlayerClimb : public StateBase
{
	int time_;
	Player* pPlayer_;
public:
	PlayerClimb(StateManager* owner);
	const char* GetName() const override { return "Climb"; }
	void Update() override;
};