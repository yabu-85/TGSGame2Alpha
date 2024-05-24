#include "Stage.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"

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

	for (int i = 0; i < 4; i++) {
		handle[i] = Model::Load("Model/toras.fbx");
		assert(handle[i] >= 0);
	}

	handle[4] = Model::Load("Model/testSph.fbx");
	assert(handle[4] >= 0);
	

}

void Stage::Update()
{
}

void Stage::Draw()
{
	//Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
	//Direct3D::SetShader(Direct3D::SHADER_UNLIT);

	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
	
	Transform trans;
#if 0
	trans.position_ = XMFLOAT3(0, 2, 0);
	Model::SetTransform(handle[0], trans);
	Model::Draw(handle[0]);

	trans.position_ = XMFLOAT3(2, 1, 0);
	trans.rotate_.x = 30;
	trans.rotate_.y = 30;
	Model::SetTransform(handle[0], trans);
	Model::Draw(handle[0]);

	trans.position_ = XMFLOAT3(0, 6, 0);
	trans.scale_ = XMFLOAT3(1.3f, 1.3f, 1.3f);
	Model::SetTransform(handle[0], trans);
	Model::Draw(handle[0]);
#endif

	//Direct3D::SetShader(type);

}

void Stage::Release()
{
}