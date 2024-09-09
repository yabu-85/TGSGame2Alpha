#pragma once
#include "StateBase.h"
#include <DirectXMath.h>
using namespace DirectX;

class Player;

class PlayerIdle : public StateBase
{
	Player* pPlayer_;
public:
	PlayerIdle(StateManager* owner);
	const char* GetName() const override { return "Idle"; }
	void Update() override;
	void OnEnter() override;
};

class PlayerMove : public StateBase
{
	Player* pPlayer_;
public:
	PlayerMove(StateManager* owner);
	const char* GetName() const override { return "Move"; }
	void Update() override;
	void OnEnter() override;
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

class PlayerReload : public StateBase
{
	int time_;
	Player* pPlayer_;
public:
	PlayerReload(StateManager* owner);
	const char* GetName() const override { return "Reload"; }
	void OnEnter() override;
};

class PlayerDead : public StateBase
{
	int time_;
	Player* pPlayer_;
public:
	PlayerDead(StateManager* owner);
	const char* GetName() const override { return "Dead"; }
	void Update() override;
	void OnEnter() override;
};