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

	//ƒAƒNƒZƒT
	GameObject* GetRootObject();
	void SetRootObject(GameObject* root);

	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	
	Player* GetPlayer(int index);
	void SetPlayer(Player* player, int index);

	SceneBase* GetScene();
	void SetScene(SceneBase* stage);

	//GameManager“à‚Å‚Ì‚İg‚¤ŠÖ”
	void CommonDraw();				//‹¤’Ê‚Ì•`‰æ
	void IndividualDraw(int index);	//ŒÂ•Ê‚Ì•`‰æ
	void ImGuiDraw();				//Imgui‚Ì•`‰æ
	void ShadoDraw();				//‰e‚Ì•`‰æ
	void OnePlayerDraw();			//ˆêl—p•`‰æ
	void TwoPlayerDraw();			//“ñl—p•`‰æ	

};

