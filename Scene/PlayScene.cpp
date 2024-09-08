#include "PlayScene.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Weapon/GunBase.h"

#include "../Stage/Stage.h"
#include "../Screen/PauseScreen.h"
#include "../Screen/ResultScreen.h"

#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Light.h"
#include "../Engine/SceneManager.h"

#include "../AI/RouteSearch.h"
#include "../Enemy/EnemyManager.h"
#include "../UI/DamageUI.h"
#include "../UI/AimCursor.h"
#include "../UI/FixedHealthGauge.h"
#include "../UI/BulletInfoDisplay.h"
#include "../Other/GameManager.h"
#include "../Other/AudioManager.h"

STAGE_TYPE PlayScene::stageType_ = STAGE_TYPE::STAGE_PLANE;

namespace {
	const int PRE_STAGE_DRAW_TIME = 30;
	const int END_TIME_DEFAULT = 60;

}

//コンストラクタ
PlayScene::PlayScene(GameObject * parent)
	: SceneBase(parent, "PlayScene"), pAimCursor_{ nullptr, nullptr }, time_(0), endTime_(0), pPlayer_{ nullptr, nullptr }, preStageDraw_(true),
	preDrawCameraPos_(XMFLOAT3()), preDrawCameraTar_(XMFLOAT3()), isPause_(false), pBulletInfoDisplay_{ nullptr, nullptr }
{
}

//初期化
void PlayScene::Initialize()
{
	//画面分割無しに
	GameManager::SetOnePlayer();
	Camera::SetOneProjectionMatrix();

	GameManager::SetCursorMode(false);
	AudioManager::Initialize();
	endTime_ = END_TIME_DEFAULT;

	Stage* pStage = Instantiate<Stage>(this);
	pPlayer_[0] = Instantiate<Player>(this);
	pPlayer_[1] = Instantiate<Player>(this);

	pBulletInfoDisplay_[0] = Instantiate< BulletInfoDisplay>(this);
	pBulletInfoDisplay_[1] = Instantiate< BulletInfoDisplay>(this);

	//Stage情報読み込み
	if (pStage) {
		StageEnvironment stageEnvironment = pStage->GetStageEnvironment();
		for (int i = 0; i < 2; i++) {
			//座標
			pPlayer_[i]->SetPosition(stageEnvironment.startPosition[i]);

			//回転
			pPlayer_[i]->GetAim()->SetRotateY(stageEnvironment.startRotateY[i] - 180.0f);
			pPlayer_[i]->TargetRotate(Float3Add(transform_.position_, pPlayer_[i]->GetAim()->GetAimDirection()), 1.0f);
		}
		XMFLOAT4 lightPos = XMFLOAT4(stageEnvironment.lightPosition.x, stageEnvironment.lightPosition.y, stageEnvironment.lightPosition.z, 1.0f);
		Light::SetPosition(0, lightPos);
	}

	//自分より下のUpdateを拒否
	AllChildLeave();
	Enter();

	//色々初期化
	RouteSearch::Initialize();
	DamageUI::Initialize();
	EnemyManager::SetParent(this);
	AllDeleteScreen();

}

//更新
void PlayScene::Update()
{
	//ゲーム勝利判定
	if (GameManager::GetPlayer(0)->IsHealthZero() || GameManager::GetPlayer(1)->IsHealthZero()) {
		//勝負ついた瞬間のやつ
		if (endTime_ >= END_TIME_DEFAULT) {
			AllDeleteScreen();

			//ResultScreen作成と値のセット
			ResultScreen* screen = new ResultScreen();
			if (GameManager::GetPlayer(0)->IsHealthZero()) screen->SetWinPlayer(1);
			else if (GameManager::GetPlayer(1)->IsHealthZero()) screen->SetWinPlayer(0);
			AddScreen(screen);
			
			//自分より下のUpdateを拒否
			AllChildLeave();
			Enter();
	
			GameManager::SetCursorMode(true);
		}

		SceneBase::Update();

		endTime_--;
		if (pScreenList_.empty()) {
			SceneManager* pSceneManager = (SceneManager*)GameManager::GetRootObject()->FindObject("SceneManager");
			pSceneManager->ChangeScene(SCENE_ID_TITLE);
		}
		return;
	}

	//Pause画面呼び出し
	if (!isPause_ && IsPauseButtonDown()) {
		AddScreen(new PauseScreen());

		//自分より下のUpdateを拒否
		AllChildLeave();
		Enter();

		GameManager::SetCursorMode(true);
		isPause_ = true;
		return;
	}
	
	//Pause中の処理
	if (isPause_) {

		//PauseButton押した
		if (IsPauseButtonDown()) {
			AllDeleteScreen();
			GameManager::SetCursorMode(false);
			isPause_ = false;
			if (!preStageDraw_) AllChildEnter();
			return;
		}

		SceneBase::Update();
		
		//Pause終了の処理
		if (pScreenList_.empty()) {
			GameManager::SetCursorMode(false);
			isPause_ = false;
			if(!preStageDraw_) AllChildEnter();
		}
		//Pause中
		else {
			return;

		}
	}

	//ゲーム開始前のStage描画
	time_++;
	if (preStageDraw_) {
		if (time_ == PRE_STAGE_DRAW_TIME) {
			//Updateの許可
			AllChildEnter();

			GameManager::SetTwoPlayer();
			preStageDraw_ = false;
		}
		else {
			//カメラの移動
			static float CAMERA_X = 42.0f;
			CAMERA_X += 0.05f;
			Camera::SetPosition(XMFLOAT3(CAMERA_X, 15.0f, 55.0f), 0);
			Camera::SetTarget(XMFLOAT3(50.0f, 5.0f, 50.0f), 0);
		}
	}

#if 1
	//デバッグ用
	{
		if (Input::IsKeyDown(DIK_F1)) EnemyManager::SpawnEnemy(ENEMY_BOSS);
		if (Input::IsKeyDown(DIK_F2)) EnemyManager::SpawnEnemy(ENEMY_TEST);
		if (Input::IsKeyDown(DIK_F3)) EnemyManager::AllKillEnemy();
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
#endif

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
	SceneBase::CommonUIDraw();
	RouteSearch::NodeModelDraw();

}

void PlayScene::IndividualUIDraw(int index)
{
	SceneBase::IndividualUIDraw(index);
	
	//AimCursor
	if (!preStageDraw_ && !isPause_ && pAimCursor_[index]) pAimCursor_[index]->Draw();

	//HealthGauge
	if (!preStageDraw_ && pPlayer_[index]) {
		float r = (float)pPlayer_[index]->GetHP() / (float)pPlayer_[index]->GetMaxHP();
		pPlayer_[index]->GetFixedHealthGauge()->SetParcent(r);
		pPlayer_[index]->GetFixedHealthGauge()->Draw(GameManager::GetDrawIndex());
	
		pBulletInfoDisplay_[index]->SetCurrentMagazine(pPlayer_[index]->GetGun()->GetCurrentMagazineCount());
		pBulletInfoDisplay_[index]->SetMaxMagazine(pPlayer_[index]->GetGun()->GetMaxMagazineCount());
		pBulletInfoDisplay_[index]->DrawBullet();
	}

}

bool PlayScene::IsPauseButtonDown()
{
	return (Input::IsKeyDown(DIK_TAB) || Input::IsPadButtonDown(XINPUT_GAMEPAD_START, 0) || Input::IsPadButtonDown(XINPUT_GAMEPAD_BACK, 0));
}