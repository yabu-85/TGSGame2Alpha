#include "PlayerState.h"
#include "StateManager.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"

PlayerWait::PlayerWait(StateManager* owner) : StateBase(owner)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerWait::Update()
{
	//入力
	int playerId = pPlayer_->GetPlayerId();
	if (InputManager::CmdWalk(playerId)) owner_->ChangeState("Move");
	else if (InputManager::IsCmdDown(InputManager::JUMP, playerId) && pPlayer_->IsReadyJump()) owner_->ChangeState("Jump");

	//処理
	else {
		pPlayer_->CalcNoMove();
		pPlayer_->Move();
	}
}

//------------------------------------------------------------------

PlayerMove::PlayerMove(StateManager* owner) : StateBase(owner)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerMove::Update()
{
	//入力
	int playerId = pPlayer_->GetPlayerId();
	if (InputManager::IsCmdDown(InputManager::JUMP, playerId) && pPlayer_->IsReadyJump()) owner_->ChangeState("Jump");
	else if (!InputManager::CmdWalk(playerId)) owner_->ChangeState("Wait");

	//処理
	else {
		pPlayer_->CalcMove();
		pPlayer_->Move();
		pPlayer_->Rotate();
	}
}

//------------------------------------------------------------------

PlayerJump::PlayerJump(StateManager* owner) : StateBase(owner)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerJump::OnEnter()
{
	int playerId = pPlayer_->GetPlayerId();
	pPlayer_->Jump();

	if (InputManager::CmdWalk(playerId)) owner_->ChangeState("Move");
	else owner_->ChangeState("Wait");
}

//------------------------------------------------------------------

PlayerClimb::PlayerClimb(StateManager* owner) : StateBase(owner), time_(0)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerClimb::Update()
{
	pPlayer_->WallClimb();

	//終了処理
	time_++;
	if (time_ >= 60) {
		pPlayer_->SetClimb(false);
		owner_->ChangeState("Wait");
		return;
	}

	if(!pPlayer_->IsClimb()) owner_->ChangeState("Wait");
}