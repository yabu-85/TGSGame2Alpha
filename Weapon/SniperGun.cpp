#include "SniperGun.h"
#include "Bullet_Normal.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Image.h"
#include "../Engine/Camera.h"
#include "../Engine/SegmentCollider.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"
#include "../Other/GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Character/Character.h"
#include "../Character/CharacterManager.h"
#include "../UI/AimCursor.h"
#include "../Scene/PlayScene.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

SniperGun::SniperGun(GameObject* parent)
    : GunBase(parent, "SniperGun"), hPict_(-1)
{
}

SniperGun::~SniperGun()
{
}

void SniperGun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topPartIndex_, &topBoneIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootPartIndex_, &rootBoneIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    hPict_ = Image::Load("Image/SniperPeak.png");
    assert(hPict_ >= 0);

    transform_.pParent_ = nullptr;
    transform_.position_ = Model::GetBoneAnimPosition(hPlayerFPSModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    pAimCursor_ = new AimCursor();
    PlayScene* scene = static_cast<PlayScene*>(FindObject("PlayScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("SniperGun");
}

void SniperGun::Update()
{
    coolTime_--;
    pAimCursor_->Update();

    transform_.position_ = Model::GetBoneAnimPosition(hPlayerFPSModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    //アニメーションセット
    if (currentReloadTime_ <= 0) {
        if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
            //覗き込みへの推移
            if (animTime_ <= 0) {
                Model::SetAnimFrame(hPlayerModel_, 120, 140, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 120, 140, 1.0f);
                animTime_ = 0;
            }

            //覗き込み中
            if (animTime_ == 20) {
                Model::SetAnimFrame(hPlayerModel_, 140, 260, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 140, 260, 1.0f);
            }

            animTime_++;
        }
        else {
            //覗き込み終了の推移
            if (InputManager::IsCmdUp(InputManager::AIM, playerId_)) {
                Model::SetAnimFrame(hPlayerModel_, 260, 280, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 260, 280, 1.0f);
                animTime_ = 0;
            }

            //通常状態
            if (animTime_ == -20) {
                Model::SetAnimFrame(hPlayerModel_, 0, 120, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 0, 120, 1.0f);
            }

            animTime_--;
        }
    }

    //リロード中の処理
    if (currentReloadTime_ > 0) {
        Reload();

        animTime_--;
        //覗き込み中
        if (animTime_ == 0) {
            Model::SetAnimFrame(hPlayerModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 0, 0, 1.0f);
        }

        //リロード終了
        if (currentMagazineCount_ == magazineCount_) {
            Model::SetAnimFrame(hModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 0, 0, 1.0f);
            animTime_ = 0;
        }

        return;
    }

    //撃った後の自動リロード
    if (coolTime_ <= 0 && currentMagazineCount_ < magazineCount_) {
        PressedReload();

        //アニメーション
        if (isPeeking_) {
            //覗き込み終了
            Model::SetAnimFrame(hPlayerModel_, 260, 280, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 260, 280, 1.0f);
        }
        else {
            //通常状態
            Model::SetAnimFrame(hPlayerModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 0, 00, 1.0f);
        }
        animTime_ = 20;

        return;
    }

    //のぞき込み処理
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        currentPeekTime_--;

        float zoom = peekZoom_ + ((1.0f - peekZoom_) * (float)currentPeekTime_ / (float)peekTime_);
        Camera::SetPeekFOVZoom(zoom, playerId_);
        
        //覗き込み完了
        if (currentPeekTime_ <= 0) {
            isPeeking_ = true;
        }
    }
    else {
        float zoom = peekZoom_ + ((1.0f - peekZoom_) * (float)currentPeekTime_ / (float)peekTime_);
        Camera::SetPeekFOVZoom(zoom, playerId_);
    }

    //通常射撃
    if (InputManager::IsCmdDown(InputManager::ATTACK, playerId_))
    {
        PressedShot();
    }

}

void SniperGun::Draw()
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
#if 0
                Transform t;
                Image::SetTransform(hPict_, t);
                Image::Draw(hPict_);
#endif
            }
        }

        //常に手前に表示するように
        Direct3D::SetDepthBafferWriteEnable(false);
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);
        Direct3D::SetDepthBafferWriteEnable(true);

    }
    //相手の表示
    else {
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);
    }

}

void SniperGun::Release()
{
}

void SniperGun::PressedShot()
{
    //まだクールタイム中
    if (coolTime_ > 0) return;

    //マガジン計算
    if (currentMagazineCount_ <= 0) return;
    currentMagazineCount_--;

    //弾丸Init
    if (pPlayer_->GetAim()->IsAimFps()) ShotFpsBullet<Bullet_Normal>("Bullet_Sniper");
    else ShotBullet<Bullet_Normal>("Bullet_Sniper");

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 8.0f), 7);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(true);
    pAimCursor_->Shot();

    //ショットエフェクト
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

bool SniperGun::PressedReload()
{
    //すでに満タン
    if (currentMagazineCount_ >= magazineCount_) return false;
    //すでにリロード中
    if (currentReloadTime_ > 0) return false;
    //クールタイムでっす
    if (coolTime_ > 0) return false;

    Model::SetAnimFrame(hModel_, 0, 170, 1.0f);
    currentReloadTime_ = reloadTime_;
    coolTime_ = reloadTime_;
    return true;
}

void SniperGun::ResetReload()
{
    coolTime_ = 0;
    currentReloadTime_ = 0;
    Model::SetAnimFrame(hModel_, 0, 0, 1.0f);
}
