#include "EnemyManager.h"
#include "Enemy.h"
#include "../State/StateManager.h"

EnemyManager::EnemyManager() : pParent_(nullptr)
{
}

void EnemyManager::AllKillEnemy()
{
	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		EnemyBase* erasedEnemy = static_cast<EnemyBase*>(*it);

		(*it)->KillMe();
		it = enemyList_.erase(it);
	}
	enemyList_.clear();
}

void EnemyManager::KillEnemy(EnemyBase* enemy)
{
	for (auto it = enemyList_.begin(); it != enemyList_.end();) {
		if (*it == enemy) {
			EnemyBase* erasedEnemy = static_cast<EnemyBase*>(*it);

			it = enemyList_.erase(it);
			break;
		}
		else {
			++it;
		}
	}
	enemy->KillMe();
}

void EnemyManager::SpawnEnemy(ENEMY_TYPE type)
{
	if (type == ENEMY_STONEGOLEM) AddEnemyList(InstantiateFront<StoneGolem>(pParent_), type);
	else if (type == ENEMY_THROW) AddEnemyList(InstantiateFront<ThrowEnemy>(pParent_), type);
	else if (type == ENEMY_MELEE) AddEnemyList(InstantiateFront<MeleeFighter>(pParent_), type);
	else if (type == ENEMY_SWORDBOSS) AddEnemyList(InstantiateFront<SwordBoss>(pParent_), type);
	else if (type == ENEMY_SCARECROW) AddEnemyList(InstantiateFront<Scarecrow>(pParent_), type);
}

void EnemyManager::SpawnEnemyTable(ENEMY_TABLE type)
{
	//spawnEnemyTable‚Ì’†‚©‚çspawnTable‚ÌêŠ‚©‚ç‚Æ‚Á‚Ä‚»‚Ìvector‚Ìsize‚ğæ“¾
	int randMax = (int)spawnEnemyTable[type].size();
	if (randMax <= 0) return;

	//‚±‚±“ïˆÕ“x‚É‚æ‚Á‚Ä‘I‚ÔŠm—¦§Œä‚Æ‚©‚µ‚½‚¢
	int r = rand() % randMax;

	int max = (int)spawnEnemyTable[type][r].size();
	for (int i = 0; i < max; i++) {
		SpawnEnemy(spawnEnemyTable[type][r].at(i));
	}
}

std::vector<EnemyBase*>& EnemyManager::GetAllEnemy()
{
	return enemyList_;
}

void EnemyManager::PlayAtPosition(XMFLOAT3 position, float range)
{
	for (auto it : enemyList_) {
		if (it->GetStateManager() && it->GetStateManager()->GetName() == "Patrol") {
			XMFLOAT3 fEnemy = it->GetPosition();
			XMVECTOR ePos = XMLoadFloat3(&fEnemy);
			XMVECTOR sPos = XMLoadFloat3(&position);
			if (XMVectorGetX(XMVector3Length(sPos - ePos)) <= range) {
				it->GetStateManager()->ChangeState("Combat");
			}
		}
	}
}

void EnemyManager::ResetAllEnemyCollider()
{
	for (auto it : enemyList_) {
		std::list<Collider*> cList = it->GetColliderList();
		for (auto c : cList) c->SetValid(true);
	}
}

void EnemyManager::SceneTransitionInitialize()
{
	enemyList_.clear();
	pParent_ = nullptr;

}

void EnemyManager::AddEnemyList(EnemyBase* e, int type)
{
	e->SetEnemyType(ENEMY_TYPE(type));
	enemyList_.push_back(e);

}