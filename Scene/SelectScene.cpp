#include "SelectScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Screen/SelectScreen.h"

#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"

//コンストラクタ
SelectScene::SelectScene(GameObject * parent)
	: SceneBase(parent, "SelectScene")
{
}

//初期化
void SelectScene::Initialize()
{
	AllDeleteScreen();
	AddScreen(new SelectScreen());

}

//更新
void SelectScene::Update()
{
}

//描画
void SelectScene::Draw()
{
}

//開放
void SelectScene::Release()
{

}
