#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/PolyLine.h"
#include "../Character/DamageSystem.h"
#include "../Enemy/EnemyBase.h"
#include "../UI/DamageUI.h"
#include "../Stage/CollisionMap.h"
#include "../Other/VFXManager.h"

namespace {
    //当たり判定距離
    static const float CALC_DISTANCE = 30.0f;

}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    : BulletBase(parent, BulletType::NORMAL, "Bullet_Normal"), pPolyLine_(nullptr), rayHit_(false)
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

void Bullet_Normal::Shot(EnemyBase* enemy, XMFLOAT3 pos)
{
    //発射地点のPolyLine追加
    pPolyLine_->AddPosition(transform_.position_);

    //敵に当たった時の処理
    if (enemy) {
        VFXManager::CreateVfxExplodeSmall(pos);
        pPolyLine_->AddPosition(pos);
        pPolyLine_->AddPosition(pos);

        //ダメージ与える（HP０以下なら倒すのここでやっとく
        DamageInfo info = DamageInfo(parameter_.damage_);
        enemy->GetDamageSystem()->ApplyDamageDirectly(info);
        if (enemy->GetDamageSystem()->IsDead()) enemy->KillMe();
        enemy->SetDamageTime(1.0f);

        //ダメージ表示
        XMFLOAT3 damagePos = enemy->GetPosition();
        damagePos = Float3Add(damagePos, enemy->GetDamageUIPos());
        DamageUI::AddDamage(damagePos, parameter_.damage_);
    }
    //敵に当たらなかった時の処理
    else {
        pPolyLine_->AddPosition(pos);
        pPolyLine_->AddPosition(pos);

        //コリジョンマップに当たっていた時
        float hitDist = CalculationDistance(transform_.position_, pos);
        if (hitDist <= CALC_DISTANCE) {
            VFXManager::CreateVfxExplodeSmall(pos);
        }
    }

}
