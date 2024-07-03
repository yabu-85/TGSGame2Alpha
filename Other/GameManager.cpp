#include "GameManager.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SceneManager.h"

#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../Other/InputManager.h"
#include "../UI/DamageUI.h"
#include "../Scene/SceneBase.h"

namespace GameManager {
	CollisionMap* pCollisionMap_ = nullptr;
	Player* pPlayer_[2] = { nullptr, nullptr };
	SceneBase* pScene_ = nullptr;

	void Initialize()
	{
		InputManager::Initialize();

	}

	void Update()
	{
		DamageUI::Update();

	}

	void CommonDraw()
	{
		if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
			pScene_->CommonUIDraw();
		}
	}

	void IndividualDraw()
	{
		if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
			pScene_->IndividualUIDraw();
		}
	}

	void SceneChange()
	{
		pCollisionMap_ = nullptr;
		for(int i = 0;i < 2;i++) pPlayer_[i] = nullptr;
		pScene_ = nullptr;

		DamageUI::SceneChange();
	}
	
	CollisionMap* GameManager::GetCollisionMap() { return pCollisionMap_; }
	void GameManager::SetCollisionMap(CollisionMap* map) { pCollisionMap_ = map; }
	Player* GameManager::GetPlayer(int index) { return pPlayer_[index]; }
	void GameManager::SetPlayer(Player* player, int index) { pPlayer_[index] = player; }
	SceneBase* GetScene() { return pScene_; }
	void SetScene(SceneBase* stage) { pScene_ = stage; }

}