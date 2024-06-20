#include "EnemyBase.h"
#include "EnemyManager.h"
#include "HealthGauge.h"
#include "../AI/RouteSearch.h"

#include "../Stage/Stage.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/Input.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Direct3D.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../Character/DamageSystem.h"

namespace {

    const float gravity = 0.002f;
    const float HeightSize = 1.3f;    //àÍî‘è„
    XMFLOAT3 start = XMFLOAT3(50.0f, 10.0f, 50.0f);

    float bodyRange_ = 0.5f;
    float bodyWeight_ = 1.0f;
    Player* pPlayer = nullptr;
    CollisionMap* pCMap = nullptr;
}

EnemyBase::EnemyBase(GameObject* parent)
    : GameObject(parent, "EnemyBase"), hModel_(-1), gravity_(0.0f), pHealthGauge_(nullptr)
{
    pPlayer = static_cast<Player*>(FindObject("Player"));
    pCMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));

}

EnemyBase::~EnemyBase()
{
    std::vector<EnemyBase*>& list = EnemyManager::GetAllEnemy();
    list.erase(std::remove_if(list.begin(), list.end(), [this](EnemyBase* EnemyBase) {
        return EnemyBase == this;
    }), list.end());

}

void EnemyBase::Initialize()
{
    hModel_ = Model::Load("Model/Scarecrow.fbx");
    assert(hModel_ >= 0);
    Model::SetAnimFrame(hModel_, 0, 100, 1.0f);
   
    type_ = ObjectType::Enemy;
    transform_.position_ = start;
    MoveSpeed = 0.075f;

    pHealthGauge_ = new HealthGauge(this);
    pHealthGauge_->SetHeight(1.7f);

    pDamageSystem_ = new DamageSystem();
    pDamageSystem_->SetMaxHP(20);
    pDamageSystem_->SetHP(20);

#if 1
    XMVECTOR vec = { 0.0f, 1.0f, 0.0f, 0.0f };
    CapsuleCollider* collid = new CapsuleCollider(XMFLOAT3(0.0f, 0.85f, 0.0f), 0.5f, 0.4f, vec);
    collid->typeList_.push_back(ObjectType::Stage);
    AddCollider(collid);
#else
    SphereCollider* collid = new SphereCollider(XMFLOAT3(0.0f, 0.7f, 0.0f), 0.5f);
    collid->typeList_.push_back(ObjectType::Stage);
    AddCollider(collid);
#endif

}

void EnemyBase::Update()
{
    if (transform_.position_.y <= -30.0f) {
        KillMe();
        return;
    }

    if (!isGround) {
        gravity_ += gravity;
        transform_.position_.y -= gravity_;
    }

    //à⁄ìÆ
    

    //ï«Ç∆ÇÃìñÇΩÇËîªíË
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

void EnemyBase::Draw()
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

void EnemyBase::Release()
{
    Model::Release(hModel_);

}