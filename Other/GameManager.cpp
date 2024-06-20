#include "GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"

namespace GameManager {
	CollisionMap* pCollisionMap_ = nullptr;
	Player* pPlayer_ = nullptr;
	StageBase* pStage_ = nullptr;

	void Initialize()
	{
	}

	void Update()
	{
	}
	
	CollisionMap* GameManager::GetCollisionMap() { return pCollisionMap_; }
	void GameManager::SetCollisionMap(CollisionMap* map) { pCollisionMap_ = map; }
	Player* GameManager::GetPlayer() { return pPlayer_; }
	void GameManager::SetPlayer(Player* player) { pPlayer_ = player; }

}