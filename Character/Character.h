#pragma once
#include "../Engine/GameObject.h"

class HealthGauge;
class DamageSystem;

class Character : public GameObject
{
    float bodyWeight_;      //めり込み時に使う重さ
    float bodyRange_;       //めり込み時に使う範囲
    float bodyHeightHalf_;  //高さの半分（注視点みたいなので使う)
    XMFLOAT3 movement_;     //移動量を保存するためのやつ

protected:
    HealthGauge* pHealthGauge_;
    DamageSystem* pDamageSystem_;

public:
    Character(GameObject* parent, std::string name);
    virtual ~Character();
    virtual void Initialize() override = 0 {};
    virtual void Update() override = 0;
    virtual void Draw() override = 0 {};
    virtual void Release() override = 0 {};

    //キャラクター同士の衝突の際めり込みを避ける
    void ReflectCharacter();

    float GetBodyRange() { return bodyRange_; }
    float GetBodyWeight() { return bodyWeight_; }
    float GetBodyHeightHalf() { return bodyHeightHalf_; }
    void SetBodyWeight(float f) { bodyWeight_ = f; }
    void SetBodyRange(float f) { bodyRange_ = f; }
    void SetBodyHeightHalf(float f) { bodyHeightHalf_ = f; }

    void SetMovement(XMFLOAT3 move) { movement_ = move; }                   //プレイヤーだけ追加処理ある
    void SetMovement(XMVECTOR move) { XMStoreFloat3(&movement_, move); }    //・・
    XMFLOAT3 GetMovement() { return movement_; }
    XMVECTOR GetMovementVector() { return XMLoadFloat3(&movement_); }
    virtual void ResetMovement() { movement_ = { 0.0f, 0.0f, 0.0f }; };
    
    DamageSystem* GetDamageSystem() { return pDamageSystem_; }
    virtual XMFLOAT3 GetDamageUIPos() = 0;
};