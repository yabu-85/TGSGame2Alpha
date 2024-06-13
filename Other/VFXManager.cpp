#include "VFXManager.h"
#include "../Engine/VFX.h"
#include "../Engine/Global.h"

namespace VFXManager
{
	//”š”­
	EmitterData sparks;
	EmitterData explode;

	//‰Œ
	EmitterData smoke;

}

void VFXManager::Initialize()
{
	//‰Î‚Ì•²
	sparks.textureFileName = "Particle/cloudA.png";
	sparks.delay = 0;
	sparks.number = 12;
	sparks.lifeTime = 50;
	sparks.directionRnd = XMFLOAT3(90.0f, 90.0f, 90.0f);
	sparks.speed = 0.1f;
	sparks.speedRnd = 1.0f;
	sparks.accel = 0.9f;
	sparks.size = XMFLOAT2(0.1f, 0.1f);
	sparks.sizeRnd = XMFLOAT2(0.1f, 0.1f);
	sparks.scale = XMFLOAT2(0.97f, 0.97f);
	sparks.color = XMFLOAT4(1.0f, 1.0f, 0.1f, 1.0f);
	sparks.deltaColor = XMFLOAT4(0.0f, 0.0f, 0.0f, -1.0f / sparks.lifeTime);
	sparks.gravity = 0.001f;

	//”š”­
	explode.textureFileName = "Particle/cloudA.png";
	explode.delay = 0;
	explode.number = 6;
	explode.lifeTime = 20;
	explode.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	explode.positionRnd = XMFLOAT3(0.5f, 0.0f, 0.5f);
	explode.direction = XMFLOAT3(1.0f, 1.0f, 1.0f);
	explode.directionRnd = XMFLOAT3(90.0f, 90.0f, 90.0f);
	explode.speed = 0.07f;
	explode.speedRnd = 0.8f;
	explode.size = XMFLOAT2(0.4f, 0.4f);
	explode.sizeRnd = XMFLOAT2(0.1f, 0.1f);
	explode.scale = XMFLOAT2(1.05f, 1.05f);
	explode.color = XMFLOAT4(1.0f, 1.0f, 0.1f, 1.0f);
	explode.deltaColor = XMFLOAT4(0.0f, -1.0f / 20.0f, 0.0f, -1.0f / 20.0f);
	explode.gravity = 0.001f;

	//‰Œ
	smoke.textureFileName = "Particle/cloudA.png";
	smoke.delay = 0;
	smoke.number = 6;
	smoke.lifeTime = 30;
	smoke.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	smoke.positionRnd = XMFLOAT3(0.2f, 0.1f, 0.2f);
	smoke.direction = XMFLOAT3(2.0f, 0.5f, 0.0f);
	smoke.directionRnd = XMFLOAT3(20.0f, 180.0f, 0.0f);
	smoke.speed = 0.0f;
	smoke.speedRnd = 0.0f;
	smoke.size = XMFLOAT2(1.3f, 1.3f);
	smoke.sizeRnd = XMFLOAT2(0.3f, 0.3f);
	smoke.scale = XMFLOAT2(1.02f, 1.02f);
	smoke.color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	smoke.deltaColor = XMFLOAT4(0.0f, 0.0f, 0.0f, -1.0f / smoke.lifeTime);
	smoke.gravity = 0.0f;
}

void VFXManager::CreateVfxExplode1(XMFLOAT3 pos)
{
	//‰Î‚Ì•²
	sparks.position = pos;
	VFX::Start(sparks);

	//”š”­
	explode.position = pos;
	VFX::Start(explode);
}

void VFXManager::CreateVfxSmoke(XMFLOAT3 pos)
{
	smoke.position = pos;
	VFX::Start(smoke);
}
