#include "StoneGolem.h"
#include "EnemyManager.h"
#include "../Engine/Model.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Global.h"
#include "../Engine/Input.h"
#include "../Character/DamageSystem.h"
#include "../Animation/AnimationController.h"
#include "../Animation/AnimationNotify.h"
#include "../UI/FixedHealthGauge.h"
#include "../Other/GameManager.h"
#include "../Other/VFXManager.h"

namespace {
    const XMFLOAT3 START_POS = XMFLOAT3(50.0f, 5.5f, 50.0f);
    
    XMFLOAT3 offsetPosition = XMFLOAT3(0.04f, 0.52f, 0.04f);
    XMFLOAT3 offsetRotation = XMFLOAT3(-88.0f, 0.0f, 0.0f);

}

StoneGolem::StoneGolem(GameObject* parent)
    : EnemyBase(parent, "StoneGolemEnemy"), pAnimationController_(nullptr)
{
}

StoneGolem::~StoneGolem() 
{
}

void StoneGolem::Initialize()
{
    hModel_ = Model::Load("Model/StoneGolem.fbx");
    assert(hModel_ >= 0);

    transform_.position_ = START_POS;
    enemyType_ = ENEMY_TYPE::ENEMY_TEST;

    SetBodyRange(0.5f);
    SetBodyWeight(0.3f);
    SetBodyHeightHalf(0.8f);
    
    SetMaxHP(1000);
    SetHP(1000);

    pHealthGauge_ = new FixedHealthGauge(this);
    pHealthGauge_->SetOffSetPosition(XMFLOAT2(0.2f, 0.8f));

    CapsuleCollider* pCapsuleCollider = new CapsuleCollider(XMFLOAT3(), 0.3f, 0.5f, XMVECTOR());
    AddCollider(pCapsuleCollider);

    Model::AttachColliderToBone(hModel_, pCapsuleCollider, "hand.R", offsetPosition, offsetRotation);

    Model::CalcDraw(hModel_);
    Model::SetAnimFrame(hModel_, 0, 300, 1.0f);
}

void StoneGolem::Update()
{
    //Dead”»’è
    if (IsHealthZero()) {
        KillMe();
        return;
    }

    if (Input::IsKey(DIK_T)) transform_.position_.x -= 0.5f;
    if (Input::IsKey(DIK_Y)) transform_.position_.x += 0.5f;
    if (Input::IsKey(DIK_G)) transform_.rotate_.y += 10;
    if (Input::IsKey(DIK_H)) transform_.rotate_.y -= 10;

    if (Input::IsKeyDown(DIK_U)) Model::AnimStop(hModel_);
    if (Input::IsKeyDown(DIK_I)) Model::AnimStart(hModel_);

    Model::SetTransform(hModel_, transform_);
    Model::Update(hModel_);
}

void StoneGolem::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
        float r = (float)GetHP() / (float)GetMaxHP();
        pHealthGauge_->SetParcent(r);
        pHealthGauge_->Draw(GameManager::GetDrawIndex());
    }

    CollisionDraw();
#if _DEBUG
    Direct3D::SetDepthBafferWriteEnable(false);
    CollisionDraw();
    Direct3D::SetDepthBafferWriteEnable(true);
#endif // _DEBUG

}

void StoneGolem::Release()
{
    Model::Release(hModel_);

}

void StoneGolem::CalcDraw()
{
    Model::CalcDraw(hModel_);
}
