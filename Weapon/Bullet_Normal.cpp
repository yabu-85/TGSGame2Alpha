#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/SegmentCollider.h"
#include "../Engine/PolyLine.h"
#include "../Enemy/Enemy.h"
#include "../Scene/TestScene.h"
#include "../Stage/CollisionMap.h"
#include "../Other/VFXManager.h"

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
    pPolyLine_->SetLength(5);
    pPolyLine_->SetWidth(0.1f);

}

void Bullet_Normal::Update()
{
    if (parameter_.killTimer_ <= 0) {
        KillMe();
        VFXManager::CreateVfxSmoke(hitPos_);
        return;
    }
    parameter_.killTimer_--;

    //座標計算
    transform_.position_ = Float3Add(transform_.position_, move_);

}

void Bullet_Normal::Draw()
{
    pPolyLine_->AddPosition(transform_.position_);
    pPolyLine_->Draw();

}

void Bullet_Normal::Release()
{
}

#include "../Character/DamageSystem.h"
void Bullet_Normal::OnCollision(GameObject* pTarget)
{
    // 敵に当たったとき
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        Enemy* pEnemy = dynamic_cast<Enemy*>(pTarget);
        DamageInfo info = DamageInfo(5);
        pEnemy->GetDamageSystem()->ApplyDamageDirectly(info);
        if (pEnemy->GetDamageSystem()->IsDead()) {
            pEnemy->KillMe();
        }
        else {
            pEnemy->SetDamageTime(1.0f);
        }
        rayHit_ = true;
    }
}

void Bullet_Normal::Shot()
{
    static const float ShotDistance = 30.0f;
    for(int i = 0;i < 10;i++) pPolyLine_->AddPosition(transform_.position_);

    SegmentCollider* collid = new SegmentCollider(XMFLOAT3(), XMVector3Normalize(XMLoadFloat3(&move_)));
    collid->size_ = XMFLOAT3(ShotDistance, ShotDistance, ShotDistance);
    collid->typeList_.push_back(ObjectType::Enemy);
    AddCollider(collid);

    RayCastData data;
    data.start = transform_.position_;
    data.dir = Float3Normalize(move_);
    XMFLOAT3 target = Float3Add(transform_.position_, Float3Multiply(Float3Normalize(move_), ShotDistance));
    CollisionMap* cMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
    cMap->RaySelectCellVsSegment(target, &data);

    //ここで死ぬって時間を計算する
    int newKillTime = (int)(data.dist / CalculationDistance(move_)) + 1;
    if (data.hit && newKillTime < parameter_.killTimer_) {
        parameter_.killTimer_ = newKillTime;
    }
    hitPos_ = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    std::vector<Enemy*> enemyList = scene->GetEnemyList();
    int minIndex = -1;

    for (Enemy* e : enemyList) {
        //敵に当たる前に、壁に当たったかどうか（SphereCollidだと仮定して）
        XMFLOAT3 eneHitPos = Float3Add(e->GetPosition(), e->GetAllColliderList().front()->center_);
        XMFLOAT3 vec = Float3Sub(e->GetPosition(), transform_.position_);
        vec = Float3Multiply(Float3Normalize(vec), e->GetAllColliderList().front()->size_.x);
        eneHitPos = Float3Add(eneHitPos, vec);
        if (CalculationDistance(transform_.position_, eneHitPos) > data.dist) continue;

        //壁に当たる距離じゃないから、当たり判定やる
        this->Collision(e);

        //当たってたら終了（貫通ならいらない）
        if (rayHit_) {
            VFXManager::CreateVfxSmoke(eneHitPos);
            KillMe();
            break;
        }
    }

    //削除状態の時余計な処理しないように削除
    ClearCollider();

}
