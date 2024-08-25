#pragma once
#include "BulletBase.h"

class PolyLine;
class Character;
class CapsuleCollider;

//�e�e���Ǘ�����N���X
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

    //�Ώۂɓ����������̃G�t�F�N�g
    void HitEffect();

    //Kill�ҋ@�J�n
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