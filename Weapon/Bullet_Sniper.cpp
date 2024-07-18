#include "Bullet_Sniper.h"
#include "Gun.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/PolyLine.h"
#include "../Engine/CapsuleCollider.h"
#include "../Json/JsonReader.h"
#include "../Character/DamageSystem.h"
#include "../Character/Character.h"
#include "../Stage/CollisionMap.h"
#include "../Other/GameManager.h"
#include "../Player/Player.h"
#include "../UI/AimCursor.h"
#include "../UI/DamageUI.h"

BulletBase::BulletParameter Bullet_Sniper::parameter_;

namespace {
    static const int POLY_LENG = 30;
}

Bullet_Sniper::Bullet_Sniper(GameObject* parent)
    : BulletBase(parent, "Bullet_Sniper"), pPolyLine_(nullptr), isHit_(false), hitPos_(XMFLOAT3()), startPos_(XMFLOAT3()),
    pHitChara_(nullptr), minHitDist_(99999.9f), nextKill_(false), pCapsuleCollider_(nullptr), killWaitTime_(0)
{
    //JSONファイル読み込み
    JsonReader::Load("Json/Bullet.json");
    auto& Bullet_Sniper = JsonReader::GetSection("Bullet_Sniper");

    //パラメータを取得
    parameter_.damage_ = Bullet_Sniper["damage"];
    parameter_.shotCoolTime_ = Bullet_Sniper["shotCoolTime"];
    parameter_.speed_ = Bullet_Sniper["speed"];
    parameter_.killTimer_ = Bullet_Sniper["killTimer"];
    parameter_.collisionScale_ = Bullet_Sniper["collisionScale"];
    
    if (Bullet_Sniper["isPenetration"] == 0) parameter_.isPenetration_ = false; 
    else parameter_.isPenetration_ = true;

}

Bullet_Sniper::~Bullet_Sniper()
{
}

void Bullet_Sniper::Initialize()
{
    pPolyLine_ = new PolyLine;
    pPolyLine_->Load("PolyImage/BulletLine.png");
    pPolyLine_->SetLength(POLY_LENG);
    pPolyLine_->SetWidth(0.1f);
    pPolyLine_->SetAlpha(0.5f);
    pPolyLine_->SetMoveAlphaFlag();

    killWaitTime_ = 20;
}

void Bullet_Sniper::Update()
{
    if (nextKill_) {
        killWaitTime_--;

        //Kill処理
        if (killWaitTime_ <= 0) {
            KillMe();
        }
        return;
    }

    if (pHitChara_) {
        transform_.position_ = pCapsuleCollider_->targetPos_;
        HitEffect();

        //ダメージ与える（HP０以下なら倒すのここでやっとく
        DamageInfo info = DamageInfo(parameter_.damage_);
        pHitChara_->GetDamageSystem()->ApplyDamageDirectly(info);
        if (pHitChara_->GetDamageSystem()->IsDead()) pHitChara_->KillMe();

        //DamageUI
        DamageUI::AddDamage(transform_.position_, parameter_.damage_, playerId_);

        //HitCursor
        GameManager::GetPlayer(playerId_)->GetGunBase()->GetAimCursor()->Hit();

        pPolyLine_->ClearFirstPosition();
        pPolyLine_->AddPosition(hitPos_);

        KillBullet();
        ClearCollider();
        return;
    }

    if (parameter_.killTimer_ <= 0) {
        //コリジョンマップに当たっていた場合
        if (isHit_) {
            transform_.position_ = hitPos_;
            HitEffect();

            pPolyLine_->ClearFirstPosition();
            pPolyLine_->AddPosition(hitPos_);
        }

        KillBullet();
        ClearCollider();
        return;
    }
    parameter_.killTimer_--;

    //移動
    transform_.position_ = Float3Add(transform_.position_, move_);
    pPolyLine_->AddPosition(transform_.position_);
}

void Bullet_Sniper::OnCollision(GameObject* pTarget)
{
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos ||
        pTarget->GetObjectName().find("Player") != std::string::npos )
    {
        float dist = CalculationDistance(startPos_, pCapsuleCollider_->targetPos_);
        float wallDist = CalculationDistance(startPos_, hitPos_);
        if (pHitChara_) {
            if (dist < minHitDist_) {
                pHitChara_ = static_cast<Character*>(pTarget);
                minHitDist_ = dist;
            }
        }
        else if(dist < wallDist) {
            pHitChara_ = static_cast<Character*>(pTarget);
            minHitDist_ = dist;
        }
    }

}

void Bullet_Sniper::Draw()
{
    pPolyLine_->Draw();
}

void Bullet_Sniper::Release()
{
    SAFE_RELEASE(pPolyLine_);
    SAFE_DELETE(pPolyLine_);
}

void Bullet_Sniper::Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos)
{
    hitPos_ = wallHitPos;
    startPos_ = transform_.position_;

    //コライダー情報セット（進行方向に向き、進行方向逆の0.5サイズでCenterを取る
    XMFLOAT3 colCenter = Float3Multiply(move_, -0.5f);
    pCapsuleCollider_ = new CapsuleCollider(colCenter, parameter_.collisionScale_, parameter_.speed_, -XMLoadFloat3(&move_));
    pCapsuleCollider_->typeList_.push_back(OBJECT_TYPE::Enemy);
    pCapsuleCollider_->typeList_.push_back(OBJECT_TYPE::Player);
    AddCollider(pCapsuleCollider_);
    
    //PolyLine追加
    XMFLOAT3 targetPos = wallHitPos;
    if (chara) targetPos = charaHitPos;

    float hitDist = CalculationDistance(transform_.position_, targetPos);
    if (hitDist < parameter_.speed_) {
        XMFLOAT3 move = Float3Multiply(move_, (hitDist / parameter_.speed_));
        for (int i = 0; i < POLY_LENG - 1; i++) {
            XMFLOAT3 addPos = Float3Multiply(move, (float)i / (float)POLY_LENG);
            pPolyLine_->AddPosition(Float3Add(transform_.position_, addPos));
        }
    }
    else {
        for (int i = 0; i < POLY_LENG - 1; i++) {
            XMFLOAT3 addPos = Float3Multiply(move_, (float)i / (float)POLY_LENG);
            pPolyLine_->AddPosition(Float3Add(transform_.position_, addPos));
        }
    }

    //コリジョンマップに当たっていた時
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

BulletBase::BulletParameter Bullet_Sniper::GetBulletParameter()
{
    return parameter_;
}

void Bullet_Sniper::HitEffect()
{
    EFFEKSEERLIB::EFKTransform t;
    transform_.scale_ = XMFLOAT3(0.2f, 0.2f, 0.2f);
    DirectX::XMStoreFloat4x4(&(t.matrix), transform_.GetWorldMatrix());
    t.isLoop = false;   //繰り返し
    t.maxFrame = 80;    //80フレーム
    t.speed = 1.0;      //スピード
    EFFEKSEERLIB::gEfk->Play("HIT", t);
}

void Bullet_Sniper::KillBullet()
{
    nextKill_ = true;

}
