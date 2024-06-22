#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

class EnemyBase;
class GameObject;

enum ENEMY_TYPE {
	ENEMY_TEST = 0,
	ENEMY_TEST2,
	ENEMY_MAX,
};

namespace EnemyManager
{
	//生成オブジェクト親の設定
	void SetParent(GameObject* parent);

	//エネミーリスト取得
	std::vector<EnemyBase*>& GetAllEnemy();

	//敵を倒す
	void AllKillEnemy();
	void KillEnemy(EnemyBase* enemy);
	
	//指定した敵スポーン
	void SpawnEnemy(ENEMY_TYPE type);

};

