#include "Stage.h"
#include "CollisionMap.h"
#include "SkyBox.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"
#include "../AI/RouteSearch.h"
#include "../Other/GameManager.h"
#include "../Scene/PlayScene.h"
#include "../Json/JsonReader.h"

namespace {

}

Stage::Stage(GameObject* parent)
	: GameObject(parent, "Stage")
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	//Instantiate<SkyBox>(this);
	CollisionMap* pCollisionMap = Instantiate<CollisionMap>(this);

	if (PlayScene::GetStageType() == STAGE_TYPE::STAGE_PLANE) {
		pCollisionMap->SetStageModelList("Json/PlaneStage.json");
		pCollisionMap->CreatIntersectDataTriangle();
		
		RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/PlaneStage.json");
	}
	else if (PlayScene::GetStageType() == STAGE_TYPE::STAGE_1) {
		pCollisionMap->SetStageModelList("Json/Stage1.json");
		pCollisionMap->CreatIntersectDataTriangle();
		
		RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/Stage1.json");
	}
	else if(PlayScene::GetStageType() == STAGE_TYPE::STAGE_2) {
		pCollisionMap->SetStageModelList("Json/Stage2.json");
		pCollisionMap->CreatIntersectDataTriangle();

		RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/Stage2.json");
	}

}

void Stage::Update()
{
}

void Stage::Draw()
{
}

void Stage::Release()
{
}

void Stage::LoadStageEnvironment(std::string fileName)
{
	//ê›íËÇì«Ç›çûÇﬁ
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
