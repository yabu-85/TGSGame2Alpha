#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

class EnemyBase;
class GameObject;

enum ENEMY_TYPE {
	ENEMY_STONEGOLEM = 0,
	ENEMY_THROW,
	ENEMY_MELEE,
	ENEMY_SWORDBOSS,
	ENEMY_SCARECROW,
	ENEMY_MAX,
};

class EnemyManager
{
	std::vector<EnemyBase*> enemyList_;
	GameObject* pParent_;


public:
	EnemyManager();
	void SetParent(GameObject* parent) { pParent_ = parent; }

	//敵倒す
	void AllKillEnemy();
	void KillEnemy(EnemyBase* enemy);

	std::vector<EnemyBase*>& GetAllEnemy();
	void AddEnemyList(EnemyBase* e, int type);

	//指定した敵
	void SpawnEnemy(ENEMY_TYPE type);

	//音の範囲内のPatrolStateのEnemyの状態を変更
	void PlayAtPosition(XMFLOAT3 position, float range);

	//全エネミーのColliderの判定をtrueに
	void ResetAllEnemyCollider();

	void SceneTransitionInitialize();

	bool IsEnemyListEmpty() { return enemyList_.empty(); }

};

