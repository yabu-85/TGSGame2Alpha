#include "EnemyBase.h"
#include "../Stage/Stage.h"

#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Direct3D.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../Scene/TestScene.h"
#include "../UI/HealthGauge.h"
#include "../Character/DamageSystem.h"

namespace {
    const float gravity = 0.002f;
    const float HeightSize = 1.3f;    //��ԏ�
    XMFLOAT3 start = XMFLOAT3(50.0f, 10.0f, 50.0f);

    float bodyRange_ = 0.5f;
    float bodyWeight_ = 1.0f;
    Player* pPlayer = nullptr;
    CollisionMap* pCMap = nullptr;
}

EnemyBase::EnemyBase(GameObject* parent)
    : GameObject(parent, "EnemyBase"), hModel_(-1), gravity_(0.0f), pHealthGauge_(nullptr)
{
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    pScene->GetEnemyList().push_back(this);

    pPlayer = static_cast<Player*>(FindObject("Player"));
    pCMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));

}

EnemyBase::~EnemyBase()
{
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    std::vector<EnemyBase*>& list = pScene->GetEnemyList();
    list.erase(std::remove_if(list.begin(), list.end(), [this](EnemyBase* EnemyBase) {
        return EnemyBase == this;
    }), list.end());

}

void EnemyBase::Initialize()
{
    hModel_ = Model::Load("Model/Scarecrow.fbx");
    assert(hModel_ >= 0);
    Model::SetAnimFrame(hModel_, 0, 100, 1.0f);
   
    type_ = ObjectType::Enemy;
    transform_.position_ = start;
    MoveSpeed = 0.075f;

    pHealthGauge_ = new HealthGauge(this);
    pHealthGauge_->SetHeight(1.7f);

    pDamageSystem_ = new DamageSystem();
    pDamageSystem_->SetMaxHP(20);
    pDamageSystem_->SetHP(20);

#if 0
    XMVECTOR vec = { 1.0f, 0.0f, 0.0f, 0.0f };
    CapsuleCollider* collid = new CapsuleCollider(XMFLOAT3(), 0.5f, 1.5, vec);
    AddCollider(collid);
#else
    SphereCollider* collid = new SphereCollider(XMFLOAT3(0.0f, 0.7f, 0.0f), 0.5f);
    collid->typeList_.push_back(ObjectType::Stage);
    AddCollider(collid);
#endif

}

void EnemyBase::Update()
{
    if (transform_.position_.y <= -30.0f) {
        KillMe();
        return;
    }

    if (!isGround) {
        gravity_ += gravity;
        transform_.position_.y -= gravity_;
    }

    //�ړ�
    Move();
    ReflectCharacter();

    //�ǂƂ̓����蔻��
    SphereCollider* collid = static_cast<SphereCollider*>(colliderList_.front());
    XMVECTOR push = XMVectorZero();
    pCMap->CellSphereVsTriangle(collid, push);

    float addPos = -1.0f;
    isGround = false;
    for (int i = 0; i < 2; i++) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + HeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        XMFLOAT3 pos = transform_.position_;
        pos.y += addPos;
        pCMap->CellFloarRayCast(pos, &rayData);
        if (rayData.hit && rayData.dist < HeightSize) {
            transform_.position_.y += HeightSize - rayData.dist;
            gravity_ = 0.0f;
            isGround = true;
        }
        addPos = 0.0f;
    }
    
}

void EnemyBase::Draw()
{
    Direct3D::EnemyPosition = transform_.position_;

    if (damageTime > 0.0f) damageTime -= 0.1f;
    Direct3D::emphasisTime_ = damageTime;
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
    Direct3D::emphasisTime_ = 0.0f;

    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
        float r = (float)pDamageSystem_->GetHP() / (float)pDamageSystem_->GetMaxHP();
        pHealthGauge_->SetParcent(r);
        pHealthGauge_->Draw();
    }

    CollisionDraw();

}

void EnemyBase::Release()
{
}

#include "../AI/RouteSearch.h"
void EnemyBase::Move()
{
    static const float RayMoveDist = 10.0f;
    static const float RayHeight = 0.5f;

    XMVECTOR vPos2 = XMLoadFloat3(&transform_.position_);
    XMFLOAT3 playerPos = pPlayer->GetPosition();
    XMVECTOR vPlaPos2 = XMLoadFloat3(&playerPos);
    XMVECTOR vec2 = vPos2 - vPlaPos2;

    float plaDist = XMVectorGetX(XMVector3Length(vec2));
    if (plaDist <= 1.0f) {
        targetList_.clear();
        return;
    }
    
    if (plaDist <= RayMoveDist) {

        if (rand() % 10 == 0) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + RayHeight, transform_.position_.z);

            XMVECTOR vec = XMVector3Normalize(vec2) * -1.0f;
            XMStoreFloat3(&rayData.dir, vec);

            XMFLOAT3 target = XMFLOAT3(pPlayer->GetPosition().x, pPlayer->GetPosition().y + RayHeight, pPlayer->GetPosition().z);
            pCMap->RaySelectCellVsSegment(target, &rayData);
            moveReady = rayData.dist >= plaDist;
        }
        
        if (moveReady) {
            targetList_.clear();
            
            //�ړ��X�s�[�h�}��
            float moveDist = XMVectorGetX(XMVector3Length(vec2));
            if (moveDist > MoveSpeed) vec2 = XMVector3Normalize(vec2) * MoveSpeed;

            //����v�Z
            CalcDodge(vec2);

            //�ړ�
            vPos2 -= vec2;
            XMStoreFloat3(&transform_.position_, vPos2);
            return;
        }
    }

    //���̎��ԂŁA���X�g�o�H�X�V����
    if ( (targetList_.empty() || rand() % 30 == 0) && rand() % 10 == 0) {
        targetList_ = RouteSearch::AStar(RouteSearch::GetNodeList(), pPlayer->GetPosition(), transform_.position_);
    }

    //�ړ��I������
    if (targetList_.empty()) {
        return;
    }

    //�W�����v�ړ�
    if (targetList_.back().type == EdgeType::JUMP) {
        gravity_ = 0.0f;
        //�ړ������v�Z
        XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
        XMVECTOR vTar = XMLoadFloat3(&targetList_.back().pos);
        XMVECTOR vMove = vTar - vPos;

        //�ړ��X�s�[�h�}��
        float moveDist = XMVectorGetX(XMVector3Length(vMove));
        if (moveDist > MoveSpeed) vMove = XMVector3Normalize(vMove) * MoveSpeed;

        //�^�[�Q�b�g�ʒu����
        if (moveDist <= moveRange_) {
            targetList_.pop_back();
            Move();
            return;
        }

        vPos += vMove;
        XMStoreFloat3(&transform_.position_, vPos);
        return;
    }

    //�ړ������v�Z
    XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
    XMVECTOR vTar = XMLoadFloat3(&targetList_.back().pos);
    XMVECTOR vMove = vTar - vPos;

    //�ړ��X�s�[�h�}��
    float moveDist = XMVectorGetX(XMVector3Length(vMove));
    if (moveDist > MoveSpeed) vMove = XMVector3Normalize(vMove) * MoveSpeed;

    //�^�[�Q�b�g�ʒu����
    if (moveDist <= moveRange_) {
        targetList_.pop_back();
        Move();
        return;
    }

    //����v�Z
    CalcDodge(vMove);
    
    vPos += vMove;
    XMStoreFloat3(&transform_.position_, vPos);
}

void EnemyBase::CalcDodge(XMVECTOR& move)
{
    const float safeSize = 1.0f;
    XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
    XMVECTOR vSafeMove = XMVectorZero();
    
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    std::vector<EnemyBase*> list = pScene->GetEnemyList();
    for (auto& e : list) {
        if (e == this) continue;
        XMFLOAT3 f = e->GetPosition();
        XMVECTOR vTarget = XMLoadFloat3(&f);
        XMVECTOR vec = vTarget - vPos;
        float range = XMVectorGetX(XMVector3Length(vec));

        if (range < safeSize) {
            range -= safeSize;
            vSafeMove += XMVector3Normalize(vec) * range;
        }
    }
    move += vSafeMove;
}

void EnemyBase::ReflectCharacter()
{
    float sY = transform_.position_.y;
    TestScene* pScene = static_cast<TestScene*>(FindObject("TestScene"));
    std::vector<EnemyBase*> list = pScene->GetEnemyList();
    for (EnemyBase* c : list) {
        //�����͔�΂�
        if (c == this) continue;
        float oY = c->transform_.position_.y;

        XMFLOAT3 targetPos = c->GetPosition();
        XMFLOAT3 direction = Float3Sub(targetPos, transform_.position_);

        //�������ĂȂ��Ȃ玟
        float addRange = bodyRange_ + bodyRange_;
        float range = CalculationDistance(direction);
        if (range > addRange) continue;

        //�����o���̋����i0�ɂȂ�̖h�����߂ɂ��傢�����j
        float w = bodyWeight_ + bodyWeight_;
        float sWeight = bodyWeight_ / w + 0.001f;
        float oWeight = bodyWeight_ / w + 0.001f;

        //�����o���x�N�g���v�Z
        XMFLOAT3 extrusion = Float3Multiply(Float3Normalize(direction), addRange - range);

        //�����o��
        XMFLOAT3 outPos = Float3Multiply(extrusion, -sWeight);
        outPos = Float3Add(outPos, transform_.position_);
        transform_.position_ = outPos;

        outPos = Float3Multiply(extrusion, oWeight);
        outPos = Float3Add(outPos, targetPos);
        c->transform_.position_ = outPos;

        //y���W�͖߂�
        c->transform_.position_.y = oY;
    }
    transform_.position_.y = sY;
}
