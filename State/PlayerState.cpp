#include "PlayerState.h"
#include "StateManager.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Weapon/GunBase.h"
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
	//リロード以外ならIDLEアニメーションに
	if (pPlayer_->GetAnimationController()->GetCurrentAnim() != (int)PLAYER_ANIMATION::RELOAD) {
		pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE);
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
	//pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RUN);
	//pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RUN);

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

PlayerReload::PlayerReload(StateManager* owner) : StateBase(owner), time_(0)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerReload::OnEnter()
{
	//ここ計算する
	int gunReloadTime = pPlayer_->GetGun()->GetReloadTime();
	int reloadS = pPlayer_->GetAnimationController()->GetAnim((int)PLAYER_ANIMATION::RELOAD).startFrame;
	int reloadE = pPlayer_->GetAnimationController()->GetAnim((int)PLAYER_ANIMATION::RELOAD).endFrame;
	int reloadAnimTime = (reloadE - reloadS);
	float animSpeed = (float)reloadAnimTime / (float)gunReloadTime;
	pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RELOAD, animSpeed);
	pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RELOAD, animSpeed);

	//State変更
	int playerId = pPlayer_->GetPlayerId();
	if (InputManager::CmdWalk(playerId)) owner_->ChangeState("Move");
	else owner_->ChangeState("Idle");
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
	pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::DEAD);
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
