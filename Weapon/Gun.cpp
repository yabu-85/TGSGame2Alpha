#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"
#include "../Other/AudioManager.h"
#include "../Other/GameManager.h"

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
    transform_.position_ = Model::GetBoneAnimPosition(hFpsPlayerModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    LoadGunJson("Gun");
}

void Gun::Update()
{
    //アニメーション
    Model::Update(hModel_);

    //集弾率回復
    if (coolTime_ <= 0) currentAccuracy_ -= accuracyRecovery_;
    if (currentAccuracy_ < 0.0f) currentAccuracy_ = 0.0f;

    //TransfromSet
    SetFpsPlayerHandGun();
    transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    coolTime_--;
    Peeking();

    //リロード中の処理
    if (currentReloadTime_ > 0) {
        Reload();
        return;
    }

    //リロードボタン押した
    if (InputManager::IsCmdDown(InputManager::RELOAD, playerId_)) {
        //成功したら
        if (PressedReload()) {
            EnterReload();

            //終わり
            return;
        }
    }

    //銃弾空だけど撃つボタン押した、リロード
    if (currentMagazineCount_ <= 0 && InputManager::IsCmdDown(InputManager::ATTACK, playerId_))
    {
        //リロードできるなら終わり
        if (PressedReload()) {
            EnterReload();
            return;
        }
    }

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
    //Shadowの場合Return
    Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
    if (type == Direct3D::SHADER_SHADOWMAP) return;

    //FPSの表示
    if (pPlayer_->GetAim()->IsAimFps() && pPlayer_->GetPlayerId() == GameManager::GetDrawIndex()) {
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);
    }
    //TPSの表示
    else {
        SetTpsPlayerHandGun();
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

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.02f, 0.3f), 1);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(false);

    currentAccuracy_ += accuracyDecrease_;
    if (currentAccuracy_ > maxAccuracy_) currentAccuracy_ = maxAccuracy_;

    AudioManager::Play(AUDIO_TYPE::SMALL_SHOT, 0.5f);
    ShotVFX();

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

    pPlayer_->GetAim()->SetCameraRotateReturn(true);
    currentReloadTime_ = reloadTime_;
    coolTime_ = reloadTime_;
    Model::SetAnimFrame(hModel_, 0, 200, (float)(200.0f / reloadTime_ * 0.5f) );
    return true;
}
