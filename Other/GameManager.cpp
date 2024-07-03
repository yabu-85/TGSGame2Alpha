#include "GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../UI/DamageUI.h"

namespace GameManager {
	CollisionMap* pCollisionMap_ = nullptr;
	Player* pPlayer_[2] = { nullptr, nullptr };
	StageBase* pStage_ = nullptr;

	void Initialize()
	{
	}

	void Update()
	{
	}

	void SceneChange()
	{
		pCollisionMap_ = nullptr;
		for(int i = 0;i < 2;i++) pPlayer_[i] = nullptr;
		pStage_ = nullptr;

		DamageUI::SceneChange();
		
	}
	
	CollisionMap* GameManager::GetCollisionMap() { return pCollisionMap_; }
	void GameManager::SetCollisionMap(CollisionMap* map) { pCollisionMap_ = map; }
	Player* GameManager::GetPlayer(int index) { return pPlayer_[index]; }
	void GameManager::SetPlayer(Player* player, int index) { pPlayer_[index] = player; }
	StageBase* GetStage() { return pStage_; }
	void SetStage(StageBase* stage) { pStage_ = stage; }

}