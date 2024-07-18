#include "TestEnemy.h"
#include "EnemyManager.h"
#include "../Engine/Model.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Global.h"
#include "../Engine/Input.h"
#include "../Character/DamageSystem.h"
#include "../Stage/CollisionMap.h"
#include "../Action/MoveAction.h"
#include "../Other/GameManager.h"
#include "../Player/Player.h"
#include "../UI/HealthGauge.h"

namespace {
    const float gravity = 0.002f;                   //掛かる重力
    const float HeightSize = 1.3f;                  //一番上

    const XMFLOAT3 START_POS = XMFLOAT3(50.0f, 10.0f, 50.0f);
}

TestEnemy::TestEnemy(GameObject* parent)
    : EnemyBase(parent, "TestEnemy"), pMoveAction_(nullptr), pAstarMoveAction_(nullptr), gravity_(0.0f), moveReady_(false)
{
}

TestEnemy::~TestEnemy() 
{
    SAFE_DELETE(pMoveAction_);
    SAFE_DELETE(pAstarMoveAction_);
}

void TestEnemy::Initialize()
{
    hModel_ = Model::Load("Model/Scarecrow.fbx");
    assert(hModel_ >= 0);

    transform_.position_ = START_POS;
    enemyType_ = ENEMY_TYPE::ENEMY_TEST;
    pMoveAction_ = new MoveAction(this, 0.05f, 0.1f);
    pAstarMoveAction_ = new AstarMoveAction(this, 0.06f, 0.1f);

    SetBodyRange(0.5f);
    SetBodyWeight(0.3f);
    SetBodyHeightHalf(0.8f);
    pHealthGauge_ = new HealthGauge(this);
    pHealthGauge_->SetOffSetPosition(XMFLOAT2(0.0f, 1.7f));
    pDamageSystem_->SetMaxHP(20);
    pDamageSystem_->SetHP(20);

    XMVECTOR vec = { 0.0f, 1.0f, 0.0f, 0.0f };
    CapsuleCollider* collid = new CapsuleCollider(XMFLOAT3(0.0f, 0.85f, 0.0f), 0.4f, 0.5f, vec);
    collid->typeList_.push_back(OBJECT_TYPE::Stage);
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

    if (Input::IsKey(DIK_F)) {
        static const float RAY_HEIGHT = 1.3f;
        static const float TMOVE_DIST = 10.0f;
        XMFLOAT3 plaPos = GameManager::GetPlayer(0)->GetPosition();
        XMFLOAT3 plaVec = Float3Sub(plaPos, transform_.position_);
        float plaDist = CalculationDistance(plaPos, transform_.position_);
        
        if (plaDist <= 1.0f) {
            pAstarMoveAction_->StopMove();
            return;
        }

        //近いと直移動
        if (plaDist <= TMOVE_DIST) {
            //プレイヤーが見えているか、ランダムで計算
            if (rand() % 10 == 0) {
                RayCastData rayData = RayCastData();
                rayData.start = transform_.position_;
                rayData.start.y += RAY_HEIGHT;
                rayData.dir = Float3Normalize(plaVec);

                //Distよりターゲットとの距離のほうが近ければ、見えている
                GameManager::GetCollisionMap()->RaySelectCellVsSegment(plaPos, &rayData);
                moveReady_ = rayData.dist >= plaDist;
            }

            if (moveReady_) {
                pMoveAction_->SetTarget(plaPos);
                pMoveAction_->Update();
            }
        }

        //AStar
        if (plaDist >= 5.0f) {
            pAstarMoveAction_->SetTarget(plaPos);
            if (pAstarMoveAction_->IsOutTarget(5.0f)) pAstarMoveAction_->UpdatePath(plaPos);
            else if (rand() % 100 == 0) pAstarMoveAction_->UpdatePath(plaPos);
        }
        pAstarMoveAction_->Update();
    }

    ReflectCharacter();

    //壁との当たり判定
    SphereCollider* Scollid = static_cast<SphereCollider*>(colliderList_.front());
    XMVECTOR push = XMVectorZero();
    GameManager::GetCollisionMap()->CellSphereVsTriangle(Scollid, push);

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

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
        float r = (float)pDamageSystem_->GetHP() / (float)pDamageSystem_->GetMaxHP();
        pHealthGauge_->SetParcent(r);
        pHealthGauge_->Draw(GameManager::GetDrawIndex());
    }

    CollisionDraw();
}

void TestEnemy::Release()
{
    Model::Release(hModel_);

}