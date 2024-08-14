#include "Stage.h"
#include "CollisionMap.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"
#include "../AI/RouteSearch.h"
#include "../Other/GameManager.h"

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
	//ÉÇÉfÉãéñëOì«Ç›çûÇ›
	Model::Load("Model/Scarecrow.fbx"); 
	
	Instantiate<CollisionMap>(this);

#if 0
	if (false) {
		GameManager::GetCollisionMap()->CreatIntersectDataTriangle("Json/Stage1.json");
		RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
	}
	else {
		GameManager::GetCollisionMap()->CreatIntersectDataTriangle("Json/Stage2.json");
		RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
	}

#else 
	GameManager::GetCollisionMap()->CreatIntersectDataTriangle("Json/PlaneStage.json");
	RouteSearch::LoadFileNode("Json/PlaneStageNode.json");
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