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
    : GunBase(parent, "SniperGun")
{
}

SniperGun::~SniperGun()
{
}

void SniperGun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topBoneIndex_, &topPartIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootBoneIndex_, &rootPartIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    hPict_ = Image::Load("Image/SniperPeak.png");
    assert(hPict_ >= 0);

    pAimCursor_ = new AimCursor();
    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    isFirstPerson_ = true;

    //プレイヤーの手の位置まで調整
    transform_.position_ = handOffset;

}

void SniperGun::Update()
{
    coolTime_--;
    pAimCursor_->Update();

    //FPSかTPSかで判定
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        pPlayer_->GetAim()->SetDistanceIncreaseAmount(AIM_DISTANCE_INCREASE);
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

    // 通常射撃
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_))
    {
        //まだクールタイム中
        if (coolTime_ > 0) {
            pPlayer_->GetAim()->SetCameraRotateReturn(true); 
            return;
        }

        ShootBullet<Bullet_Sniper>();
        pAimCursor_->Shot();

        CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 3.0f), 3);
        pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
        pPlayer_->GetAim()->SetCameraRotateReturn(false);

    }
    else {
        //連続で打つのやめた時だけ、RotateShake戻り処理をTrueに
        pPlayer_->GetAim()->SetCameraRotateReturn(true);

    }

}

void SniperGun::Draw()
{
    if (pPlayer_->GetPlayerId() == GameManager::GetDrawIndex()) {
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
    else {
        transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);
    }

}

void SniperGun::Release()
{
}