#pragma once
#include "EnemyBase.h"
#include <vector>

class TestEnemy : public EnemyBase
{
    static float valueA;

public:
    TestEnemy(GameObject* parent);
    ~TestEnemy();

    float GetValueA() override { return valueA; }

};