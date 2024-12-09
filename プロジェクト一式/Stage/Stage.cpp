#include "Stage.h"
#include "CollisionMap.h"
#include "SkyBox.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Light.h"
#include "../AI/RouteSearch.h"
#include "../Other/GameManager.h"
#include "../Other/VFXManager.h"
#include "../Scene/PlayScene.h"
#include "../Json/JsonReader.h"

Stage::Stage(GameObject* parent)
	: GameObject(parent, "Stage")
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	Instantiate<SkyBox>(this);
	CollisionMap* pCollisionMap = Instantiate<CollisionMap>(this);

	if (PlayScene::GetStageType() == STAGE_TYPE::STAGE_PLANE) {
		pCollisionMap->SetStageModelList("Json/PlaneStage.json");
		pCollisionMap->CreatIntersectDataTriangle();
		
		//RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/PlaneStage.json");
	}
	else if (PlayScene::GetStageType() == STAGE_TYPE::STAGE_1) {
		pCollisionMap->SetStageModelList("Json/Stage1.json");
		pCollisionMap->CreatIntersectDataTriangle();
		
		//RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/Stage1.json");
	}
	else if(PlayScene::GetStageType() == STAGE_TYPE::STAGE_2) {
		pCollisionMap->SetStageModelList("Json/Stage2.json");
		pCollisionMap->CreatIntersectDataTriangle();

		//RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/Stage2.json");
	}

}

void Stage::Update()
{
}

void Stage::Draw()
{
	if (rand() % 10 != 0) return;
	//Effekseer
	XMFLOAT3 lightPos = XMFLOAT3(Light::GetPosition(0).x, Light::GetPosition(0).y, Light::GetPosition(0).z);
	Transform pTrans;
	pTrans.position_ = lightPos;

	EFFEKSEERLIB::EFKTransform t;
	DirectX::XMStoreFloat4x4(&(t.matrix), pTrans.GetWorldMatrix());
	t.isLoop = false;   //繰り返し
	t.maxFrame = 80;    //80フレーム
	t.speed = 1.0;      //スピード
	EFFEKSEERLIB::gEfk->Play("FEATHER", t);

}

void Stage::Release()
{
}

void Stage::LoadStageEnvironment(std::string fileName)
{
	//設定を読み込む
	JsonReader::Load(fileName);
	auto& stageSection = JsonReader::GetSection("environment");
	stageEnvironment_ = StageEnvironment();

	stageEnvironment_.gravity = stageSection["gravity"];
	stageEnvironment_.startRotateY[0] = stageSection["startRotateY1"];
	stageEnvironment_.startRotateY[1] = stageSection["startRotateY2"];
	stageEnvironment_.startPosition[0] = { stageSection["startPosition1"]["x"], stageSection["startPosition1"]["y"], stageSection["startPosition1"]["z"] };
	stageEnvironment_.startPosition[1] = { stageSection["startPosition2"]["x"], stageSection["startPosition2"]["y"], stageSection["startPosition2"]["z"] };
	stageEnvironment_.lightPosition = { stageSection["lightPosition"]["x"], stageSection["lightPosition"]["y"], stageSection["lightPosition"]["z"] };
}
