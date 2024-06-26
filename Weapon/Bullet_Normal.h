#pragma once
#include "BulletBase.h"

class PolyLine;

// e’e‚ğŠÇ—‚·‚éƒNƒ‰ƒX
class Bullet_Normal : public BulletBase
{
    bool isHit_;
    XMFLOAT3 hitPos_;
    PolyLine* pPolyLine_;

public:
    Bullet_Normal(GameObject* parent);
    ~Bullet_Normal();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;
    void Shot(EnemyBase* enemy, XMFLOAT3 pos) override;

};