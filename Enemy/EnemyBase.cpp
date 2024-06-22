#include "EnemyBase.h"
#include "EnemyManager.h"
#include "HealthGauge.h"
#include "../Character/DamageSystem.h"

EnemyBase::EnemyBase(GameObject* parent, std::string name)
    : GameObject(parent, name), enemyType_(ENEMY_TYPE::ENEMY_MAX), hModel_(-1), gravity_(0.0f)
{
    pHealthGauge_ = new HealthGauge(this);
    pDamageSystem_ = new DamageSystem();

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