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

	//�G�|��
	void AllKillEnemy();
	void KillEnemy(EnemyBase* enemy);

	std::vector<EnemyBase*>& GetAllEnemy();
	void AddEnemyList(EnemyBase* e, int type);

	//�w�肵���G
	void SpawnEnemy(ENEMY_TYPE type);

	//���͈͓̔���PatrolState��Enemy�̏�Ԃ�ύX
	void PlayAtPosition(XMFLOAT3 position, float range);

	//�S�G�l�~�[��Collider�̔����true��
	void ResetAllEnemyCollider();

	void SceneTransitionInitialize();

	bool IsEnemyListEmpty() { return enemyList_.empty(); }

};

