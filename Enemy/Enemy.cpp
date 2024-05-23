#include "Enemy.h"
#include "../Stage/Stage.h"

#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Direct3D.h"

namespace {
    bool isFly = false;
    float gra = 0.0f;
    const float gravity = 0.0015f;
    const float pushPower = 0.1f;

    const float EnemyHeightSize = 1.3f;    //一番上
    const float EnemyHeadSize = 0.3f;      //首のとこ
    const float EnemyBesideSize = 0.3f;    //横判定サイズ
    const float CheckDistance[2] = { 0.3f, EnemyHeightSize };  //足元、頭
}

Enemy::Enemy(GameObject* parent)
    : GameObject(parent, "Enemy"), hModel_(-1)
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
    hModel_ = Model::Load("Model/desiFiter.fbx");
    assert(hModel_ >= 0);
    Model::SetAnimFrame(hModel_, 0, 100, 1.0f);

#if 1
    XMVECTOR vec = { 1.0f, 0.0f, 0.0f, 0.0f };
    AddCollider(new CapsuleCollider(XMFLOAT3(), 0.5f, 2.2f, vec));
#else
    AddCollider(new SphereCollider(XMFLOAT3(), 1.0f));
#endif

}

void Enemy::Update()
{
    //重力
    if (!isFly) {
        gra += gravity;
        transform_.position_.y -= gra;
    }

    if (transform_.position_.y < 0.0f) {
        gra = 0;
        transform_.position_.y = 0.0f;
    }

    /*
    //ステージ当たり判定
    Stage* pStage = (Stage*)FindObject("Stage");
    //上
    if(gra < 0.0f) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
        Model::RayCastSurface(pStage->GetModelHandle(), &rayData);
        if (rayData.hit && rayData.dist < EnemyHeightSize) {
            transform_.position_.y -= EnemyHeightSize - rayData.dist;
            gra = 0.0f;
        }
    }
    //下
    else {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + EnemyHeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        Model::RayCast(pStage->GetModelHandle(), &rayData);
        if (rayData.hit && rayData.dist < EnemyHeightSize) {
            float d = EnemyHeightSize - rayData.dist;
            transform_.position_.y += d;
            if (transform_.position_.y == 0.0f) {
                int a = 0;
            }
            gra = 0.0f;
        }
    }
    //右
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(1.0f, 0.0f, 0.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < EnemyBesideSize) {
                transform_.position_.x += rayData.dist - EnemyBesideSize;
            }
        }
    }
    //左
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(-1.0f, 0.0f, 0.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < EnemyBesideSize) {
                transform_.position_.x -= rayData.dist - EnemyBesideSize;
            }
        }
    }
    //前
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < EnemyBesideSize) {
                transform_.position_.z += rayData.dist - EnemyBesideSize;
            }
        }
    }
    //後ろ
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(0.0f, 0.0f, -1.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < EnemyBesideSize) {
                transform_.position_.z -= rayData.dist - EnemyBesideSize;
            }
        }
    }
    */
    
    
}

void Enemy::Draw()
{
    Direct3D::EnemyPosition = transform_.position_;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    CollisionDraw();

}

void Enemy::Release()
{
}

void Enemy::OnCollision(GameObject* pTarget)
{
    std::list<Collider*> list = pTarget->GetAllColliderList();
    auto it = colliderList_.begin();
    if ((*it)->GetColliderType() == COLLIDER_CAPSULE) {
        CapsuleCollider* capsule = static_cast<CapsuleCollider*>((*it));
        capsule->targetDit_;

    }

}