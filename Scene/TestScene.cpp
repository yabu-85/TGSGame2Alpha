#include "TestScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Enemy/Enemy.h"

#include "../Engine/Input.h"
#include "../Engine/Light.h"
#include "../Engine/Direct3D.h"
#include "../AI/RouteSearch.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene")
{
}

//初期化
void TestScene::Initialize()
{
	Instantiate<Stage>(this);
	Instantiate<Player>(this);

	int count = 0;
	for(int i = 0;i < count;i++) Instantiate<Enemy>(this);

	RouteSearch::InitializeList();
}

//更新
void TestScene::Update()
{
	if (Input::IsKeyDown(DIK_M)) Instantiate<Enemy>(this);
	if (Input::IsKeyDown(DIK_N)) {
		Enemy* e = static_cast<Enemy*>(FindObject("Enemy"));
		e->KillMe();
	}

	static const float speed = 0.3f;
	XMFLOAT4 pos = Light::GetPosition(0);
	if (Input::IsKey(DIK_LEFTARROW)) pos.x += speed;
	if (Input::IsKey(DIK_RIGHTARROW)) pos.x -= speed;
	if (Input::IsKey(DIK_UPARROW)) pos.z += speed;
	if (Input::IsKey(DIK_DOWNARROW)) pos.z -= speed;
	if (Input::IsKey(DIK_E)) pos.y += speed;
	if (Input::IsKey(DIK_Q)) pos.y -= speed;
	Light::SetPosition(0, pos);

	//2画面の表示内容切り替え
	if (Input::IsKeyDown(DIK_R)) Direct3D::isTwoWindowShadowDraw_ = !Direct3D::isTwoWindowShadowDraw_;

}

//描画
void TestScene::Draw()
{
	RouteSearch::NodeModelDraw();

}

//開放
void TestScene::Release()
{
}
