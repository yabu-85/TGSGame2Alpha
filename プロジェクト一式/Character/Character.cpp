#include "Character.h"
#include "CharacterManager.h"
#include "../UI/HealthGauge.h"
#include "../Character/DamageSystem.h"
#include "../Engine/Global.h"
#include <vector>
#include "../Engine/Fbx.h"
#include "../Other/GameManager.h"

Character::Character(GameObject* parent, std::string name)
    : GameObject(parent, name), bodyRange_(0.0f), bodyWeight_(0.0f), movement_{0,0,0}, bodyHeightHalf_(0.0f)
{
    pHealthGauge_ = nullptr;
    CharacterManager::AddCharacter(this);
}

Character::~Character()
{
    SAFE_DELETE(pHealthGauge_);
    CharacterManager::RemoveCharacter(this);
}

void Character::Update()
{
}

void Character::BounceFloar()
{
    RayCastData rayData = RayCastData();
    rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerHeight, transform_.position_.z);
    rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
    XMFLOAT3 pos = XMFLOAT3(transform_.position_.x, transform_.position_.y + calcHeight, transform_.position_.z);
    GameManager::GetCollisionMap()->CellFloarRayCast(pos, &rayData);
    if (rayData.dist <= PlayerHeight + perDist) {
        transform_.position_.y += PlayerHeight - rayData.dist;
        gravity_ = 0.0f;
        isFly_ = false;
    }
}

void Character::BounceWall()
{
    //下
    XMVECTOR push = XMVectorZero();
    bool hit = GameManager::GetCollisionMap()->CellSphereVsTriangle(pSphereCollider_[0], push);

    //上
    push = XMVectorZero();
    if (GameManager::GetCollisionMap()->CellSphereVsTriangle(pSphereCollider_[1], push)) hit = true;

    return hit;
}

void Character::BounceRoof()
{
    RayCastData rayData = RayCastData();
    rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerWaist, transform_.position_.z);
    rayData.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
    GameManager::GetCollisionMap()->CellFloarRayCast(transform_.position_, &rayData);
    float calcSize = PlayerHeight - PlayerWaist;
    if (rayData.hit && rayData.dist < calcSize) {
        transform_.position_.y -= calcSize - rayData.dist;

        //誤差
        transform_.position_.y -= 0.05f;

        gravity_ = 0.0f;
        return true;
    }
    return false;
}

void Character::ReflectCharacter()
{
    float sY = transform_.position_.y;
    std::vector<Character*> list = CharacterManager::GetCharacterList();
    for (Character* c : list) {
        //自分は飛ばす
        if (c == this) continue;
        float oY = c->transform_.position_.y;

        XMFLOAT3 targetPos = c->GetPosition();
        XMFLOAT3 direction = Float3Sub(targetPos, transform_.position_);

        //当たってないなら次
        float addRange = bodyRange_ + bodyRange_;
        float range = CalculationDistance(direction);
        if (range > addRange) continue;

        //押し出しの強さ（0になるの防ぐためにちょい足す）
        float w = bodyWeight_ + bodyWeight_;
        float sWeight = bodyWeight_ / (w + 0.001f);
        float oWeight = bodyWeight_ / (w + 0.001f);

        //押し出しベクトル計算
        XMFLOAT3 extrusion = Float3Multiply(Float3Normalize(direction), addRange - range);

        //押し出す
        XMFLOAT3 outPos = Float3Multiply(extrusion, -sWeight);
        outPos = Float3Add(outPos, transform_.position_);
        transform_.position_ = outPos;

        outPos = Float3Multiply(extrusion, oWeight);
        outPos = Float3Add(outPos, targetPos);
        c->transform_.position_ = outPos;

        //y座標は戻す
        c->transform_.position_.y = oY;
    }
    transform_.position_.y = sY;
}
