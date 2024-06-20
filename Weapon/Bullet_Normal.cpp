#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/SegmentCollider.h"
#include "../Engine/PolyLine.h"
#include "../Character/DamageSystem.h"
#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"
#include "../UI/DamageUI.h"
#include "../Stage/CollisionMap.h"
#include "../Other/VFXManager.h"

namespace {
    //�����蔻�苗��
    static const float CALC_DISTANCE = 30.0f;

}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    : BulletBase(parent, BulletType::NORMAL, "Bullet_Normal"), rayHit_(false), pPolyLine_(nullptr)
{
    // �p�����[�^���擾
    parameter_.damage_ = 5;
    parameter_.shotCoolTime_ = 3;
    parameter_.speed_ = 4.0f;
    parameter_.killTimer_ = 30;
    parameter_.collisionScale_ = 0.0f;
    parameter_.isPenetration_ = 0;
}

Bullet_Normal::~Bullet_Normal()
{
}

void Bullet_Normal::Initialize()
{
    type_ = ObjectType::Bullet;

    pPolyLine_ = new PolyLine;
    pPolyLine_->Load("PolyImage/BulletLine.png");
    pPolyLine_->SetLength(3);
    pPolyLine_->SetWidth(0.1f);
    pPolyLine_->SetMoveAlphaFlag();

}

void Bullet_Normal::Update()
{
    parameter_.killTimer_--;
    if (parameter_.killTimer_ <= 0) {
        KillMe();
    }

}

void Bullet_Normal::Draw()
{
    pPolyLine_->Draw();
}

void Bullet_Normal::Release()
{
    SAFE_RELEASE(pPolyLine_);
    SAFE_DELETE(pPolyLine_);
}

void Bullet_Normal::OnCollision(GameObject* pTarget)
{
    // �G�ɓ��������Ƃ�
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        rayHit_ = true;
    }
}

void Bullet_Normal::Shot()
{
    //�R���C�_�[�o�^
    SegmentCollider* collid = new SegmentCollider(XMFLOAT3(), XMLoadFloat3(&move_));
    collid->size_ = XMFLOAT3(CALC_DISTANCE, CALC_DISTANCE, CALC_DISTANCE);
    collid->typeList_.push_back(ObjectType::Enemy);
    AddCollider(collid);

    //�R���W�����}�b�v��RayCast�ōŒZ�����v�Z
    RayCastData data;
    data.start = transform_.position_;
    data.dir = move_;
    XMFLOAT3 target = Float3Add(transform_.position_, Float3Multiply(move_, CALC_DISTANCE));
    CollisionMap* cMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
    cMap->RaySelectCellVsSegment(target, &data);

    //�G�Ɠ����������A�ŒZ�������v�Z
    std::vector<EnemyBase*>& enemyList = EnemyManager::GetAllEnemy();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minEneHitPos = XMFLOAT3();

    for (int i = 0; i < enemyList.size();i++ ) {
        //�G�ɓ�����O�ɁA�ǂɓ����������ǂ����iSphereCollid���Ɖ��肵�āj
        XMFLOAT3 vec = Float3Sub(enemyList[i]->GetPosition(), transform_.position_);
        float hitDist = CalculationDistance(vec);
        if (hitDist > data.dist) continue;

        //�ǂɓ����鋗������Ȃ�����A�����蔻����
        rayHit_ = false;
        this->Collision(enemyList[i]);

        //�ŒZ�����œ�������
        if (rayHit_ && hitDist < minDist) {
            minDist = hitDist;
            minIndex = i;
            minEneHitPos = collid->targetPos_;
        }
    }

    //���˒n�_��PolyLine�ǉ�
    pPolyLine_->AddPosition(transform_.position_);

    //�G�ɓ����������̏���
    if (minIndex >= 0) {
        VFXManager::CreateVfxSmoke(minEneHitPos);
        pPolyLine_->AddPosition(minEneHitPos);
        pPolyLine_->AddPosition(minEneHitPos);

        //�_���[�W�^����iHP�O�ȉ��Ȃ�|���̂����ł���Ƃ�
        DamageInfo info = DamageInfo(parameter_.damage_);
        enemyList[minIndex]->GetDamageSystem()->ApplyDamageDirectly(info);
        if (enemyList[minIndex]->GetDamageSystem()->IsDead()) enemyList[minIndex]->KillMe();
        enemyList[minIndex]->SetDamageTime(1.0f);

        //�_���[�W�\��
        XMFLOAT3 damagePos = enemyList[minIndex]->GetPosition();
        damagePos.y += enemyList[minIndex]->GetValueA();
        DamageUI::AddDamage(damagePos, parameter_.damage_);

    }
    //�G�ɓ�����Ȃ��������̏���
    else {
        XMFLOAT3 hitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));
        pPolyLine_->AddPosition(hitPos);
        pPolyLine_->AddPosition(hitPos);

        //�R���W�����}�b�v�ɓ������Ă�����
        if (data.dist <= CALC_DISTANCE) {
            VFXManager::CreateVfxExplode1(hitPos);
        }
    }

    //�폜��Ԃ̎��]�v�ȏ������Ȃ��悤�ɍ폜
    ClearCollider();

}
