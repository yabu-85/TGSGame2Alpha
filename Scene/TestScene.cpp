#include "TestScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Screen/PlayScreen.h"
#include "../Screen/PauseScreen.h"

#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Light.h"
#include "../Engine/SceneManager.h"
#include "../AI/RouteSearch.h"

#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/TestEnemy.h"
#include "../UI/DamageUI.h"
#include "../UI/AimCursor.h"
#include "../Other/GameManager.h"

//コンストラクタ
TestScene::TestScene(GameObject * parent)
	: SceneBase(parent, "TestScene"), pAimCursor_{nullptr, nullptr}
{
}

//初期化
void TestScene::Initialize()
{
	GameManager::SetTwoPlayer();
	
	Instantiate<Stage>(this);
	Instantiate<Player>(this);

	//2人プレイヤーなら
	if (!GameManager::IsOnePlayer()) Instantiate<Player>(this);

	RouteSearch::Initialize();
	DamageUI::Initialize();
	EnemyManager::SetParent(this);
	AllDeleteScreen();

	//AddScreen(new PlayScreen());
	AddScreen(new PauseScreen());

	//モデル事前読み込み
	Model::Load("Model/Scarecrow.fbx");

}

//更新
void TestScene::Update()
{
	if (Input::IsKeyDown(DIK_N)) {
		static_cast<SceneManager*>(FindObject("SceneManager"))->ChangeScene(SCENE_ID_SELECT);
		return;
	}

	SceneBase::Update();

	if (Input::IsKeyDown(DIK_F1)) EnemyManager::SpawnEnemy(ENEMY_TEST);
	if (Input::IsKeyDown(DIK_F2)) for(int i = 0; i < 10; i++) EnemyManager::SpawnEnemy(ENEMY_TEST);
	if (Input::IsKeyDown(DIK_F3)) EnemyManager::KillEnemy(static_cast<TestEnemy*>(FindObject("TestEnemy")));
	if (Input::IsKeyDown(DIK_F4)) {
		OutputDebugStringA(std::to_string(EnemyManager::GetAllEnemy().size()).c_str());
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
}

//開放
void TestScene::Release()
{
}

void TestScene::CommonUIDraw()
{
	SceneBase::Draw();
	RouteSearch::NodeModelDraw();

}

void TestScene::IndividualUIDraw(int index)
{
	pAimCursor_[index]->Draw();

}
