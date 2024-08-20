#include "PlayScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Screen/PlayScreen.h"
#include "../Screen/SettingScreen.h"

#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Light.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Light.h"

#include "../AI/RouteSearch.h"
#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/TestEnemy.h"
#include "../UI/DamageUI.h"
#include "../UI/AimCursor.h"
#include "../Other/GameManager.h"

//コンストラクタ
PlayScene::PlayScene(GameObject * parent)
	: SceneBase(parent, "PlayScene"), pAimCursor_{nullptr, nullptr},
	time_(0)
{
}

//最初は画面分割はなしで
//ステージ見せた後に分割する

//初期化
void PlayScene::Initialize()
{
	//画面分割無しに
	GameManager::SetOnePlayer();
	Camera::SetOneProjectionMatrix();

	Instantiate<Stage>(this);
	Instantiate<Player>(this);
	Instantiate<Player>(this);

	RouteSearch::Initialize();
	DamageUI::Initialize();
	EnemyManager::SetParent(this);

	AllDeleteScreen();
	AddScreen(new SettingScreen());

}

//更新
void PlayScene::Update()
{
	time_++;
	if (time_ >= 10) {
		GameManager::SetTwoPlayer();

	}

	SceneBase::Update();
	
	//デバッグ用
	{
		if (Input::IsKeyDown(DIK_F1)) EnemyManager::SpawnEnemy(ENEMY_BOSS);
		if (Input::IsKeyDown(DIK_F2)) EnemyManager::SpawnEnemy(ENEMY_TEST);
		if (Input::IsKeyDown(DIK_F3)) {
			EnemyManager::KillEnemy(static_cast<TestEnemy*>(FindObject("TestEnemy")));
			EnemyManager::KillEnemy(static_cast<TestEnemy*>(FindObject("TestBossEnemy")));
		}
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

}

//描画
void PlayScene::Draw()
{
}

//開放
void PlayScene::Release()
{
}

void PlayScene::CommonUIDraw()
{
	SceneBase::Draw();
	RouteSearch::NodeModelDraw();

}

void PlayScene::IndividualUIDraw(int index)
{
	if (pAimCursor_[index]) pAimCursor_[index]->Draw();

}
