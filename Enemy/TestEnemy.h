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

    int animTime_;              //���ԕۊǗp
    int waistPart_;             //�e�X�gPart�̃C���f�b�N�X
    int waistListIndex_[20];     //�e�X�gBone�̃C���f�b�N�X
    float waistRotateX_;        //�e�X�g����z�肵���{�[����RotateX
    float waistRotateY_;        //�e�X�g����z�肵���{�[����RotateY
    float waistRotateZ_;        //�e�X�g����z�肵���{�[����RotateZ

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