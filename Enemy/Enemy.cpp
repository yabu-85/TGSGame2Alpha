#include "Enemy.h"
#include "../Stage/Stage.h"

#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Direct3D.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"

namespace {
    const float gravity = 0.002f;
    const float PlayerHeightSize = 1.3f;    //ˆê”Ôã
    XMFLOAT3 start = XMFLOAT3(50.0f, 10.0f, 50.0f);

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
   
    transform_.position_ = start;
    MoveSpeed = 0.05f + rand() % 100 * 0.001f;

#if 0
    XMVECTOR vec = { 1.0f, 0.0f, 0.0f, 0.0f };
    AddCollider(new CapsuleCollider(XMFLOAT3(), 0.5f, 1.5, vec));
#else
    AddCollider(new SphereCollider(XMFLOAT3(0.0f, 0.7f, 0.0f), 0.5));
#endif

}

void Enemy::Update()
{
    if (!isGround) {
        gra += gravity;
        transform_.position_.y -= gra;
    }

    //ˆÚ“®
    Player* p = static_cast<Player*>(FindObject("Player"));
    XMFLOAT3 move = Float3Sub(p->GetPosition(), transform_.position_);
    float dist = sqrtf(move.x * move.x + move.y * move.y + move.z * move.z);
    if (dist > 1.0f) {
        move = Float3Multiply(Float3Normalize(move), MoveSpeed);
        transform_.position_ = Float3Add(transform_.position_, move);
    }

    //•Ç‚Æ‚Ì“–‚½‚è”»’è
    CollisionMap* pStage = (CollisionMap*)FindObject("CollisionMap");
    SphereCollider* collid = static_cast<SphereCollider*>(colliderList_.front());
    XMVECTOR push = XMVectorZero();
    pStage->CellSphereVsTriangle(collid, push);

    float addPos = -1.0f;
    isGround = false;
    for (int i = 0; i < 2; i++) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerHeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        XMFLOAT3 pos = transform_.position_;
        pos.y += addPos;
        pStage->CellRayCast(pos, &rayData);
        if (rayData.hit && rayData.dist < PlayerHeightSize) {
            transform_.position_.y += PlayerHeightSize - rayData.dist;
            gra = 0.0f;
            isGround = true;
        }
        addPos = 0.0f;
    }
    
}

void Enemy::Draw()
{
    Direct3D::EnemyPosition = transform_.position_;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    //CollisionDraw();

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