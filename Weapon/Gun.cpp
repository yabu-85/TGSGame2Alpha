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
#include "../Scene/TestScene.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

#include "../Engine/Input.h"

namespace {
    XMFLOAT3 handOffset = { 0.15f, 0.7f, 0.05f };      // 移動量

}

Gun::Gun(GameObject* parent)
    : GunBase(parent, "Gun")
{
}

Gun::~Gun()
{
}

void Gun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle2.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topPartIndex_, &topBoneIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootPartIndex_, &rootBoneIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    Model::GetPartBoneIndex(hModel_, "PeakRoot", &peekRootPartIndex_, &peekRootBoneIndex_);
    assert(peekRootPartIndex_ >= 0);

    transform_.pParent_ = nullptr;
    pAimCursor_ = new AimCursor();
    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("Gun");
    isFirstPerson_ = false; 
}

void Gun::Update()
{
    //クールタイムを減らす
    coolTime_--;
    pAimCursor_->Update();

    static float rotateY = 0.0f;
    if(Input::IsKey(DIK_V)) rotateY += 1.0f;
    if (Input::IsKey(DIK_B)) rotateY -= 1.0f;

    SetGunHandPosition();
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        Model::SetAnimFrame(hPlayerModel_, 150, 150, 1.0f);
    }
    else {
        Model::SetAnimFrame(hPlayerModel_, 180, 180, 1.0f);
    }

    //リロード中
    if (currentReloadTime_ >= 1) {
        Reload();

        //終わり
        if (currentReloadTime_ <= 0) {
            Model::SetAnimFrame(hModel_, 0, 0, 0.0f);
        }
        return;
    }

    //リロード
    if (InputManager::IsCmdDown(InputManager::RELOAD, playerId_)) {
        PressedReload();
    }

    //通常射撃
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_)) {
        PressedShot();
    }

    //RotateShake戻り処理をTrueに
    if (currentMagazineCount_ <= 0) pPlayer_->GetAim()->SetCameraRotateReturn(true);

}

void Gun::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
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

    ShotBullet<Bullet_Normal>();
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
