#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/SegmentCollider.h"
#include "../Engine/PolyLine.h"
#include "../Enemy/EnemyBase.h"
#include "../Scene/TestScene.h"
#include "../Stage/CollisionMap.h"
#include "../Other/VFXManager.h"
#include "../Character/DamageSystem.h"
#include "../Enemy/EnemyManager.h"

namespace {
    //当たり判定距離
    static const float ShotDistance = 30.0f;

}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    : BulletBase(parent, BulletType::NORMAL, "Bullet_Normal"), rayHit_(false), pPolyLine_(nullptr)
{
    // パラメータを取得
    parameter_.damage_ = 1;
    parameter_.shotCoolTime_ = 3;
    parameter_.speed_ = 4.0f;
    parameter_.killTimer_ = 30;
    parameter_.collisionScale_ = 0.0f;
    parameter_.isPenetration_ = 0;
}

Bullet_Normal::~Bullet_Normal()
{
}

void Bullet_Normal::Initialize()
{
    type_ = ObjectType::Bullet;

    pPolyLine_ = new PolyLine;
    pPolyLine_->Load("PolyImage/BulletLine.png");
    pPolyLine_->SetLength(3);
    pPolyLine_->SetWidth(0.1f);
    pPolyLine_->SetMoveAlphaFlag();

}

void Bullet_Normal::Update()
{
    if (parameter_.killTimer_ <= 0) {
        KillMe();
        return;
    }
    parameter_.killTimer_--;

}

void Bullet_Normal::Draw()
{
    pPolyLine_->Draw();

}

void Bullet_Normal::Release()
{
    SAFE_RELEASE(pPolyLine_);
    SAFE_DELETE(pPolyLine_);
}

void Bullet_Normal::OnCollision(GameObject* pTarget)
{
    // 敵に当たったとき
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        rayHit_ = true;
    }
}

void Bullet_Normal::Shot()
{
    //コライダー登録
    SegmentCollider* collid = new SegmentCollider(XMFLOAT3(), XMLoadFloat3(&move_));
    collid->size_ = XMFLOAT3(ShotDistance, ShotDistance, ShotDistance);
    collid->typeList_.push_back(ObjectType::Enemy);
    AddCollider(collid);

    //コリジョンマップとRayCastで最短距離計算
    RayCastData data;
    data.start = transform_.position_;
    data.dir = move_;
    XMFLOAT3 target = Float3Add(transform_.position_, Float3Multiply(move_, ShotDistance));
    CollisionMap* cMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
    cMap->RaySelectCellVsSegment(target, &data);

    std::vector<EnemyBase*>& enemyList = EnemyManager::GetAllEnemy();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minEneHitPos = XMFLOAT3();

    for (int i = 0; i < enemyList.size();i++ ) {

        //敵に当たる前に、壁に当たったかどうか（SphereCollidだと仮定して）
        float PEdist = CalculationDistance(enemyList[i]->GetPosition(), transform_.position_);
        float CoDist = 0.0f;
        
        //Sphereコライダー
        XMFLOAT3 hitPos = Float3Add(data.start, Float3Multiply(data.dir, PEdist));
        CoDist = CalculationDistance(transform_.position_, hitPos);
        if (CoDist > data.dist) continue;
        
        //Capsuleコライダー



        //壁に当たる距離じゃないから、当たり判定やる
        rayHit_ = false;
        this->Collision(enemyList[i]);

        //当たってたら終了（貫通ならいらない）
        if (rayHit_ && CoDist < minDist) {
            minDist = CoDist;
            minIndex = i;
        }
    }

    pPolyLine_->AddPosition(transform_.position_);

    if (minIndex >= 0) {
        VFXManager::CreateVfxExplode1(minEneHitPos);
        
        pPolyLine_->AddPosition(minEneHitPos);
        pPolyLine_->AddPosition(minEneHitPos);

        DamageInfo info = DamageInfo(5);
        enemyList[minIndex]->GetDamageSystem()->ApplyDamageDirectly(info);
        if (enemyList[minIndex]->GetDamageSystem()->IsDead()) {
            enemyList[minIndex]->KillMe();
        }
        else {
            enemyList[minIndex]->SetDamageTime(1.0f);
        }
    }
    else {
        XMFLOAT3 hitPos_ = Float3Add(data.start, Float3Multiply(data.dir, data.dist));
        VFXManager::CreateVfxExplode1(hitPos_);
        pPolyLine_->AddPosition(hitPos_);
        pPolyLine_->AddPosition(hitPos_);

    }

    //削除状態の時余計な処理しないように削除
    ClearCollider();


}
