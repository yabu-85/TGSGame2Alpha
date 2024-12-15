#pragma once
#include "EnemyBase.h"

class AnimationController;

class StoneGolem : public EnemyBase
{
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