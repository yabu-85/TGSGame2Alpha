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

#include <cmath>
// 回転行列から回転角度 (Radian) を取得する関数
XMFLOAT3 GetRotationFromMatrix(const XMMATRIX& matrix) {
    // 回転行列から必要な値を抽出
    float r11 = matrix.r[0].m128_f32[0];
    float r12 = matrix.r[0].m128_f32[1];
    float r13 = matrix.r[0].m128_f32[2];
    float r21 = matrix.r[1].m128_f32[0];
    float r22 = matrix.r[1].m128_f32[1];
    float r23 = matrix.r[1].m128_f32[2];
    float r33 = matrix.r[2].m128_f32[2];

    XMFLOAT3 rot;

    // Y軸の回転を算出
    rot.y = std::asin(-r13);

    // 特殊ケース: Gimbal Lock の判定
    const float threshold = 1.0f - 1e-6f; // 許容誤差
    if (std::abs(std::cos(rot.y)) > threshold) {
        // 通常ケース
        rot.x = std::atan2(r23, r33);
        rot.z = std::atan2(r12, r11);
    }
    else {
        // Gimbal Lock: ±90度の場合
        rot.x = 0.0f;
        rot.z = (rot.y > 0) ? std::atan2(r21, r22) : -std::atan2(r21, r22);
    }

    // ラジアンを度に変換
    const float radToDeg = 180.0f / XM_PI;
    rot.x *= radToDeg;
    rot.y *= radToDeg;
    rot.z *= radToDeg;

    return rot;
}

namespace {
    const XMFLOAT3 START_POS = XMFLOAT3(50.0f, 5.0f, 50.0f);
    
    XMFLOAT3 CapsuleCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 CapRot = XMFLOAT3();

}

StoneGolem::StoneGolem(GameObject* parent)
    : EnemyBase(parent, "StoneGolemEnemy")
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

    XMVECTOR vec = { 0.0f, 0.0f, 0.0f, 0.0f };
    pCapsuleCollider_ = new CapsuleCollider(CapsuleCenter, 0.2f, 0.4f, vec);
    AddCollider(pCapsuleCollider_);

    Model::GetPartBoneIndex(hModel_, "hand.R", &partIndex_, &boneIndex_);

    Model::CalcDraw(hModel_);
    Model::SetAnimFrame(hModel_, 0, 300, 1.0f);

}



void StoneGolem::Update()
{
    //Dead判定
    if (IsHealthZero()) KillMe();
    Model::Update(hModel_);

    if (Input::IsKey(DIK_T)) transform_.position_.x -= 0.5f;
    if (Input::IsKey(DIK_Y)) transform_.position_.x += 0.5f;
    if (Input::IsKey(DIK_G)) transform_.rotate_.y += 10;
    if (Input::IsKey(DIK_H)) transform_.rotate_.y -= 10;

    if (Input::IsKeyDown(DIK_U)) Model::AnimStop(hModel_);
    if (Input::IsKeyDown(DIK_I)) Model::AnimStart(hModel_);

    if (Input::IsKey(DIK_F)) {
        if (Input::IsKey(DIK_NUMPAD1)) CapRot.x += 3.0f;
        if (Input::IsKey(DIK_NUMPAD2)) CapRot.x -= 3.0f;
        if (Input::IsKey(DIK_NUMPAD4)) CapRot.y += 3.0f;
        if (Input::IsKey(DIK_NUMPAD5)) CapRot.y -= 3.0f;
        if (Input::IsKey(DIK_NUMPAD7)) CapRot.z += 3.0f;
        if (Input::IsKey(DIK_NUMPAD8)) CapRot.z -= 3.0f;
        if (Input::IsKey(DIK_NUMPAD0)) CapRot = XMFLOAT3();
    
    }
    else {
        float mSpeed = 0.1f;
        if (Input::IsKey(DIK_NUMPAD1)) CapsuleCenter.x += mSpeed;
        if (Input::IsKey(DIK_NUMPAD2)) CapsuleCenter.x -= mSpeed;
        if (Input::IsKey(DIK_NUMPAD4)) CapsuleCenter.y += mSpeed;
        if (Input::IsKey(DIK_NUMPAD5)) CapsuleCenter.y -= mSpeed;
        if (Input::IsKey(DIK_NUMPAD7)) CapsuleCenter.z += mSpeed;
        if (Input::IsKey(DIK_NUMPAD8)) CapsuleCenter.z -= mSpeed;
        if (Input::IsKey(DIK_NUMPAD0)) CapsuleCenter = XMFLOAT3();

    }

    XMFLOAT3 cBoneRot = Model::GetBoneAnimRotateAtNow(hModel_, partIndex_, boneIndex_);

    // 初期回転行列を保存
    XMMATRIX initialRotationMatrix = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(CapRot.x),
        XMConvertToRadians(CapRot.y),
        XMConvertToRadians(CapRot.z)
    );

    // 現在の回転行列を取得
    XMMATRIX currentRotationMatrix = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(cBoneRot.x),
        XMConvertToRadians(cBoneRot.y),
        XMConvertToRadians(cBoneRot.z)
    );

    // 初期回転行列を現在の回転行列で回転
    XMMATRIX finalRotationMatrix = XMMatrixMultiply(initialRotationMatrix, currentRotationMatrix);

    // 必要に応じてcorrectedRotationを利用
    XMFLOAT3 boneRot = GetRotationFromMatrix(finalRotationMatrix);
    boneRot = cBoneRot;

    boneRot.y += transform_.rotate_.y;
    pCapsuleCollider_->direction_ = CalculationVectorDirection(boneRot);

    // Center の更新
    pCapsuleCollider_->center_ = Float3Sub(Model::GetBoneAnimPositionAtNow(hModel_, partIndex_, boneIndex_), transform_.position_);
    pCapsuleCollider_->center_ = Float3Add(pCapsuleCollider_->center_, CapsuleCenter);
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
