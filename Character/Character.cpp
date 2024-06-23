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
        float sWeight = bodyWeight_ / (w + 0.001f);
        float oWeight = bodyWeight_ / (w + 0.001f);

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
