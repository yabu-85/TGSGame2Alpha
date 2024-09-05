#include "PlayerState.h"
#include "StateManager.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"

//移動していない場合Orientは0にしておく
//移動している場合はその方向に向ける、上半身はAim方向へ

//Wait/Move
//のぞき込み	
//撃つ
//リロード

//Jump
//撃つ
//リロード

//Climd

PlayerWait::PlayerWait(StateManager* owner) : StateBase(owner)
{
	pPlayer_ = static_cast<Player*>(owner_->GetGameObject());
}

void PlayerWait::Update()
{
	int playerId = pPlayer_->GetPlayerId();

	//のぞき込み
	//リロード
	//撃つ

	//入力
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
	int playerId = pPlayer_->GetPlayerId();
	
	//入力
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
	time_ = 0
		;
	EFFEKSEERLIB::EFKTransform t;
	DirectX::XMStoreFloat4x4(&(t.matrix), pPlayer_->GetWorldMatrix());
	t.isLoop = false;   //繰り返し
	t.maxFrame = 80;    //80フレーム
	t.speed = 1.0;      //スピード
	EFFEKSEERLIB::gEfk->Play("HIT", t);

}

//------------------------------------------------------------------
