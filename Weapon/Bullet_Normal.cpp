#include "Bullet_Normal.h"
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

namespace {
    static const int POLY_LENG = 30;

}

Bullet_Normal::Bullet_Normal(GameObject* parent)
    : BulletBase(parent, "Bullet_Normal"), pPolyLine_(nullptr), isHit_(false), hitPos_(XMFLOAT3()), startPos_(XMFLOAT3()),
    pHitChara_(nullptr), minHitDist_(99999.9f), nextKill_(false), pCapsuleCollider_(nullptr), killWaitTime_(0)
{
    //JSON�t�@�C���ǂݍ���
    JsonReader::Load("Json/Bullet.json");
    auto& bullet_normal = JsonReader::GetSection("Bullet_Normal");

    //�p�����[�^���擾
    parameter_.damage_ = bullet_normal["damage"];
    parameter_.shotCoolTime_ = bullet_normal["shotCoolTime"];
    parameter_.speed_ = bullet_normal["speed"];
    parameter_.killTimer_ = bullet_normal["killTimer"];
    parameter_.collisionScale_ = bullet_normal["collisionScale"];

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
    pPolyLine_->SetAlpha(0.5f);
    pPolyLine_->SetMoveAlphaFlag();

    killWaitTime_ = 20;
}

void Bullet_Normal::Update()
{
    if (nextKill_) {
        killWaitTime_--;

        //Kill����
        if (killWaitTime_ <= 0) {
            KillMe();
        }
        return;
    }

    if (pHitChara_) {
        transform_.position_ = pCapsuleCollider_->targetPos_;
        HitEffect();
        
        //�_���[�W�^����iHP�O�ȉ��Ȃ�|���̂����ł���Ƃ�
        DamageInfo info = DamageInfo(parameter_.damage_);
        pHitChara_->ApplyDamageDirectly(info);

        //DamageUI
        DamageUI::AddDamage(transform_.position_, parameter_.damage_, playerId_);

        //HitCursor
        GameManager::GetPlayer(playerId_)->GetGun()->GetAimCursor()->Hit();

        pPolyLine_->ClearFirstPosition();
        pPolyLine_->AddPosition(hitPos_);

        KillBullet();
        ClearCollider();
        return;
    }

    if (parameter_.killTimer_ <= 0) {
        //�R���W�����}�b�v�ɓ������Ă����ꍇ
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

    //�ړ�
    transform_.position_ = Float3Add(transform_.position_, move_);
    pPolyLine_->AddPosition(transform_.position_);
}

void Bullet_Normal::OnCollision(GameObject* pTarget)
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

void Bullet_Normal::Draw()
{
    pPolyLine_->Draw();
}

void Bullet_Normal::Release()
{
    SAFE_RELEASE(pPolyLine_);
    SAFE_DELETE(pPolyLine_);
}

void Bullet_Normal::Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos)
{
    hitPos_ = wallHitPos;
    startPos_ = transform_.position_;

    //�R���C�_�[���Z�b�g�i�i�s�����Ɍ����A�i�s�����t��0.5�T�C�Y��Center�����
    XMFLOAT3 colCenter = Float3Multiply(move_, -0.5f);
    pCapsuleCollider_ = new CapsuleCollider(colCenter, parameter_.collisionScale_, parameter_.speed_, -XMLoadFloat3(&move_));
    pCapsuleCollider_->typeList_.push_back(OBJECT_TYPE::Enemy);
    pCapsuleCollider_->typeList_.push_back(OBJECT_TYPE::Player);
    AddCollider(pCapsuleCollider_);
    
    //PolyLine�ǉ�
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

    //�R���W�����}�b�v�ɓ������Ă�����
    float calcDist = parameter_.speed_ * parameter_.killTimer_;
    if (hitDist <= calcDist) {
        isHit_ = true;

        //KillTime�v�Z
        int newKillTime = (int)(hitDist / CalculationDistance(move_)) + 1;
        if (newKillTime < parameter_.killTimer_) {
            parameter_.killTimer_ = newKillTime;
        }
    }

}

void Bullet_Normal::HitEffect()
{
    EFFEKSEERLIB::EFKTransform t;
    transform_.scale_ = XMFLOAT3(0.02f, 0.02f, 0.02f);
    DirectX::XMStoreFloat4x4(&(t.matrix), transform_.GetWorldMatrix());
    t.isLoop = false;   //�J��Ԃ�
    t.maxFrame = 80;    //80�t���[��
    t.speed = 1.0;      //�X�s�[�h
    EFFEKSEERLIB::gEfk->Play("HIT", t);
}

void Bullet_Normal::KillBullet()
{
    nextKill_ = true;

}
