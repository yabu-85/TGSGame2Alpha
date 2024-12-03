#pragma once
#include "EnemyBase.h"

class CapsuleCollider;
class AnimationController;

enum class STONEGOLEM_ANIMATION {
    ANIM1 = 0,
    ANIM2,
    MAX,
};

const int STONEGOLEM_ANIMATION_DATA[(int)STONEGOLEM_ANIMATION::MAX][2]{
    { 0, 240 },
    { 240, 360 },
};

class StoneGolem : public EnemyBase
{
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

    int boneIndex_[MAX_INDEX * 2];
    int partIndex_[MAX_INDEX * 2];

    CapsuleCollider* pCapsuleCollider_[MAX_INDEX];
    AnimationController* pAnimationController_;

public:
    StoneGolem(GameObject* parent);
    ~StoneGolem();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
};