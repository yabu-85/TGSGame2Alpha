#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/SegmentCollider.h"
#include "../Engine/PolyLine.h"
#include "../Enemy/Enemy.h"
#include "../Scene/TestScene.h"

namespace
{
    XMFLOAT3 collisionOffset = { 0.0f, 0.0f, 0.0f };        // 当たり判定の位置
    std::string modelName = "Model/Bullet.fbx";            // モデル名
}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    :BulletBase(parent, BulletType::NORMAL, "Bullet_Normal"), hModel_(-1)
{
    // パラメータを取得
    parameter_.damage_ = 1;
    parameter_.shotCoolTime_ = 3;
    parameter_.speed_ = 2.0f;
    parameter_.killTimer_ = 30;
    parameter_.collisionScale_ = 0.2f;
    parameter_.isPenetration_ = 0;
}

Bullet_Normal::~Bullet_Normal()
{
}

void Bullet_Normal::Initialize()
{
    //モデルデータのロード
    hModel_ = Model::Load(modelName);
    assert(hModel_ >= 0);

    type_ = ObjectType::Bullet;
    pPolyLine_ = new DoublePolyLine;
    pPolyLine_->Load("PolyImage/BulletLine.png");

}

void Bullet_Normal::Update()
{
    //弾を飛ばす
    transform_.position_ = Float3Add(transform_.position_, move_);

    XMFLOAT3 pos = Float3Normalize(transform_.position_);
    XMFLOAT3 pos1 = XMFLOAT3(transform_.position_.x + pos.x, transform_.position_.y, transform_.position_.z + pos.z);
    XMFLOAT3 pos2 = XMFLOAT3(transform_.position_.x - pos.x, transform_.position_.y, transform_.position_.z - pos.z);
    pPolyLine_->AddPosition(pos1, pos2);
    
    parameter_.killTimer_--;
    if (parameter_.killTimer_ <= 0) KillMe();

}

void Bullet_Normal::Draw()
{
    pPolyLine_->Draw();

}

void Bullet_Normal::Release()
{
    Model::Release(hModel_);

}

void Bullet_Normal::OnCollision(GameObject* pTarget)
{
    // 敵に当たったとき
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        // すでにこの敵に対してヒット済みの場合は無視
        //if (hitEnemies.find(pTarget) != hitEnemies.end()) return;

        Enemy* pEnemy = dynamic_cast<Enemy*>(pTarget);
        pEnemy->KillMe();

        // 貫通しない場合は弾丸を消す.貫通する場合はヒットを記録
        //if (parameter_.isPenetration_ == 0)  parameter_.killTimer_ = 0;
        //else hitEnemies.insert(pTarget);
    
    }
}

void Bullet_Normal::Shot()
{
#if 0
    SphereCollider* collid = new SphereCollider(collisionOffset, parameter_.collisionScale_);
    collid->typeList_.push_back(ObjectType::Enemy);
#else 
    SegmentCollider* collid = new SegmentCollider(collisionOffset, XMVector3Normalize(XMLoadFloat3(&move_)));
    collid->size_ = XMFLOAT3(30.0f, 30.0f, 30.0f);
    collid->typeList_.push_back(ObjectType::Enemy);
#endif
    AddCollider(collid);

    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    for (Enemy* e : scene->GetEnemyList()) {
        this->Collision(e);
    }
}
