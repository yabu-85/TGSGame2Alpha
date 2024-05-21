#include "Light.h"
#include "Model.h"

struct LightData {
	XMFLOAT4 posision;
	XMFLOAT4 target;
	LightData() : posision(XMFLOAT4()), target(XMFLOAT4()) {}
};

namespace Light {
	LightData lightDatas[LIGHT_TOTAL_NUM];
	int hModel = -1;
}

void Light::Initialize()
{
#if 1
	hModel = Model::Load("DebugCollision/SphereCollider.fbx");
	assert(hModel >= 0);
#endif

	lightDatas[0] = LightData();
	lightDatas[0].posision = XMFLOAT4(10.0f, 10.0f, 0.0f, 0.0f);
	lightDatas[0].target = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

void Light::Draw()
{
#if 1
	Transform trans;
	trans.position_ = XMFLOAT3(lightDatas[0].posision.x, lightDatas[0].posision.y, lightDatas[0].posision.z);
	Model::SetTransform(hModel, trans);
	Model::Draw(hModel);
#endif

}

void Light::SetPosition(int i, XMFLOAT4 pos)
{
	lightDatas[0].posision = pos;
}

XMFLOAT4 Light::GetPosition(int i)
{
	return lightDatas[0].posision;
}

XMFLOAT4 Light::GetTarget(int i)
{
	return lightDatas[0].target;
}
