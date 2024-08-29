#include "PlayScene.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Weapon/GunBase.h"

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

STAGE_TYPE PlayScene::stageType_ = STAGE_PLANE;

//コンストラクタ
PlayScene::PlayScene(GameObject * parent)
	: SceneBase(parent, "PlayScene"), pAimCursor_{ nullptr, nullptr }, time_(0), pPlayer_{ nullptr, nullptr }
{
}

//初期化
void PlayScene::Initialize()
{
	//画面分割無しに
	GameManager::SetOnePlayer();
	Camera::SetOneProjectionMatrix();

	Stage* pStage = Instantiate<Stage>(this);
	pPlayer_[0] = Instantiate<Player>(this);
	pPlayer_[1] = Instantiate<Player>(this);

	//Stage情報読み込み
	if (pStage) {
		StageEnvironment stageEnvironment = pStage->GetStageEnvironment();
		for (int i = 0; i < 2; i++) {
			pPlayer_[i]->SetPosition(stageEnvironment.startPosition[i]);
			pPlayer_[i]->SetRotateY(stageEnvironment.startRotateY[i]);
		}
		XMFLOAT4 lightPos = XMFLOAT4(stageEnvironment.lightPosition.x, stageEnvironment.lightPosition.y, stageEnvironment.lightPosition.z, 1.0f);
		Light::SetPosition(0, lightPos);
	}

	for (int i = 0; i < 2; i++) {
		pPlayer_[i]->Leave();
		pPlayer_[i]->GetAim()->Leave();
		//pPlayer_[i]->GetGun()->Leave();
	}

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
	if (time_ == 3) {
		pPlayer_[0]->Enter();
		pPlayer_[1]->Enter();
		pPlayer_[0]->GetAim()->Enter();
		pPlayer_[1]->GetAim()->Enter();
		GameManager::SetTwoPlayer();
	}
	else {
		static float CAMERA_X = 42.0f;
		CAMERA_X += 0.05f;
		Camera::SetPosition(XMFLOAT3(CAMERA_X, 15.0f, 55.0f), 0);
		Camera::SetTarget(XMFLOAT3(50.0f, 5.0f, 50.0f), 0);

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
