#pragma once

class CollisionMap;
class Player;
class StageBase;

namespace GameManager
{
	void Initialize();
	void Update();
	void SceneChange();

	//�A�N�Z�T
	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	
	Player* GetPlayer(int index);
	void SetPlayer(Player* player, int index);

	StageBase* GetStage();
	void SetStage(StageBase* stage);

};

