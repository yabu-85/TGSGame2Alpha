#include "Stage.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"

#include "CollisionMap.h"

namespace {
	int handle[5] = { -1 };

}

Stage::Stage(GameObject* parent)
	: GameObject(parent, "Stage"), hModel_(-1)
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	CollisionMap* cMap = Instantiate<CollisionMap>(this);
	cMap->CreatIntersectDataTriangle();

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