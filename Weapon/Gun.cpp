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

namespace {
    XMFLOAT3 handOffset = { 0.2f, 0.7f, 0.1f };      // 移動量

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
    hModel_ = Model::Load("Model/Rifle.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topBoneIndex_, &topPartIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootBoneIndex_, &rootPartIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    pAimCursor_ = new AimCursor();
    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("Gun");
    isFirstPerson_ = false; 

    //プレイヤーの手の位置まで調整
    transform_.position_ = handOffset;

}

void Gun::Update()
{
    //クールタイムを減らす
    coolTime_--;
    pAimCursor_->Update();

    //リロード中
    if (currentReloadTime_ >= 1) {
        Reload();
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
    transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;

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
    transform.position_ = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    transform.rotate_ = Float3Add(pPlayer_->GetRotate(), transform_.rotate_);
    transform.rotate_.y += +180.0f;
    transform.rotate_.x = -transform.rotate_.x;
    transform.scale_ = transform_.scale_;
    DirectX::XMStoreFloat4x4(&(t.matrix), transform.GetWorldMatrix());
    t.isLoop = false;   //繰り返し
    t.maxFrame = 20;    //80フレーム
    t.speed = 1.0;      //スピード
    EFFEKSEERLIB::gEfk->Play("GUNSHOT", t);
}

void Gun::PressedReload()
{
    //すでに満タン
    if (currentMagazineCount_ >= magazineCount_) return;
    //すでにリロード中
    if (currentReloadTime_ > 0) return;
    //クールタイムでっす
    if (coolTime_ > 0) return;

    currentReloadTime_ = reloadTime_;
    coolTime_ = reloadTime_;
    pPlayer_->GetAim()->SetCameraRotateReturn(true);
}
