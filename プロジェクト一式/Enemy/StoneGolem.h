#pragma once
#include "EnemyBase.h"

class CapsuleCollider;
class AnimationController;

class StoneGolem : public EnemyBase
{
    int boneIndex_;
    int partIndex_;

    CapsuleCollider* pCapsuleCollider_;
    AnimationController* pAnimationController_;

public:
    StoneGolem(GameObject* parent);
    ~StoneGolem();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void CalcDraw() override;

};