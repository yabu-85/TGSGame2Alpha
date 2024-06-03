#include "Enemy.h"
#include "../Stage/Stage.h"

#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Direct3D.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../Scene/TestScene.h"

namespace {
    const float gravity = 0.002f;
    const float HeightSize = 1.3f;    //一番上
    XMFLOAT3 start = XMFLOAT3(50.0f, 10.0f, 50.0f);

    float bodyRange_ = 0.5f;
    float bodyWeight_ = 1.0f;
    Player* pPlayer = nullptr;
    CollisionMap* pCMap = nullptr;
}

Enemy::Enemy(GameObject* parent)
    : GameObject(parent, "Enemy"), hModel_(-1), gravity_(0.0f)
{
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    pScene->GetEnemyList().push_back(this);

    pPlayer = static_cast<Player*>(FindObject("Player"));
    pCMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
}

Enemy::~Enemy()
{
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    std::vector<Enemy*> list = pScene->GetEnemyList();
    for (auto it = list.begin(); it != list.end();) {
        if (*it == this) {
            it = list.erase(it);
            break;
        }
        else {
            ++it;
        }
    }
}

void Enemy::Initialize()
{
    hModel_ = Model::Load("Model/Scarecrow.fbx");
    assert(hModel_ >= 0);
    Model::SetAnimFrame(hModel_, 0, 100, 1.0f);
   
    transform_.position_ = start;
    MoveSpeed = 0.075f;

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
    ReflectCharacter();

    //壁との当たり判定
    SphereCollider* collid = static_cast<SphereCollider*>(colliderList_.front());
    XMVECTOR push = XMVectorZero();
    pCMap->CellSphereVsTriangle(collid, push);

    float addPos = -1.0f;
    isGround = false;
    for (int i = 0; i < 2; i++) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + HeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        XMFLOAT3 pos = transform_.position_;
        pos.y += addPos;
        pCMap->CellFloarRayCast(pos, &rayData);
        if (rayData.hit && rayData.dist < HeightSize) {
            transform_.position_.y += HeightSize - rayData.dist;
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
}

#include "../AI/RouteSearch.h"
void Enemy::Move()
{
    static const float RayMoveDist = 10.0f;
    static const float RayHeight = 0.5f;

    XMVECTOR vPos2 = XMLoadFloat3(&transform_.position_);
    XMFLOAT3 playerPos = pPlayer->GetPosition();
    XMVECTOR vPlaPos2 = XMLoadFloat3(&playerPos);
    XMVECTOR vec2 = vPos2 - vPlaPos2;

    float plaDist = XMVectorGetX(XMVector3Length(vec2));
    if (plaDist <= RayMoveDist && plaDist >= 1.0f) {
        targetList_.clear();

        XMFLOAT3 plaPos = pPlayer->GetPosition();
        plaPos.y += 0.01f;
        XMVECTOR vPlaPos = XMLoadFloat3(&plaPos);

        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + RayHeight, transform_.position_.z);
        
        XMVECTOR vec = XMVector3Normalize(vec2) * -1.0f;
        XMFLOAT3 nve = XMFLOAT3();
        XMStoreFloat3(&nve, vec);
        rayData.dir = nve;

        XMFLOAT3 target = XMFLOAT3(pPlayer->GetPosition().x, pPlayer->GetPosition().y + RayHeight, pPlayer->GetPosition().z);
        pCMap->RaySelectCellVsSegment(target, &rayData);
        
        if (rayData.dist >= plaDist) {
            //移動スピード抑制
            float moveDist = XMVectorGetX(XMVector3Length(vec2));
            if (moveDist > MoveSpeed) vec2 = XMVector3Normalize(vec2) * MoveSpeed;

            //回避計算
            CalcDodge(vec2);

            //移動
            vPos2 -= vec2;
            XMStoreFloat3(&transform_.position_, vPos2);
            return;
        }
    }

    if (targetList_.empty() && rand() % 10 == 0) {
        targetList_ = RouteSearch::AStar(RouteSearch::GetNodeList(), pPlayer->GetPosition(), transform_.position_);
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

    //ターゲット位置ついた
    if (moveDist <= moveRange_) {
        targetList_.pop_back();
        Update();
        return;
    }

    //回避計算
    CalcDodge(vMove);
    
    vPos += vMove;
    XMStoreFloat3(&transform_.position_, vPos);
}

void Enemy::CalcDodge(XMVECTOR& move)
{
    const float safeSize = 1.0f;
    XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
    XMVECTOR vSafeMove = XMVectorZero();
    
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    std::vector<Enemy*> list = pScene->GetEnemyList();
    for (auto& e : list) {
        if (e == this) continue;
        XMFLOAT3 f = e->GetPosition();
        XMVECTOR vTarget = XMLoadFloat3(&f);
        XMVECTOR vec = vTarget - vPos;
        float range = XMVectorGetX(XMVector3Length(vec));

        if (range < safeSize) {
            range -= safeSize;
            vSafeMove += XMVector3Normalize(vec) * range;
        }
    }
    move += vSafeMove;
}

void Enemy::ReflectCharacter()
{
    float sY = transform_.position_.y;
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    std::vector<Enemy*> list = pScene->GetEnemyList();
    for (Enemy* c : list) {
        //自分は飛ばす
        if (c == this) continue;
        float oY = c->transform_.position_.y;

        XMFLOAT3 targetPos = c->GetPosition();
        XMFLOAT3 direction = Float3Sub(targetPos, transform_.position_);

        //当たってないなら次
        float addRange = bodyRange_ + bodyRange_;
        float range = CalculationDistance(direction);
        if (range > addRange) continue;

        //押し出しの強さ（0になるの防ぐためにちょい足す）
        float w = bodyWeight_ + bodyWeight_;
        float sWeight = bodyWeight_ / w + 0.001f;
        float oWeight = bodyWeight_ / w + 0.001f;

        //押し出しベクトル計算
        XMFLOAT3 extrusion = Float3Multiply(Float3Normalize(direction), addRange - range);

        //押し出す
        XMFLOAT3 outPos = Float3Multiply(extrusion, -sWeight);
        outPos = Float3Add(outPos, transform_.position_);
        transform_.position_ = outPos;

        outPos = Float3Multiply(extrusion, oWeight);
        outPos = Float3Add(outPos, targetPos);
        c->transform_.position_ = outPos;

        //y座標は戻す
        c->transform_.position_.y = oY;
    }
    transform_.position_.y = sY;
}
