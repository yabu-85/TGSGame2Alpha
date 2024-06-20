#pragma once
#include "EnemyBase.h"
#include <vector>

class HealthGauge;
class DamageSystem;
class MoveAction;
class AstarMoveAction;

class TestEnemy : public EnemyBase
{
    static float valueA;

    MoveAction* pMoveAction_;
    AstarMoveAction* pAstarMoveAction_;

public:
    TestEnemy(GameObject* parent);
    ~TestEnemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    float GetValueA() override { return valueA; }

};