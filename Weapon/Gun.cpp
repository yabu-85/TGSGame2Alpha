#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
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
#include "../Engine/EffekseeLib/EffekseerVFX.h"

#include "../Engine/Input.h"
#include "../Scene/PlayScene.h"

namespace {
    static const int PEEK_TIME = 15;                            //覗き込みにかかる時間

}

Gun::Gun(GameObject* parent)
    : GunBase(parent, "Gun"), animTime_(0)
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

    //覗き込みのInput関係はUpとDown使わずに作ったほうがいいかも
    //Pauseをやめた時にキャンセルされない

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
    if (InputManager::IsCmdDown(InputManager::RELOAD, playerId_)) {
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

        //Peeking
        isPeeking_ = false;
        peekTime_ = PEEK_TIME;

        return;
    }

    //のぞき込み処理
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        peekTime_--;

        float zoom = (float)peekTime_ / (float)PEEK_TIME;
        zoom = std::clamp(zoom, 0.7f, 1.0f);
        Camera::SetPeekFOVZoom(zoom, playerId_);

        //覗き込み完了
        if (peekTime_ <= 0) {
            isPeeking_ = true;
        }
    }
    else {
        peekTime_ = PEEK_TIME;
        isPeeking_ = false;
        Camera::SetPeekFOVZoom(1.0f, playerId_);
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
    ShotBullet<Bullet_Normal>("Bullet_Normal");
    pAimCursor_->Shot();

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 0.3f), 1);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(false);

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
