#pragma once
#include "EnemyBase.h"
#include <vector>

class HealthGauge;
class DamageSystem;
class MoveAction;
class AstarMoveAction;

class TestEnemy : public EnemyBase
{
    bool isGround_;
    bool moveReady_;
    float gravity_;

    int animTime_;              //時間保管用
    int waistPart_;             //テストPartのインデックス
    int waistListIndex_[20];     //テストBoneのインデックス
    float waistRotateX_;        //テスト腰を想定したボーンのRotateX
    float waistRotateY_;        //テスト腰を想定したボーンのRotateY
    float waistRotateZ_;        //テスト腰を想定したボーンのRotateZ

    MoveAction* pMoveAction_;
    AstarMoveAction* pAstarMoveAction_;

public:
    TestEnemy(GameObject* parent);
    ~TestEnemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
};