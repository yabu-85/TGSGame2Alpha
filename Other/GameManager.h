#pragma once

class GameObject;
class CollisionMap;
class Player;
class SceneBase;

namespace GameManager
{
	void Initialize();
	void Update();
	void Draw();
	void SceneChange();

	void SetOnePlayer();
	void SetTwoPlayer();
	bool IsOnePlayer();

	//�A�N�Z�T
	GameObject* GetRootObject();
	void SetRootObject(GameObject* root);

	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	
	Player* GetPlayer(int index);
	void SetPlayer(Player* player, int index);

	SceneBase* GetScene();
	void SetScene(SceneBase* stage);

	//GameManager���ł̂ݎg���֐�
	void CommonDraw();				//���ʂ̕`��
	void IndividualDraw(int index);	//�ʂ̕`��
	void ImGuiDraw();				//Imgui�̕`��
	void ShadoDraw();				//�e�̕`��
	void OnePlayerDraw();			//��l�p�`��
	void TwoPlayerDraw();			//��l�p�`��	

};

