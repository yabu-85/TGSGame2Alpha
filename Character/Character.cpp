#include "Character.h"
#include "CharacterManager.h"
#include "../Engine/Global.h"
#include <vector>

Character::Character(GameObject* parent, std::string name) : GameObject(parent, name), bodyRange_(0.0f), bodyWeight_(0.0f), movement_{0,0,0}
{
    CharacterManager::AddCharacter(this);
}

Character::~Character()
{
    CharacterManager::RemoveCharacter(this);
}

void Character::Update()
{
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
