#pragma once
#include "BulletBase.h"

class PolyLine;
class Character;

// e’e‚ğŠÇ—‚·‚éƒNƒ‰ƒX
class Bullet_Normal : public BulletBase
{
    bool isHit_;
    XMFLOAT3 hitPos_;
    PolyLine* pPolyLine_;
    
    float minHitDist_;
    Character* pHitChara_;

public:
    Bullet_Normal(GameObject* parent);
    ~Bullet_Normal();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;
    void Shot(Character* chara, XMFLOAT3 pos) override;

};