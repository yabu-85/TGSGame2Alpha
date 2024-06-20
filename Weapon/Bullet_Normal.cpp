#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/SegmentCollider.h"
#include "../Engine/PolyLine.h"
#include "../Character/DamageSystem.h"
#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"
#include "../UI/DamageUI.h"
#include "../Stage/CollisionMap.h"
#include "../Other/VFXManager.h"

namespace {
    //当たり判定距離
    static const float CALC_DISTANCE = 30.0f;

}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    : BulletBase(parent, BulletType::NORMAL, "Bullet_Normal"), rayHit_(false), pPolyLine_(nullptr)
{
    // パラメータを取得
    parameter_.damage_ = 5;
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
    parameter_.killTimer_--;
    if (parameter_.killTimer_ <= 0) {
        KillMe();
    }

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
    collid->size_ = XMFLOAT3(CALC_DISTANCE, CALC_DISTANCE, CALC_DISTANCE);
    collid->typeList_.push_back(ObjectType::Enemy);
    AddCollider(collid);

    //コリジョンマップとRayCastで最短距離計算
    RayCastData data;
    data.start = transform_.position_;
    data.dir = move_;
    XMFLOAT3 target = Float3Add(transform_.position_, Float3Multiply(move_, CALC_DISTANCE));
    CollisionMap* cMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
    cMap->RaySelectCellVsSegment(target, &data);

    //敵と当たったか、最短距離を計算
    std::vector<EnemyBase*>& enemyList = EnemyManager::GetAllEnemy();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minEneHitPos = XMFLOAT3();

    for (int i = 0; i < enemyList.size();i++ ) {
        //敵に当たる前に、壁に当たったかどうか（SphereCollidだと仮定して）
        XMFLOAT3 vec = Float3Sub(enemyList[i]->GetPosition(), transform_.position_);
        float hitDist = CalculationDistance(vec);
        if (hitDist > data.dist) continue;

        //壁に当たる距離じゃないから、当たり判定やる
        rayHit_ = false;
        this->Collision(enemyList[i]);

        //最短距離で当たった
        if (rayHit_ && hitDist < minDist) {
            minDist = hitDist;
            minIndex = i;
            minEneHitPos = collid->targetPos_;
        }
    }

    //発射地点のPolyLine追加
    pPolyLine_->AddPosition(transform_.position_);

    //敵に当たった時の処理
    if (minIndex >= 0) {
        VFXManager::CreateVfxSmoke(minEneHitPos);
        pPolyLine_->AddPosition(minEneHitPos);
        pPolyLine_->AddPosition(minEneHitPos);

        //ダメージ与える（HP０以下なら倒すのここでやっとく
        DamageInfo info = DamageInfo(parameter_.damage_);
        enemyList[minIndex]->GetDamageSystem()->ApplyDamageDirectly(info);
        if (enemyList[minIndex]->GetDamageSystem()->IsDead()) enemyList[minIndex]->KillMe();
        enemyList[minIndex]->SetDamageTime(1.0f);

        //ダメージ表示
        XMFLOAT3 damagePos = enemyList[minIndex]->GetPosition();
        damagePos.y += enemyList[minIndex]->GetValueA();
        DamageUI::AddDamage(damagePos, parameter_.damage_);

    }
    //敵に当たらなかった時の処理
    else {
        XMFLOAT3 hitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));
        pPolyLine_->AddPosition(hitPos);
        pPolyLine_->AddPosition(hitPos);

        //コリジョンマップに当たっていた時
        if (data.dist <= CALC_DISTANCE) {
            VFXManager::CreateVfxExplode1(hitPos);
        }
    }

    //削除状態の時余計な処理しないように削除
    ClearCollider();

}
