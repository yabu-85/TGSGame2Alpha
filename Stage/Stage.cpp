#include "Stage.h"
#include "../Engine/Model.h"

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
	hModel_ = Model::Load("Model/planeStage.fbx");
	assert(hModel_ >= 0);

	for (int i = 0; i < 5; i++) {
		handle[i] = Model::Load("Model/toras.fbx");
		assert(handle[i] >= 0);
	}
	

}

void Stage::Update()
{
}

void Stage::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);

	Transform trans;
	trans.position_ = XMFLOAT3(0, 2, 0);
	Model::SetTransform(handle[0], trans);
	Model::Draw(handle[0]);

	trans.position_ = XMFLOAT3(2, 1, 0);
	trans.rotate_.x = 30;
	trans.rotate_.y = 30;
	Model::SetTransform(handle[0], trans);
	Model::Draw(handle[0]);

}

void Stage::Release()
{
}