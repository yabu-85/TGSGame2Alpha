#include "TestScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../UI/ScreenManager.h"
#include "../UI/PauseScreen.h"

#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Light.h"
#include "../Engine/Direct3D.h"
#include "../AI/RouteSearch.h"

#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/TestEnemy.h"
#include "../UI/DamageUI.h"

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

	RouteSearch::Initialize();
	DamageUI::Initialize();
	EnemyManager::SetParent(this);
	
	ScreenManager::AllDeleteScreen();
	ScreenManager::AddScreen(new PauseScreen());

	//モデル事前読み込み
	Model::Load("Model/Scarecrow.fbx");

}

//更新
void TestScene::Update()
{
	if (Input::IsKeyDown(DIK_F1)) EnemyManager::SpawnEnemy(ENEMY_TEST);
	if (Input::IsKeyDown(DIK_F2)) EnemyManager::KillEnemy(static_cast<TestEnemy*>(FindObject("TestEnemy")));
	if (Input::IsKeyDown(DIK_F3)) {
		OutputDebugStringA(std::to_string(EnemyManager::GetAllEnemy().size()).c_str());
		OutputDebugString("\n");
	}

	if (Input::IsKeyDown(DIK_F3)) 
		ScreenManager::AllDeleteScreen();
	
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
