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

	int GetDrawIndex();

	void SetOnePlayer();
	void SetTwoPlayer();
	bool IsOnePlayer();

	void SetPCCtrlON();
	void SetPCCtrlOFF();
	bool IsPCCtrl();
	void SetPCCtrlNumber(int number);
	int GetPCCtrlNumber();

	//アクセサ
	GameObject* GetRootObject();
	void SetRootObject(GameObject* root);

	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	
	Player* GetPlayer(int index);
	void SetPlayer(Player* player, int index);

	SceneBase* GetScene();
	void SetScene(SceneBase* stage);

	//GameManager内でのみ使う関数ーーーーーーーーーーーーーーー
	void CommonDraw();				//共通の描画
	void IndividualDraw(int index);	//個別の描画
	void ImGuiDraw();				//Imguiの描画
	void ShadoDraw();				//影の描画
	void OnePlayerDraw();			//一人用描画
	void TwoPlayerDraw();			//二人用描画	

};

