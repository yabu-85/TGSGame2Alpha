#include "Light.h"
#include "Model.h"
#include "Direct3D.h"

struct LightData {
	XMFLOAT4 posision;
	XMFLOAT4 target;
	LightData() : posision(XMFLOAT4()), target(XMFLOAT4()) {}
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
	lightDatas[0].position = XMFLOAT4(10.0f, 10.0f, 0.0f, 0.0f);
}

void Light::Draw()
{
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