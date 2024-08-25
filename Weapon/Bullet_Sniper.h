#pragma once
#include "BulletBase.h"

class PolyLine;
class Character;
class CapsuleCollider;

// �e�e���Ǘ�����N���X
class Bullet_Sniper : public BulletBase
{
    bool isHit_;                //Shot�v�Z���ɃR���W�����}�b�v�ɓ������Ă��邩�ǂ���
    bool nextKill_;             //PolyLine�\���p��Kill��x�点�邽�߂̃t���O
    int killWaitTime_;          //PolyLine�\���p�ҋ@����

    float minHitDist_;          //Chara�ɓ����������̍ŒZ�����ۑ��p
    XMFLOAT3 hitPos_;           //�R���W�����}�b�v�ɓ������Ă������̒��e�n�_
    XMFLOAT3 startPos_;         //���˒n�_

    Character* pHitChara_;      //�R���W�����v�Z�œ�������Chara
    PolyLine* pPolyLine_;
    CapsuleCollider* pCapsuleCollider_;

    //�Ώۂɓ����������̃G�t�F�N�g
    void HitEffect();

    //Kill�ҋ@�J�n
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