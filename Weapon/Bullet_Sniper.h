#pragma once
#include "BulletBase.h"

class PolyLine;
class Character;
class CapsuleCollider;

// 銃弾を管理するクラス
class Bullet_Sniper : public BulletBase
{
    bool isHit_;                //Shot計算時にコリジョンマップに当たっているかどうか
    bool nextKill_;             //PolyLine表示用にKillを遅らせるためのフラグ
    int killWaitTime_;          //PolyLine表示用待機時間

    float minHitDist_;          //Charaに当たった時の最短距離保存用
    XMFLOAT3 hitPos_;           //コリジョンマップに当たっていた時の着弾地点
    XMFLOAT3 startPos_;         //発射地点

    Character* pHitChara_;      //コリジョン計算で当たったChara
    PolyLine* pPolyLine_;
    CapsuleCollider* pCapsuleCollider_;

    //対象に当たった時のエフェクト
    void HitEffect();

    //Kill待機開始
    void KillBullet();

public:
    Bullet_Sniper(GameObject* parent);
    ~Bullet_Sniper();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;
    void Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos) override;

};