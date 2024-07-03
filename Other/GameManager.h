#pragma once

class CollisionMap;
class Player;
class SceneBase;

namespace GameManager
{
	void Initialize();
	void Update();
	void CommonDraw();		//共通の描画
	void IndividualDraw();	//個別の描画
	void SceneChange();

	//アクセサ
	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	
	Player* GetPlayer(int index);
	void SetPlayer(Player* player, int index);

	SceneBase* GetScene();
	void SetScene(SceneBase* stage);

};

