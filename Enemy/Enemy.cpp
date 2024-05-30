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
    const float PlayerHeightSize = 1.3f;    //一番上
    XMFLOAT3 start = XMFLOAT3(50.0f, 10.0f, 50.0f);

}

Enemy::Enemy(GameObject* parent)
    : GameObject(parent, "Enemy"), hModel_(-1), gravity_(0.0f)
{
}

Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
    hModel_ = Model::Load("Model/Box.fbx");
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
        gravity_ += gravity;
        transform_.position_.y -= gravity_;
    }

    //移動
    Move();

    //壁との当たり判定
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
        pStage->CellFloarRayCast(pos, &rayData);
        if (rayData.hit && rayData.dist < PlayerHeightSize) {
            transform_.position_.y += PlayerHeightSize - rayData.dist;
            gravity_ = 0.0f;
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

#include "../AI/RouteSearch.h"
void Enemy::Move()
{
    if (targetList_.empty() && rand() % 100 == 0) {
        int random = -1;
        do { random = rand() % RouteSearch::GetNodeList().size(); } while (random == lastTarget);
        targetList_ = RouteSearch::AStar(RouteSearch::GetNodeList(), lastTarget, random);
        if (!targetList_.empty()) {
            //経路表示
            OutputDebugStringA(std::to_string(lastTarget).c_str());
            OutputDebugString(" , ");
            OutputDebugStringA(std::to_string(random).c_str());
            OutputDebugString("\n\n");

            lastTarget = random;
        }
    }

    //移動終了した
    if (targetList_.empty()) {
        return;
    }

    if (targetList_.back().type == EdgeType::JUMP) gravity_ = 0.0f;

    //移動方向計算
    XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
    XMVECTOR vTar = XMLoadFloat3(&targetList_.back().pos);
    XMVECTOR vMove = vTar - vPos;

    //移動スピード抑制
    float moveDist = XMVectorGetX(XMVector3Length(vMove));
    if (moveDist > MoveSpeed) vMove = XMVector3Normalize(vMove) * MoveSpeed;

    if (moveDist <= moveRange_) {
        targetList_.pop_back();
        Update();
        return;
    }

    vPos += vMove;
    XMStoreFloat3(&transform_.position_, vPos);
}
