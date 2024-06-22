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
	//�����I�u�W�F�N�g�e�̐ݒ�
	void SetParent(GameObject* parent);

	//�G�l�~�[���X�g�擾
	std::vector<EnemyBase*>& GetAllEnemy();

	//�G��|��
	void AllKillEnemy();
	void KillEnemy(EnemyBase* enemy);
	
	//�w�肵���G�X�|�[��
	void SpawnEnemy(ENEMY_TYPE type);

};

