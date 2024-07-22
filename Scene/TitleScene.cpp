#include "TitleScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Screen/SelectScreen.h"
#include "../Other/GameManager.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SceneManager.h"

//コンストラクタ
TitleScene::TitleScene(GameObject * parent)
	: SceneBase(parent, "TitleScene")
{
}

//初期化
void TitleScene::Initialize()
{
	GameManager::SetOnePlayer();

	AllDeleteScreen();
	AddScreen(new SelectScreen());

}

//更新
void TitleScene::Update()
{
	SceneBase::Update();

	if (Input::IsKeyDown(DIK_M)) {
		static_cast<SceneManager*>(FindObject("SceneManager"))->ChangeScene(SCENE_ID_TEST);
		return;
	}

}

//描画
void TitleScene::Draw()
{

}

//開放
void TitleScene::Release()
{

}
