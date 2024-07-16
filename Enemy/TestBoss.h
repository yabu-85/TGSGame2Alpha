#pragma once
#include "EnemyBase.h"

class CapsuleCollider;
class AnimationController;

enum BONE_INDEX {
    HEAD = 0,
    NECK,
    BODY_UP,
    BODY_DOWN,
    TAIL_UP,
    TAIL_DOWN,
    THIGHS_LEFT,
    CALF_LEFT,
    FEET_LEFT,
    THIGHS_RIGHT,
    CALF_RIGHT,
    FEET_RIGHT,
    HAND_LEFT,
    HAND_RIGHT,
    MAX_INDEX,
};

enum class TESTBOSS_ANIMATION {
    ANIM1 = 0,
    ANIM2,
    MAX,
};

const int TESTBOSS_ANIMATION_DATA[(int)TESTBOSS_ANIMATION::MAX][2]{
    { 0, 240 },
    { 240, 360 },
};

class TestBoss : public EnemyBase
{
    static XMFLOAT3 damageUIPos_;

    int boneIndex_[MAX_INDEX * 2];
    int partIndex_[MAX_INDEX * 2];

    CapsuleCollider* pCapsuleCollider_[MAX_INDEX];
    AnimationController* pAnimationController_;

public:
    TestBoss(GameObject* parent);
    ~TestBoss();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    XMFLOAT3 GetDamageUIPos() override { return damageUIPos_; }
};