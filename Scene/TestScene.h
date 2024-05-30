#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class Enemy;

class TestScene : public GameObject
{
	std::vector<Enemy*> enemyList_;

public:
	TestScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	std::vector<Enemy*>& GetEnemyList() { return enemyList_; }
};