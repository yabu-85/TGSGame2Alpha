#include "TestScene.h"
#include "Player/Player.h"
#include "Stage/Stage.h"

#include "Engine/Input.h"
#include "Engine/Light.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene")
{
}

//初期化
void TestScene::Initialize()
{
	Instantiate<Player>(this);
	Instantiate<Stage>(this);

}

//更新
void TestScene::Update()
{
	XMFLOAT4 pos = Light::GetPosition(0);
	if (Input::IsKey(DIK_LEFTARROW)) pos.x += 0.1f;
	if (Input::IsKey(DIK_RIGHTARROW)) pos.x -= 0.1f;
	if (Input::IsKey(DIK_UPARROW)) pos.z += 0.1f;
	if (Input::IsKey(DIK_DOWNARROW)) pos.z -= 0.1f;
	Light::SetPosition(0, pos);

}

//描画
void TestScene::Draw()
{
	Light::Draw();

}

//開放
void TestScene::Release()
{
}
