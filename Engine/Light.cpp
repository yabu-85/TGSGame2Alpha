#include "Light.h"
#include "Model.h"
#include "Direct3D.h"

#define DRAW 1;

struct LightData {
	XMFLOAT4 position;
	XMFLOAT4 target;
	LightData() : position(XMFLOAT4()), target(XMFLOAT4()) {}
};

namespace Light {
	LightData lightDatas[LIGHT_TOTAL_NUM];
	int hModel = -1;
	
}

void Light::Initialize()
{
#if DRAW
	hModel = Model::Load("DebugCollision/SphereCollider.fbx");
	assert(hModel >= 0);
#endif

	lightDatas[0].position = XMFLOAT4(10.0f, 10.0f, 0.0f, 0.0f);
}

void Light::Draw()
{

#if DRAW
	Transform trans;
	trans.position_ = XMFLOAT3(lightDatas[0].position.x, lightDatas[0].position.y, lightDatas[0].position.z);
	Model::SetTransform(hModel, trans);
	Model::Draw(hModel);
#endif

}

void Light::SetPosition(int i, XMFLOAT4 pos)
{
	lightDatas[i].position = pos;
}

void Light::SetTarget(int i, XMFLOAT4 tar)
{
	lightDatas[i].target = tar;
}

XMFLOAT4 Light::GetPosition(int i)
{
	return lightDatas[i].position;
}

XMFLOAT4 Light::GetTarget(int i)
{
	return lightDatas[i].target;
}