#include "TestScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Enemy/Enemy.h"

#include "../Engine/Model.h"
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

	Model::Load("Model/Bullet.fbx");

}

//更新
void TestScene::Update()
{
	if (Input::IsKey(DIK_F1)) Instantiate<Enemy>(this);
	if (Input::IsKeyDown(DIK_F2)) {
		Enemy* e = static_cast<Enemy*>(FindObject("Enemy"));
		if(e) e->KillMe();
	}
	if (Input::IsKeyDown(DIK_F3)) {
		OutputDebugStringA(std::to_string(enemyList_.size()).c_str());
		OutputDebugString("\n");
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
