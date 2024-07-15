#pragma once
#include "EnemyBase.h"

class CapsuleCollider;

class TestBoss : public EnemyBase
{
    static XMFLOAT3 damageUIPos_;

    int boneIndex_[28];
    int partIndex_[28];
    CapsuleCollider* pCapsuleCollider_[14];

public:
    TestBoss(GameObject* parent);
    ~TestBoss();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    XMFLOAT3 GetDamageUIPos() override { return damageUIPos_; }
};