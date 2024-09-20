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
	const int PRE_STAGE_DRAW_TIME = 0;
	const int END_TIME_DEFAULT = 60;

	const XMFLOAT3 PRE_CAMERA_POSITION = XMFLOAT3(42.0f, 15.0f, 55.0f);
	const XMFLOAT3 PRE_CAMERA_TARGET = XMFLOAT3(50.0f, 0.0f, 50.0f);
	const float PRE_CAMERA_POS_MOVE_X = 0.05f;

}

//�R���X�g���N�^
PlayScene::PlayScene(GameObject * parent)
	: SceneBase(parent, "PlayScene"), pAimCursor_{ nullptr, nullptr }, time_(0), endTime_(0), pPlayer_{ nullptr, nullptr }, preStageDraw_(true),
	preDrawCameraPos_(XMFLOAT3()), preDrawCameraTar_(XMFLOAT3()), isPause_(false), pBulletInfoDisplay_{ nullptr, nullptr }
{
}

//������
void PlayScene::Initialize()
{
	//��ʕ���������
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

	//Stage���ǂݍ���
	if (pStage) {
		StageEnvironment stageEnvironment = pStage->GetStageEnvironment();
		for (int i = 0; i < 2; i++) {
			//���W
			pPlayer_[i]->SetPosition(stageEnvironment.startPosition[i]);

			//��]
			pPlayer_[i]->GetAim()->SetRotateY(stageEnvironment.startRotateY[i] - 180.0f);
			pPlayer_[i]->TargetRotate(Float3Add(transform_.position_, pPlayer_[i]->GetAim()->GetAimDirection()), 1.0f);
		}
		XMFLOAT4 lightPos = XMFLOAT4(stageEnvironment.lightPosition.x, stageEnvironment.lightPosition.y, stageEnvironment.lightPosition.z, 1.0f);
		Light::SetPosition(0, lightPos);
	}

	preDrawCameraPos_ = PRE_CAMERA_POSITION;
	preDrawCameraTar_ = PRE_CAMERA_TARGET;

	//������艺��Update������
	AllChildLeave();
	Enter();

	//�F�X������
	RouteSearch::Initialize();
	DamageUI::Initialize();
	EnemyManager::SetParent(this);
	AllDeleteScreen();

}

//�X�V
void PlayScene::Update()
{
	//�Q�[����������
	if (GameManager::GetPlayer(0)->IsHealthZero() || GameManager::GetPlayer(1)->IsHealthZero()) {
		//���������u�Ԃ̂��
		if (endTime_ >= END_TIME_DEFAULT) {
			AllDeleteScreen();

			//ResultScreen�쐬�ƒl�̃Z�b�g
			ResultScreen* screen = new ResultScreen();
			if (GameManager::GetPlayer(0)->IsHealthZero()) screen->SetWinPlayer(1);
			else if (GameManager::GetPlayer(1)->IsHealthZero()) screen->SetWinPlayer(0);
			AddScreen(screen);
			
			//������艺��Update������
			AllChildLeave();
			Enter();
	
			//Player�̏���
			for (int i = 0; i < 2; i++) {
				if (pPlayer_[i]->GetAim()->IsAimFps()) {
					//Player��Update����
					pPlayer_[i]->Enter();
					pPlayer_[i]->GetAim()->Enter();

					//Tps�ݒ��
					pPlayer_[i]->GetAim()->SetAimFps(false);
					pPlayer_[i]->GetAim()->SetAimMove(false);

					pPlayer_[i]->GetAim()->SetDistanceBehind(0.0f);
					pPlayer_[i]->GetAim()->SetDistanceHeight(0.0f);
					pPlayer_[i]->GetAim()->SetDistanceHorizontal(0.0f);
					pPlayer_[i]->GetAim()->SetDistanceIncreaseAmount(0.05f);
				}
			}

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

	//Pause��ʌĂяo��
	if (!isPause_ && IsPauseButtonDown()) {
		AddScreen(new PauseScreen());

		//������艺��Update������
		AllChildLeave();
		Enter();

		GameManager::SetCursorMode(true);
		isPause_ = true;
		return;
	}
	
	//Pause���̏���
	if (isPause_) {

		//PauseButton������
		if (IsPauseButtonDown()) {
			AllDeleteScreen();
			GameManager::SetCursorMode(false);
			isPause_ = false;
			if (!preStageDraw_) AllChildEnter();
			return;
		}

		SceneBase::Update();
		
		//Pause�I���̏���
		if (pScreenList_.empty()) {
			GameManager::SetCursorMode(false);
			isPause_ = false;
			if(!preStageDraw_) AllChildEnter();
		}
		//Pause��
		else {
			return;
		}
	}

	//�Q�[���J�n�O��Stage�`��
	time_++;
	if (preStageDraw_) {
		if (time_ >= PRE_STAGE_DRAW_TIME) {
			//Update�̋���
			AllChildEnter();

			GameManager::SetTwoPlayer();
			preStageDraw_ = false;
		}
		else {
			//�J�����̈ړ�
			preDrawCameraPos_.x += PRE_CAMERA_POS_MOVE_X;
			Camera::SetPosition(preDrawCameraPos_, 0);
			Camera::SetTarget(preDrawCameraTar_, 0);
		}
	}

	//�f�o�b�O�p
#if 1
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

//�`��
void PlayScene::Draw()
{
}

//�J��
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

	//StageDraw���Ȃ�I���
	if (preStageDraw_) return;

	//�|�[�Y��ʂ���Ȃ���
	if (!isPause_) {
		//AimCursor
		if (pAimCursor_[index]) pAimCursor_[index]->Draw();
	}

	//Player�֌W
	if (pPlayer_[index]) {
		//HealthGauge
		float r = (float)pPlayer_[index]->GetHP() / (float)pPlayer_[index]->GetMaxHP();
		pPlayer_[index]->GetFixedHealthGauge()->SetParcent(r);
		pPlayer_[index]->GetFixedHealthGauge()->Draw(GameManager::GetDrawIndex());
	
		//�e�̏��
		if (pPlayer_[index]->GetGun()) {
			pBulletInfoDisplay_[index]->SetCurrentMagazine(pPlayer_[index]->GetGun()->GetCurrentMagazineCount());
			pBulletInfoDisplay_[index]->SetMaxMagazine(pPlayer_[index]->GetGun()->GetMaxMagazineCount());
		}
	}
	
	pBulletInfoDisplay_[index]->DrawBullet();

}

bool PlayScene::IsPauseButtonDown()
{
	return (Input::IsKeyDown(DIK_TAB) || Input::IsPadButtonDown(XINPUT_GAMEPAD_START, 0) || Input::IsPadButtonDown(XINPUT_GAMEPAD_BACK, 0));
}