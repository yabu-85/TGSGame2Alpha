#pragma once

class GameObject;
class CollisionMap;
class Player;
class SceneBase;

namespace GameManager
{
	extern float	playerSpeed;
	extern bool		playerClimb;
	extern bool		playerFaly;

	void Initialize();
	void Update();
	void Draw();
	void SceneChange();
	int GetDrawIndex();

	//���j���[�\�������i�Ⴄ�Ȃ�v���C�V�[����Pause��ʂł͂Ȃ�
	bool IsCursorMode();
	void SetCursorMode(bool b);

	//��ʕ���
	void SetOnePlayer();
	void SetTwoPlayer();
	bool IsOnePlayer();

	//�R���g���[���[
	void SetPCCtrlON();
	void SetPCCtrlOFF();
	bool IsPCCtrl();
	void SetPCCtrlNumber(int number);
	int GetPCCtrlNumber();

	//�e�̐ݒ�
	void SetShadowDraw(bool b);
	bool IsShadowDraw();

	//�A�N�Z�T
	GameObject* GetRootObject();
	void SetRootObject(GameObject* root);

	CollisionMap* GetCollisionMap();
	void SetCollisionMap(CollisionMap* map);
	
	Player* GetPlayer(int index);
	void SetPlayer(Player* player, int index);

	SceneBase* GetScene();
	void SetScene(SceneBase* stage);

	//GameManager���ł̂ݎg���֐��[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	void CommonDraw();				//���ʂ̕`��
	void IndividualDraw(int index);	//�ʂ̕`��
	void ImGuiDraw();				//Imgui�̕`��
	void ShadowDraw();				//�e�̕`��
	void OnePlayerDraw();			//��l�p�`��
	void TwoPlayerDraw();			//��l�p�`��	

};

