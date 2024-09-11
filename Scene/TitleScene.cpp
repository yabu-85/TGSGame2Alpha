#include "TitleScene.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include "../Other/GameManager.h"
#include "../Other/AudioManager.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "../Engine/Light.h"
#include "../Screen/TitleScreen.h"
#include "../Stage/CollisionMap.h"
#include "../Stage/SkyBox.h"
#include "../Json/JsonReader.h"

//コンストラクタ
TitleScene::TitleScene(GameObject * parent)
	: SceneBase(parent, "TitleScene"), time_(0), stageModel_(0), lightPos_{ XMFLOAT3(), XMFLOAT3() }
{
}

//初期化
void TitleScene::Initialize()
{
	GameManager::SetOnePlayer();
	GameManager::SetCursorMode(true);
	AudioManager::Initialize();

	AllDeleteScreen();
	AddScreen(new TitleScreen());

	Camera::SetPosition(XMFLOAT3(60.0f, 15.0f, 40.0f), 0);
	Camera::SetTarget(XMFLOAT3(50.0f, 0.0f, 50.0f), 0);
	Camera::Update(0);

	//最初はStage１に設定
	Instantiate<SkyBox>(this);
	CollisionMap* pCollisionMap = Instantiate<CollisionMap>(this);
	pCollisionMap->SetStageModelList("Json/Stage1.json");
	stageModel_ = 1;

	//JSONファイル読み込み
	JsonReader::Load("Json/Stage1.json");
	auto& stage1 = JsonReader::GetSection("environment");
	lightPos_[0] = { stage1["lightPosition"]["x"], stage1["lightPosition"]["y"], stage1["lightPosition"]["z"] };

	JsonReader::Load("Json/Stage2.json");
	auto& stage2 = JsonReader::GetSection("environment");
	lightPos_[1] = { stage2["lightPosition"]["x"], stage2["lightPosition"]["y"], stage2["lightPosition"]["z"] };

	//LightSet
	Light::SetPosition(0, XMFLOAT4(lightPos_[0].x, lightPos_[0].y, lightPos_[0].z, 1.0f));

}

//更新
void TitleScene::Update()
{
	time_++;
	
	//ちょっと待ってからUpdate
	if(time_ >= 10) SceneBase::Update();

	//モデルの入れ替え
	if (time_ % 200 == 0) {
		if (stageModel_ == 1) {
			GameManager::GetCollisionMap()->SetStageModelList("Json/Stage2.json");
			Light::SetPosition(0, XMFLOAT4(lightPos_[1].x, lightPos_[1].y, lightPos_[1].z, 1.0f));
			stageModel_ = 2;
		}
		else if (stageModel_ == 2) {
			GameManager::GetCollisionMap()->SetStageModelList("Json/Stage1.json");
			Light::SetPosition(0, XMFLOAT4(lightPos_[0].x, lightPos_[0].y, lightPos_[0].z, 1.0f));
			stageModel_ = 1;
		}
	}

	//カメラの回転
	static const float valueZ = 0.005f;
	static const float CameraCenter = 50.0f;
	static const float cameraPosX = 15.0f;
	static float value = 0.0f;
	
	value += valueZ;
	float CamX = CameraCenter + cameraPosX * std::cosf(value);
	float CamZ = CameraCenter + cameraPosX * std::sinf(value);
	Camera::SetPosition(XMFLOAT3(CamX, 15.0f, CamZ), 0);


	//デバッグ用
#if 1
	{
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
void TitleScene::Draw()
{

}

//開放
void TitleScene::Release()
{

}
