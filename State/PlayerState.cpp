#include "PlayerState.h"
#include "StateManager.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"

namespace {
	Player* p = nullptr;

}

PlayerWait::PlayerWait(StateManager* owner) : StateBase(owner)
{
	p = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerWait::Update()
{
	//入力
	if (InputManager::CmdWalk(p->GetPlayerId())) owner_->ChangeState("Move");
	else if (InputManager::IsCmdDown(InputManager::JUMP, p->GetPlayerId()) && p->IsReadyJump()) owner_->ChangeState("Jump");

	//処理
	else {
		p->CalcNoMove();
		p->Move();
	}
}

//------------------------------------------------------------------

PlayerMove::PlayerMove(StateManager* owner) : StateBase(owner)
{
}

void PlayerMove::Update()
{
	//入力
	if (InputManager::IsCmdDown(InputManager::JUMP, p->GetPlayerId()) && p->IsReadyJump()) owner_->ChangeState("Jump");
	else if (!InputManager::CmdWalk(p->GetPlayerId())) owner_->ChangeState("Wait");

	//処理
	else {
		p->CalcMove();
		p->Move();
		p->Rotate();
	}
}

//------------------------------------------------------------------

PlayerJump::PlayerJump(StateManager* owner) : StateBase(owner)
{
}

void PlayerJump::OnEnter()
{
	p->Jump();
	if (InputManager::CmdWalk(p->GetPlayerId())) owner_->ChangeState("Move");
	else owner_->ChangeState("Wait");
}

//------------------------------------------------------------------

PlayerClimb::PlayerClimb(StateManager* owner) : StateBase(owner)
{
}

void PlayerClimb::Update()
{
	p->WallClimb();

	//終了処理
	if(!p->IsClimb()) owner_->ChangeState("Wait");
}