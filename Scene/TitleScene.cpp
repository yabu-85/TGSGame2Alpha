#include "TitleScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Other/GameManager.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "../Engine/Light.h"
#include "../Screen/TitleScreen.h"

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
	AddScreen(new TitleScreen());

	Light::SetPosition(0, XMFLOAT4(0.0f, 5.0f, 0.0f, 1.0f));
	Camera::SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f), 0);
	Camera::SetTarget(XMFLOAT3(0.0f, 0.0f, -10.0f), 0);
	Camera::Update(0);

}

//更新
void TitleScene::Update()
{
	SceneBase::Update();

	if (Input::IsKeyDown(DIK_M)) {
		static_cast<SceneManager*>(FindObject("SceneManager"))->ChangeScene(SCENE_ID_PLAY);
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
