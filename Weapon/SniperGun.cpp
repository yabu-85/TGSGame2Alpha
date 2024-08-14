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
#include "../Scene/TestScene.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

//スナイパー画像URL
//https://www.ac-illust.com/main/detail.php?id=23464746&word=%E7%85%A7%E6%BA%96%E3%83%9E%E3%83%BC%E3%82%AF%E9%BB%92

namespace {
    static const XMFLOAT3 handOffset = { 0.22f, 0.7f, 0.1f };    //移動量
    static const int PEEK_TIME = 15;                            //覗き込みにかかる時間
    static const float AIM_DISTANCE_INCREASE = 0.2f;            //覗き込みにかかる時間
        
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
    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("SniperGun");
    isFirstPerson_ = true;

    //プレイヤーの手の位置まで調整
    transform_.position_ = handOffset;

}

void SniperGun::Update()
{
    coolTime_--;
    pAimCursor_->Update();
    SetGunHandPosition();

    //アニメーションセット
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        if (animTime_ <= 0) {
            Model::SetAnimFrame(hPlayerModel_, 120, 140, 0.5f);
            Model::SetAnimFrame(pPlayer_->GetFPSModelHandle(), 120, 140, 0.5f);
            animTime_ = 0;
        }
        if (animTime_ == 20) {
            Model::SetAnimFrame(hPlayerModel_, 140, 260, 0.5f);
            int handle = pPlayer_->GetFPSModelHandle();
            Model::SetAnimFrame(handle, 140, 260, 0.5f);
        }

        animTime_++;
    }
    else {
        if (InputManager::IsCmdUp(InputManager::AIM, playerId_)) {
            Model::SetAnimFrame(hPlayerModel_, 260, 280, 0.5f);
            Model::SetAnimFrame(pPlayer_->GetFPSModelHandle(), 260, 280, 0.5f);
            animTime_ = 0;
        }

        if (animTime_ == -20) {
            Model::SetAnimFrame(hPlayerModel_, 0, 120, 0.5f);
            Model::SetAnimFrame(pPlayer_->GetFPSModelHandle(), 0, 120, 0.5f);
        }

        animTime_--;
    }

    //リロード中の処理
    if (currentReloadTime_ > 0) {
        Reload();
        //リロード終了
        if (currentMagazineCount_ == magazineCount_) {
            Model::SetAnimFrame(hModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(pPlayer_->GetFPSModelHandle(), 0, 0, 1.0f);
        }

        return;
    }

    //撃った後の自動リロード
    if (coolTime_ <= 0 && currentMagazineCount_ < magazineCount_) {
        PressedReload();
        //Peeking
        isPeeking_ = false;
        peekTime_ = PEEK_TIME;

        //アニメーション
        Model::SetAnimFrame(hPlayerModel_, 0, 120, 0.5f);
        Model::SetAnimFrame(pPlayer_->GetFPSModelHandle(), 0, 120, 0.5f);
        animTime_ = 0;
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
    int dI = GameManager::GetDrawIndex();
    Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
    if (pPlayer_->GetPlayerId() == dI) {
        //Shadoの場合Return
        if (type == Direct3D::SHADER_SHADOWMAP) return;

        if (isPeeking_) {
            if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
                Transform t;
                Image::SetTransform(hPict_, t);
                Image::Draw(hPict_);
            }
        }
        else {
            transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
            Model::SetTransform(hModel_, transform_);
            Model::Draw(hModel_);
        }
    }
    //自分の影表示はしない
    else if (type != Direct3D::SHADER_SHADOWMAP) {
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