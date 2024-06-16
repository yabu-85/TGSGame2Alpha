#include "EnemyManager.h"
#include "EnemyBase.h"
#include "../State/StateManager.h"

namespace EnemyManager {
	std::vector<EnemyBase*> enemyList_;
	GameObject* pParent_;

	//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

	void AllKillEnemy()
	{
		for (auto it = enemyList_.begin(); it != enemyList_.end();) {
			(*it)->KillMe();
			it = enemyList_.erase(it);
		}
		enemyList_.clear();
	}

	void KillEnemy(EnemyBase* enemy)
	{
		for (auto it = enemyList_.begin(); it != enemyList_.end();) {
			if (*it == enemy) {
				it = enemyList_.erase(it);
				enemy->KillMe();
				break;
			}
			else {
				++it;
			}
		}
	}

	void SpawnEnemy(ENEMY_TYPE type)
	{
		EnemyBase* e = nullptr;
		if (type == ENEMY_TEST) enemyList_.push_back(e = Instantiate<EnemyBase>(pParent_));
		if (type == ENEMY_TEST2) enemyList_.push_back(e = Instantiate<EnemyBase>(pParent_));
		e->SetEnemyType(type);
	}

	void SetParent(GameObject* parent) { pParent_ = parent; }
	std::vector<EnemyBase*>& GetAllEnemy() { return enemyList_; }

}
