#include "EnemyBase.h"
#include "EnemyManager.h"
#include "../Engine/Global.h"

EnemyBase::EnemyBase(GameObject* parent, std::string name)
    : Character(parent, name), enemyType_(ENEMY_TYPE::ENEMY_MAX), hModel_(-1)
{
    objectType_ = OBJECT_TYPE::Enemy;
}

EnemyBase::~EnemyBase()
{
    std::vector<EnemyBase*>& list = EnemyManager::GetAllEnemy();
    list.erase(std::remove_if(list.begin(), list.end(), [this](EnemyBase* EnemyBase) {
        return EnemyBase == this;
    }), list.end());

}

void EnemyBase::Initialize()
{
}

void EnemyBase::Update()
{
}

void EnemyBase::Draw()
{
}

void EnemyBase::Release()
{
}