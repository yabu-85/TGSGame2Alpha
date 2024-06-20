#include "TestEnemy.h"
#include "HealthGauge.h"

#include "../Engine/Model.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Direct3D.h"
#include "../Character/DamageSystem.h"
#include "../Stage/CollisionMap.h"
#include "../Action/MoveAction.h"
#include "../Other/GameManager.h"

float TestEnemy::valueA = 3.0f;

namespace {
    const float gravity = 0.002f;                   //掛かる重力
    const float HeightSize = 1.3f;                  //一番上
    const XMFLOAT3 start = { 50.0f, 10.0f, 50.0f }; //スタートポジション

}

TestEnemy::TestEnemy(GameObject* parent)
    : EnemyBase(parent), pMoveAction_(nullptr), pAstarMoveAction_(nullptr)
{
    pMoveAction_ = new MoveAction(this, 0.2f, 0.1f);
    pAstarMoveAction_ = new AstarMoveAction(this, 0.2f, 0.1f);
    type_ = ObjectType::Enemy;

}

TestEnemy::~TestEnemy() 
{
}

void TestEnemy::Initialize()
{
    hModel_ = Model::Load("Model/Scarecrow.fbx");
    assert(hModel_ >= 0);

    transform_.position_ = start;

    pHealthGauge_->SetHeight(1.7f);
    pDamageSystem_->SetMaxHP(20);
    pDamageSystem_->SetHP(20);

    XMVECTOR vec = { 0.0f, 1.0f, 0.0f, 0.0f };
    CapsuleCollider* collid = new CapsuleCollider(XMFLOAT3(0.0f, 0.85f, 0.0f), 0.5f, 0.4f, vec);
    collid->typeList_.push_back(ObjectType::Stage);
    AddCollider(collid);

}

void TestEnemy::Update()
{
    if (transform_.position_.y <= -30.0f) {
        KillMe();
        return;
    }

    if (!isGround) {
        gravity_ += gravity;
        transform_.position_.y -= gravity_;
    }

    //移動


    //壁との当たり判定
    SphereCollider* collid = static_cast<SphereCollider*>(colliderList_.front());
    XMVECTOR push = XMVectorZero();
    GameManager::GetCollisionMap()->CellSphereVsTriangle(collid, push);

    float addPos = -1.0f;
    isGround = false;
    for (int i = 0; i < 2; i++) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + HeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        XMFLOAT3 pos = transform_.position_;
        pos.y += addPos;
        GameManager::GetCollisionMap()->CellFloarRayCast(pos, &rayData);
        if (rayData.hit && rayData.dist < HeightSize) {
            transform_.position_.y += HeightSize - rayData.dist;
            gravity_ = 0.0f;
            isGround = true;
        }
        addPos = 0.0f;
    }

}

void TestEnemy::Draw()
{
    Direct3D::EnemyPosition = transform_.position_;

    if (damageTime > 0.0f) damageTime -= 0.1f;
    Direct3D::emphasisTime_ = damageTime;
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
    Direct3D::emphasisTime_ = 0.0f;

    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
        float r = (float)pDamageSystem_->GetHP() / (float)pDamageSystem_->GetMaxHP();
        pHealthGauge_->SetParcent(r);
        pHealthGauge_->Draw();
    }

    CollisionDraw();
}

void TestEnemy::Release()
{
    Model::Release(hModel_);

}
