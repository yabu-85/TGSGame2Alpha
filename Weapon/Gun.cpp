#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Animation/AnimationController.h"
#include "../State/StateManager.h"
#include "../Other/InputManager.h"
#include "../Other/AudioManager.h"
#include "../Other/GameManager.h"
#include "../Scene/PlayScene.h"
#include "../UI/AimCursor.h"

Gun::Gun(GameObject* parent) : GunBase(parent, "Gun")
{
}

Gun::~Gun()
{
}

void Gun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topPartIndex_, &topBoneIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootPartIndex_, &rootBoneIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    transform_.pParent_ = nullptr;
    transform_.position_ = Model::GetBoneAnimPosition(hPlayerFPSModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    pAimCursor_ = new AimCursor();
    PlayScene* scene = static_cast<PlayScene*>(FindObject("PlayScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("Gun");
}

void Gun::Update()
{
    coolTime_--;
    pAimCursor_->Update();

    transform_.position_ = Model::GetBoneAnimPosition(hPlayerFPSModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    //リロード中の処理
    if (currentReloadTime_ > 0) {
        Reload();

        //リロード終了
        if (currentMagazineCount_ == magazineCount_) {
            Model::SetAnimFrame(hModel_, 0, 0, 1.0f);
            
            //プレイヤーのStateがIdleかJumpだったら
            std::string stateName = pPlayer_->GetStateManager()->GetName();
            if (stateName == "Idle") {
                pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
                pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
            }
        }
        return;
    }

    //リロードボタン押した
    if (InputManager::IsCmdDown(InputManager::RELOAD, playerId_)) {
        //成功したら
        if (PressedReload()) {
            //Peeking
            isPeeking_ = false;
            currentPeekTime_ = peekTime_;
        
            //PlayerState
            pPlayer_->GetStateManager()->ChangeState("Reload");

            //終わり
            return;
        }
    }

    //銃弾空だけど撃つボタン押した、リロード
    if (currentMagazineCount_ <= 0 && InputManager::IsCmdDown(InputManager::ATTACK, playerId_))
    {
        //リロードできるなら終わり
        if (PressedReload()) return;
    }

    //のぞき込み処理
    Peeking();

    //通常射撃
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_))
    {
        PressedShot();
    }

    //RotateShake戻り処理をTrueに
    if (currentMagazineCount_ <= 0 || coolTime_ <= 0) pPlayer_->GetAim()->SetCameraRotateReturn(true);

}

void Gun::Draw()
{
    //アニメーション
    if (IsEntered()) Model::AnimStart(hModel_);
    else Model::AnimStop(hModel_);

    //Shadoの場合Return
    Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
    if (type == Direct3D::SHADER_SHADOWMAP) return;

    transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
    int dI = GameManager::GetDrawIndex();
    if (pPlayer_->GetPlayerId() == dI) {

        //ピーク中は画像の表示
        if (isPeeking_) {
            if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
                Model::SetTransform(hModel_, transform_);
                Model::Draw(hModel_);
            }
        }
        else {
            Model::SetTransform(hModel_, transform_);
            Model::Draw(hModel_);
        }
    }
    //相手の表示
    else {
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);
    }
}

void Gun::Release()
{
}

void Gun::PressedShot()
{
    //まだクールタイム中
    if (coolTime_ > 0) return;

    //マガジン計算
    if (currentMagazineCount_ <= 0) return;
    currentMagazineCount_--;

    //弾丸Init
    if (pPlayer_->GetAim()->IsAimFps()) ShotFpsBullet<Bullet_Normal>("Bullet_Normal");
    else ShotBullet<Bullet_Normal>("Bullet_Normal");

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 0.3f), 1);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(false);
    pAimCursor_->Shot();

    AudioManager::Play(AUDIO_TYPE::SMALL_SHOT, 0.5f);

    //ヒットエフェクト
    EFFEKSEERLIB::EFKTransform t;
    Transform transform;
    transform.position_ = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
    transform.rotate_ = transform_.rotate_;
    transform.rotate_.x = -transform.rotate_.x;
    transform.rotate_.y += 180.0f;
    transform.scale_ = transform_.scale_;
    DirectX::XMStoreFloat4x4(&(t.matrix), transform.GetWorldMatrix());
    t.isLoop = false;   //繰り返し
    t.maxFrame = 20;    //80フレーム
    t.speed = 1.0;      //スピード
    EFFEKSEERLIB::gEfk->Play("GUNSHOT", t);
}

bool Gun::PressedReload()
{
    //Playerの状態がどうか
    if (pPlayer_->IsClimb()) return false;

    //すでに満タン
    if (currentMagazineCount_ >= magazineCount_) return false;
    //すでにリロード中
    if (currentReloadTime_ > 0) return false;
    //クールタイムでっす
    if (coolTime_ > 0) return false;

    currentReloadTime_ = reloadTime_;
    coolTime_ = reloadTime_;
    pPlayer_->GetAim()->SetCameraRotateReturn(true);
    Model::SetAnimFrame(hModel_, 0, 200, (float)(200.0f / reloadTime_ * 0.5f) );
    return true;
}
