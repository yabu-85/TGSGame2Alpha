#include "SkyBox.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"

SkyBox::SkyBox(GameObject* parent)
	:GameObject(parent, "SkyBos"), hModel_(-1)
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Initialize()
{
	hModel_ = Model::Load("Model/Stage/SkyBox.fbx");
	assert(hModel_ >= 0);

	//‰e–³‚µ
	Model::SetShadow(hModel_, false);

	//Max950‚­‚ç‚¢
	float size = 900.0f;
	transform_.scale_ = { size, size, size };
	transform_.position_ = XMFLOAT3(50.0f, 0.0f, 50.0f);

}

void SkyBox::Update()
{
}

void SkyBox::Draw()
{
	if (Direct3D::GetCurrentShader() != Direct3D::SHADER_SHADOWMAP) {
		Direct3D::SetShader(Direct3D::SHADER_SKYBOX);
		
		transform_.rotate_.y += 0.005f;
		Model::SetTransform(hModel_, transform_);
		Model::Draw(hModel_);

		Direct3D::SetShader(Direct3D::SHADER_3D);
	}

}

void SkyBox::Release()
{
}
