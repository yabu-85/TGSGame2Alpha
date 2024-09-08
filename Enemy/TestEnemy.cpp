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
    int orientBoneSize = 0;

}

TestEnemy::TestEnemy(GameObject* parent)
    : EnemyBase(parent, "TestEnemy"), pMoveAction_(nullptr), pAstarMoveAction_(nullptr), gravity_(0.0f), moveReady_(false), isGround_(false)
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
    Model::SetAnimFrame(hModel_, 0, 100, 0.2f);

    transform_.position_ = START_POS;
    enemyType_ = ENEMY_TYPE::ENEMY_TEST;
    pMoveAction_ = new MoveAction(this, 0.05f, 0.1f);
    pAstarMoveAction_ = new AstarMoveAction(this, 0.06f, 0.1f);

    SetBodyRange(0.2f);
    SetBodyWeight(0.3f);
    SetBodyHeightHalf(0.8f);

    SetMaxHP(20);
    SetHP(20);

    pHealthGauge_ = new HealthGauge(this);
    pHealthGauge_->SetOffSetPosition(XMFLOAT2(0.0f, 1.7f));

    XMVECTOR vec = { 0.0f, 1.0f, 0.0f, 0.0f };
    CapsuleCollider* collid = new CapsuleCollider(XMFLOAT3(0.0f, 0.85f, 0.0f), 0.4f, 0.5f, vec);
    collid->typeList_.push_back(OBJECT_TYPE::Stage);
    AddCollider(collid);

    //Orient登録
#if 1
    waistPart_ = Model::GetPartIndex(hModel_, "thigh.L");
    //親→子　の順番で追加していく
    std::pair<std::string, std::string> boneNameList[] = {
        { "Bone.001.R", "" },
        { "Bone.003.R", "Bone.001.R"},
        { "Bone.001.L", "" },
        { "Bone.003.L", "Bone.001.L"},
        { "Bone.002", ""},
        { "Bone.003", "Bone.002" },
        { "Bone.005", "Bone.002" },
    };
    orientBoneSize = (int)sizeof(boneNameList) / sizeof(boneNameList[0]);
    for (int i = 0; i < orientBoneSize; i++) {
        waistListIndex_[i] = Model::AddOrientRotateBone(hModel_, boneNameList[i].first, boneNameList[i].second);
    }
    int endTest = 0;
#endif

}

void TestEnemy::Update()
{
    //Dead判定
    if (IsHealthZero()) KillMe();

    //Orientテスト
#if 1
    static const float ORIENT_ROTATE_SPEED = 3.0f;
    if (Input::IsKey(DIK_NUMPAD4)) {
        waistRotateX_ += ORIENT_ROTATE_SPEED;
        for (int i = 0; i < orientBoneSize; i++) Model::SetOrietnRotateBone(hModel_, waistListIndex_[i], XMFLOAT3(waistRotateX_, waistRotateY_, waistRotateZ_));
    }
    if (Input::IsKey(DIK_NUMPAD5)) {
        waistRotateY_ += ORIENT_ROTATE_SPEED;
        for (int i = 0; i < orientBoneSize; i++) Model::SetOrietnRotateBone(hModel_, waistListIndex_[i], XMFLOAT3(waistRotateX_, waistRotateY_, waistRotateZ_));
    }
    if (Input::IsKey(DIK_NUMPAD6)) {
        waistRotateZ_ += ORIENT_ROTATE_SPEED;
        for (int i = 0; i < orientBoneSize; i++) Model::SetOrietnRotateBone(hModel_, waistListIndex_[i], XMFLOAT3(waistRotateX_, waistRotateY_, waistRotateZ_));
    }
    if (Input::IsKeyDown(DIK_NUMPAD7)) {
        Model::SetAnimFrame(hModel_, 0, 0, 0.0f);
        animTime_ = 0;
    }
    if (Input::IsKeyDown(DIK_NUMPAD9)) {
        waistRotateX_ = 0.0f;
        waistRotateY_ = 0.0f;
        waistRotateZ_ = 0.0f;
        for (int i = 0; i < orientBoneSize; i++) Model::SetOrietnRotateBone(hModel_, waistListIndex_[i], XMFLOAT3(waistRotateX_, waistRotateY_, waistRotateZ_));
    }
#endif

    if (Input::IsKeyDown(DIK_NUMPAD8)) {
        int time = Model::GetAnimFrame(hModel_);
        if (time != animTime_) {
            animTime_ = time;
            Model::SetAnimFrame(hModel_, time, time, 0.0f);
        }
        else {
            Model::SetAnimFrame(hModel_, 0, 120, 0.2f);
        }

    }


    if (transform_.position_.y <= -30.0f) {
        KillMe();
        return;
    }

    if (!isGround_) {
        gravity_ += gravity;
        transform_.position_.y -= gravity_;
    }

    //coppy
    {
        ReflectCharacter();

        //壁との当たり判定
        SphereCollider* Scollid = static_cast<SphereCollider*>(colliderList_.front());
        XMVECTOR push = XMVectorZero();
        GameManager::GetCollisionMap()->CellSphereVsTriangle(Scollid, push);

        float addPos = -1.0f;
        isGround_ = false;
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
                isGround_ = true;
            }
            addPos = 0.0f;
        }
    }

    return;




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
    isGround_ = false;
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
            isGround_ = true;
        }
        addPos = 0.0f;
    }

}

void TestEnemy::Draw()
{
    Model::SetShadow(hModel_, false);

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
        float r = (float)GetHP() / (float)GetMaxHP();
        pHealthGauge_->SetParcent(r);
        pHealthGauge_->Draw(GameManager::GetDrawIndex());
    }

    //if (Direct3D::GetCurrentShader() != Direct3D::SHADER_SHADOWMAP) CollisionDraw();

}

void TestEnemy::Release()
{
    Model::Release(hModel_);

}