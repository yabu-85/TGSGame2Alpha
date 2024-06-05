#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/SegmentCollider.h"
#include "../Enemy/Enemy.h"
#include "../Scene/TestScene.h"
#include "../Stage/CollisionMap.h"

namespace
{
    XMFLOAT3 collisionOffset = { 0.0f, 0.0f, 0.0f };        // �����蔻��̈ʒu
    std::string modelName = "Model/Bullet.fbx";            // ���f����
}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    :BulletBase(parent, BulletType::NORMAL, "Bullet_Normal"), hModel_(-1)
{
    // �p�����[�^���擾
    parameter_.damage_ = 1;
    parameter_.shotCoolTime_ = 3;
    parameter_.speed_ = 3.0f;
    parameter_.killTimer_ = 30;
    parameter_.collisionScale_ = 0.2f;
    parameter_.isPenetration_ = 0;
}

Bullet_Normal::~Bullet_Normal()
{
}

void Bullet_Normal::Initialize()
{
    hModel_ = Model::Load(modelName);
    assert(hModel_ >= 0);
    type_ = ObjectType::Bullet;

}

void Bullet_Normal::Update()
{
    //���W�v�Z
    transform_.position_ = Float3Add(transform_.position_, move_);

    parameter_.killTimer_--;
    if (parameter_.killTimer_ <= 0) KillMe();

}

void Bullet_Normal::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

}

void Bullet_Normal::Release()
{
    Model::Release(hModel_);

}

void Bullet_Normal::OnCollision(GameObject* pTarget)
{
    // �G�ɓ��������Ƃ�
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        Enemy* pEnemy = dynamic_cast<Enemy*>(pTarget);
        pEnemy->KillMe();
        KillMe();
    }
}

void Bullet_Normal::Shot()
{
    static const float ShotDistance = 30.0f;

    SegmentCollider* collid = new SegmentCollider(collisionOffset, XMVector3Normalize(XMLoadFloat3(&move_)));
    collid->size_ = XMFLOAT3(ShotDistance, ShotDistance, ShotDistance);
    collid->typeList_.push_back(ObjectType::Enemy);
    AddCollider(collid);

    RayCastData data;
    data.start = transform_.position_;
    data.dir = Float3Normalize(move_);
    XMFLOAT3 target = Float3Add(transform_.position_, Float3Multiply(Float3Normalize(move_), ShotDistance));
    CollisionMap* cMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
    cMap->RaySelectCellVsSegment(target, &data);

    //�����Ŏ��ʂ��Ď��Ԃ��v�Z����
    if (data.hit) {
        parameter_.killTimer_ = data.dist / CalculationDistance(move_);
    }

    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    for (Enemy* e : scene->GetEnemyList()) {
        //�G�ɓ�����O�ɁA�ǂɓ����������ǂ���
        if (CalculationDistance(transform_.position_, e->GetPosition()) > data.dist) continue;
        
        //�ǂɓ����鋗������Ȃ�����A�����蔻����
        this->Collision(e);

        //�������Ă���I��
        if (IsDead()) break;
    }
}
