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
	//����
	if (InputManager::CmdWalk()) owner_->ChangeState("Move");
	else if (InputManager::IsCmdDown(InputManager::JUMP) && p->IsReadyJump()) owner_->ChangeState("Jump");

	//����
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
	//����
	if (InputManager::IsCmdDown(InputManager::JUMP) && p->IsReadyJump()) owner_->ChangeState("Jump");
	else if (!InputManager::CmdWalk()) owner_->ChangeState("Wait");

	//����
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
	if (InputManager::CmdWalk()) owner_->ChangeState("Move");
	else owner_->ChangeState("Wait");
}

//------------------------------------------------------------------

PlayerClimb::PlayerClimb(StateManager* owner) : StateBase(owner)
{
}

void PlayerClimb::Update()
{
	p->WallClimb();

	//�I������
	if(!p->IsClimb()) owner_->ChangeState("Wait");
}