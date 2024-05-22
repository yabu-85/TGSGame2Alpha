#include "Enemy.h"
#include "../Stage/Stage.h"

#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/Direct3D.h"

namespace {
    bool isFly = false;
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
    AddCollider(new CapsuleCollider(XMFLOAT3(), 1.0f, 3.0f));

}

void Enemy::Update()
{
    //重力
    static float gra = 0.0f;
    if (!isFly) {
        gra += gravity;
        transform_.position_.y -= gra;
    }

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
            OutputDebugString("上\n");
        }
    }
    //下
    else {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + EnemyHeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        Model::RayCast(pStage->GetModelHandle(), &rayData);
        if (rayData.hit && rayData.dist < EnemyHeightSize) {
            transform_.position_.y += EnemyHeightSize - rayData.dist;
            gra = 0.0f;
            OutputDebugString("下\n");
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
                OutputDebugString("右\n");
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
                OutputDebugString("左\n");
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
                OutputDebugString("前\n");
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
                OutputDebugString("後ろ\n");
            }
        }
    }
    
}

void Enemy::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    CollisionDraw();

}

void Enemy::Release()
{
}

void Enemy::OnCollision(GameObject* pTarget)
{
    transform_.position_.y += pushPower;
}
