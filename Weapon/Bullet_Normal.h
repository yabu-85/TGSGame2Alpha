#pragma once
#include "BulletBase.h"

class PolyLine;
class Character;
class CapsuleCollider;

//銃弾を管理するクラス
class Bullet_Normal : public BulletBase
{
    bool isHit_;
    bool nextKill_;
    int killWaitTime_;
    float minHitDist_;
    XMFLOAT3 hitPos_;
    XMFLOAT3 startPos_;

    PolyLine* pPolyLine_;
    Character* pHitChara_;
    CapsuleCollider* pCapsuleCollider_;

    //対象に当たった時のエフェクト
    void HitEffect();

    //Kill待機開始
    void KillBullet();

public:
    Bullet_Normal(GameObject* parent);
    ~Bullet_Normal();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;
    void Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos) override;

};