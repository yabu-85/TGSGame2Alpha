#include "SelectScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Screen/ScreenManager.h"
#include "../Screen/SelectScreen.h"

#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"

//�R���X�g���N�^
SelectScene::SelectScene(GameObject * parent)
	: GameObject(parent, "SelectScene")
{
}

//������
void SelectScene::Initialize()
{
	ScreenManager::AllDeleteScreen();
	ScreenManager::AddScreen(new SelectScreen());

}

//�X�V
void SelectScene::Update()
{
}

//�`��
void SelectScene::Draw()
{
}

//�J��
void SelectScene::Release()
{

}
