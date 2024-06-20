#pragma once

class CollisionMap;
class Player;
class StageBase;

namespace GameManager
{
	void Initialize();
	void Update();

	//アクセサ
	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	Player* GetPlayer();
	void SetPlayer(Player* player);
	StageBase* GetStage();
	void SetStage(StageBase* stage);

};

