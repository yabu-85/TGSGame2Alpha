#include "SniperGun.h"
#include "Bullet_Sniper.h"
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

//スナイパー画像URL
//https://www.ac-illust.com/main/detail.php?id=23464746&word=%E7%85%A7%E6%BA%96%E3%83%9E%E3%83%BC%E3%82%AF%E9%BB%92

namespace {
    static const int PEEK_TIME = 15;                            //覗き込みにかかる時間
        
}

SniperGun::SniperGun(GameObject* parent)
    : GunBase(parent, "SniperGun"), hPict_(-1), animTime_(0)
{
}

SniperGun::~SniperGun()
{
}

void SniperGun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle2.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topPartIndex_, &topBoneIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootPartIndex_, &rootBoneIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    hPict_ = Image::Load("Image/SniperPeak.png");
    assert(hPict_ >= 0);

    transform_.pParent_ = nullptr;
    pAimCursor_ = new AimCursor();
    PlayScene* scene = static_cast<PlayScene*>(FindObject("PlayScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("SniperGun");
    isFirstPerson_ = true;
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

        //Peeking
        isPeeking_ = false;
        peekTime_ = PEEK_TIME;

        return;
    }

    //のぞき込み処理
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        peekTime_--;
        
        //覗き込み完了
        if (peekTime_ <= 0) {
            isPeeking_ = true;
        }
    }
    else {
        peekTime_ = PEEK_TIME;
        isPeeking_ = false;
    }

    //通常射撃
    if (InputManager::IsCmdDown(InputManager::ATTACK, playerId_))
    {
        PressedShot();
    }

}

void SniperGun::Draw()
{
    //Shadoの場合Return
    Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
    if (type == Direct3D::SHADER_SHADOWMAP) return;

    int dI = GameManager::GetDrawIndex();
    if (pPlayer_->GetPlayerId() == dI) {

        //ピーク中は画像の表示
        if (isPeeking_) {
            if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
                Transform t;
                Image::SetTransform(hPict_, t);
                Image::Draw(hPict_);
            
                Model::SetTransform(hModel_, transform_);
                Model::Draw(hModel_);
            }
        }
        else {
            transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
            Model::SetTransform(hModel_, transform_);
            Model::Draw(hModel_);
        }
    }
    //相手の表示
    else {
        transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
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
    if (coolTime_ > 0) {
        return;
    }

    //マガジン計算
    if (currentMagazineCount_ <= 0) return;
    currentMagazineCount_--;

    ShotBullet<Bullet_Sniper>();
    pAimCursor_->Shot();

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 3.0f), 3);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(true);

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