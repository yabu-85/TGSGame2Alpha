#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/PolyLine.h"
#include "../Character/DamageSystem.h"
#include "../Enemy/EnemyBase.h"
#include "../UI/DamageUI.h"
#include "../Stage/CollisionMap.h"
#include "../Other/VFXManager.h"
#include "../Engine/CapsuleCollider.h"
#include "../Enemy/EnemyManager.h"
#include "../Json/JsonReader.h"

namespace {
    static const int POLY_LENG = 5;
}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    : BulletBase(parent, BulletType::NORMAL, "Bullet_Normal"), pPolyLine_(nullptr), isHit_(false), hitPos_(XMFLOAT3())
{
    // JSONファイル読み込み
    JsonReader::Load("Json/Weapon.json");
    auto& bullet_normal = JsonReader::GetSection("Bullet_Normal");

    // パラメータを取得
    parameter_.damage_ = bullet_normal["damage"];
    parameter_.shotCoolTime_ = bullet_normal["shotCoolTime"];
    parameter_.speed_ = bullet_normal["speed"];
    parameter_.killTimer_ = bullet_normal["killTimer"];
    parameter_.collisionScale_ = bullet_normal["collisionScale"];
    parameter_.isPenetration_ = bullet_normal["isPenetration"];
}

Bullet_Normal::~Bullet_Normal()
{
}

void Bullet_Normal::Initialize()
{
    pPolyLine_ = new PolyLine;
    pPolyLine_->Load("PolyImage/BulletLine.png");
    pPolyLine_->SetLength(POLY_LENG);
    pPolyLine_->SetWidth(0.1f);

}

void Bullet_Normal::Update()
{
    if (parameter_.killTimer_ <= 0) {
        //コリジョンマップに当たっていた場合
        if (isHit_) {
            VFXManager::CreateVfxExplodeSmall(hitPos_);
        }

        KillMe();
        return;
    }
    parameter_.killTimer_--;

    //移動
    transform_.position_ = Float3Add(transform_.position_, move_);
    pPolyLine_->AddPosition(transform_.position_);
}

void Bullet_Normal::OnCollision(GameObject* pTarget)
{
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        //ダメージ与える（HP０以下なら倒すのここでやっとく
        DamageInfo info = DamageInfo(parameter_.damage_);
        EnemyBase* enemy = static_cast<EnemyBase*>(pTarget);
        enemy->GetDamageSystem()->ApplyDamageDirectly(info);
        if (enemy->GetDamageSystem()->IsDead()) enemy->KillMe();
        enemy->SetDamageTime(1.0f);

        //ダメージ表示
        XMFLOAT3 damagePos = enemy->GetPosition();
        damagePos = Float3Add(damagePos, enemy->GetDamageUIPos());
        DamageUI::AddDamage(damagePos, parameter_.damage_);
        
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

void Bullet_Normal::Shot(EnemyBase* enemy, XMFLOAT3 hitPos)
{
    //コライダー情報セット
    XMFLOAT3 colCenter = Float3Multiply(move_, -0.5f);
    CapsuleCollider* collid = new CapsuleCollider(colCenter, parameter_.collisionScale_, parameter_.speed_, -XMLoadFloat3(&move_));
    collid->typeList_.push_back(OBJECT_TYPE::Enemy);
    AddCollider(collid);

    //PolyLine追加
    for (int i = 0; i < POLY_LENG; i++) {
        XMFLOAT3 addPos = Float3Multiply(move_, (float)i / (float)POLY_LENG);
        pPolyLine_->AddPosition(Float3Add(transform_.position_, addPos));
    }
    hitPos_ = hitPos;

    //コリジョンマップに当たっていた時
    float hitDist = CalculationDistance(transform_.position_, hitPos);
    float calcDist = parameter_.speed_ * parameter_.killTimer_;
    if (hitDist <= calcDist) {
        isHit_ = true;

        //KillTime計算
        int newKillTime = (int)(hitDist / CalculationDistance(move_)) + 1;
        if (newKillTime < parameter_.killTimer_) {
            parameter_.killTimer_ = newKillTime;
        }

    }
}