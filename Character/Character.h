#pragma once
#include "../Engine/GameObject.h"

class HealthGauge;
class DamageSystem;

class Character : public GameObject
{
    float bodyWeight_;      //�߂荞�ݎ��Ɏg���d��
    float bodyRange_;       //�߂荞�ݎ��Ɏg���͈�
    float bodyHeightHalf_;  //�����̔����i�����_�݂����Ȃ̂Ŏg��)
    XMFLOAT3 movement_;     //�ړ��ʂ�ۑ����邽�߂̂��

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

    //�L�����N�^�[���m�̏Փ˂̍ۂ߂荞�݂������
    void ReflectCharacter();

    float GetBodyRange() { return bodyRange_; }
    float GetBodyWeight() { return bodyWeight_; }
    float GetBodyHeightHalf() { return bodyHeightHalf_; }
    void SetBodyWeight(float f) { bodyWeight_ = f; }
    void SetBodyRange(float f) { bodyRange_ = f; }
    void SetBodyHeightHalf(float f) { bodyHeightHalf_ = f; }

    void SetMovement(XMFLOAT3 move) { movement_ = move; }                   //�v���C���[�����ǉ���������
    void SetMovement(XMVECTOR move) { XMStoreFloat3(&movement_, move); }    //�E�E
    XMFLOAT3 GetMovement() { return movement_; }
    XMVECTOR GetMovementVector() { return XMLoadFloat3(&movement_); }
    virtual void ResetMovement() { movement_ = { 0.0f, 0.0f, 0.0f }; };
    
    DamageSystem* GetDamageSystem() { return pDamageSystem_; }
    virtual XMFLOAT3 GetDamageUIPos() = 0;
};