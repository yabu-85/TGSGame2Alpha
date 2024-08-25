#include "Stage.h"
#include "CollisionMap.h"
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
	CollisionMap* pCollisionMap = Instantiate<CollisionMap>(this);
	PlayScene* pPlayScene = static_cast<PlayScene*>(GetParent());

#if 1
	if (true) {
		pCollisionMap->CreatIntersectDataTriangle("Json/Stage1.json");
		RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/Stage1.json");
	}
	else {
		pCollisionMap->CreatIntersectDataTriangle("Json/Stage2.json");
		RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
		LoadStageEnvironment("Json/Stage2.json");
	}

#else 
	pCollisionMap->CreatIntersectDataTriangle("Json/PlaneStage.json");
	RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
	LoadStageEnvironment("Json/PlaneStage.json");
#endif

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
