#pragma once

class CollisionMap;
class Player;
class SceneBase;

namespace GameManager
{
	void Initialize();
	void Update();
	void CommonDraw();		//���ʂ̕`��
	void IndividualDraw();	//�ʂ̕`��
	void SceneChange();

	//�A�N�Z�T
	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	
	Player* GetPlayer(int index);
	void SetPlayer(Player* player, int index);

	SceneBase* GetScene();
	void SetScene(SceneBase* stage);

};

