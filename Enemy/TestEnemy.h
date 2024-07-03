#pragma once
#include "EnemyBase.h"
#include <vector>

class HealthGauge;
class DamageSystem;
class MoveAction;
class AstarMoveAction;

class TestEnemy : public EnemyBase
{
    static XMFLOAT3 damageUIPos_;

    bool isGround = false;
    bool moveReady_;
    float gravity_ = 0.0f;

    MoveAction* pMoveAction_;
    AstarMoveAction* pAstarMoveAction_;

public:
    TestEnemy(GameObject* parent);
    ~TestEnemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;

    XMFLOAT3 GetDamageUIPos() override { return damageUIPos_; }

};