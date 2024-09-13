#include "PlayerState.h"
#include "StateManager.h"
#include "../Player/Player.h"
#include "../Other/InputManager.h"
#include "../Animation/AnimationController.h"

PlayerIdle::PlayerIdle(StateManager* owner) : StateBase(owner)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerIdle::Update()
{
	int playerId = pPlayer_->GetPlayerId();

	//入力
	if (InputManager::CmdWalk(playerId)) owner_->ChangeState("Move");
	else if (InputManager::IsCmdDown(InputManager::JUMP, playerId) && pPlayer_->IsReadyJump()) owner_->ChangeState("Jump");

	//処理
	else {
		pPlayer_->CalcNoMove();
		pPlayer_->Move();
	}
}

void PlayerIdle::OnEnter()
{
	//リロード/jump以外ならIDLEアニメーションに
	int animId = pPlayer_->GetUpAnimationController()->GetCurrentAnim();
	if (animId != (int)PLAYER_ANIMATION::RELOAD && animId != (int)PLAYER_ANIMATION::JUMP) {
		pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE);
		pPlayer_->GetDownAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE);
		
		if(!InputManager::IsCmd(InputManager::AIM, pPlayer_->GetPlayerId()))
			pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE);
	}

}

//------------------------------------------------------------------

PlayerMove::PlayerMove(StateManager* owner) : StateBase(owner)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerMove::Update()
{
	int playerId = pPlayer_->GetPlayerId();
	
	//入力
	if (InputManager::IsCmdDown(InputManager::JUMP, playerId) && pPlayer_->IsReadyJump()) owner_->ChangeState("Jump");
	else if (!InputManager::CmdWalk(playerId)) owner_->ChangeState("Idle");

	//処理
	else {
		pPlayer_->CalcMove();
		pPlayer_->Move();
		pPlayer_->Rotate();
	}
}

void PlayerMove::OnEnter()
{
	//ジャンプ以外ならIDLEアニメーションに
	if (pPlayer_->GetUpAnimationController()->GetCurrentAnim() != (int)PLAYER_ANIMATION::JUMP) {
		//pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RUN);
		pPlayer_->GetDownAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RUN);
		//pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RUN);
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
	pPlayer_->GetDownAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::JUMP);

	if (InputManager::CmdWalk(playerId)) owner_->ChangeState("Move");
	else owner_->ChangeState("Idle");
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
		owner_->ChangeState("Idle");
		return;
	}

	if(!pPlayer_->IsClimb()) owner_->ChangeState("Idle");
}

//------------------------------------------------------------------

PlayerDead::PlayerDead(StateManager* owner) : StateBase(owner), time_(0)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerDead::Update()
{
	time_++;

}

void PlayerDead::OnEnter()
{
	time_ = 0;

	//アニメーション
	pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::DEAD);
	pPlayer_->GetDownAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::DEAD);
	pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::DEAD);

	//Effekseer
	Transform pTrans;
	pTrans.position_ = pPlayer_->GetPosition();

	EFFEKSEERLIB::EFKTransform t;
	DirectX::XMStoreFloat4x4(&(t.matrix), pTrans.GetWorldMatrix());
	t.isLoop = false;   //繰り返し
	t.maxFrame = 80;    //80フレーム
	t.speed = 1.0;      //スピード
	EFFEKSEERLIB::gEfk->Play("FEATHER", t);

}
