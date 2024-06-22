#pragma once
#include "BulletBase.h"

class PolyLine;

// �e�e���Ǘ�����N���X
class Bullet_Normal : public BulletBase
{
    bool rayHit_;
    PolyLine* pPolyLine_;

public:
    Bullet_Normal(GameObject* parent);
    ~Bullet_Normal();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void Shot(EnemyBase* enemy, XMFLOAT3 pos) override;

};