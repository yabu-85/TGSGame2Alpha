#include "SelectScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Screen/SelectScreen.h"
#include "../Other/GameManager.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SceneManager.h"

//�R���X�g���N�^
SelectScene::SelectScene(GameObject * parent)
	: SceneBase(parent, "SelectScene")
{
}

//������
void SelectScene::Initialize()
{
	GameManager::SetOnePlayer();

	AllDeleteScreen();
	AddScreen(new SelectScreen());

}

//�X�V
void SelectScene::Update()
{
	SceneBase::Update();

	if (Input::IsKeyDown(DIK_M)) {
		static_cast<SceneManager*>(FindObject("SceneManager"))->ChangeScene(SCENE_ID_TEST);
		return;
	}

}

//�`��
void SelectScene::Draw()
{

}

//�J��
void SelectScene::Release()
{

}
