#include "AnimationNotify.h"
#include "../Other/VFXManager.h"
#include "../Other/AudioManager.h"
#include "../Engine/GameObject.h"
#include "../Engine/Model.h"

void OneFrame::CalcFrame(int t, GameObject* obj) {
	if (time == t) OnFrame(obj); 
}

void DoubleFrame::CalcFrame(int t, GameObject* obj) {
	if (time <= t && endTime >= t) {
		OnFrame(obj);
		if (time == t) OnEnter(obj);
		if (endTime == t) OnExit(obj);
	}
}

void CreatFrame::CalcFrame(int t, GameObject* obj) {
	if (time == t) VFXManager::CreateVfx(obj->GetPosition(), type);
}

void PlaySoundFrame::CalcFrame(int t, GameObject* obj) {
}

CreatFrameBone::CreatFrameBone(int t, VFX_TYPE _type, int model, std::string name) : CreatFrame(t, _type), hModel(model)
{
	Model::GetPartBoneIndex(hModel, name, &partIndex, &boneIndex);
	assert(boneIndex >= 0);
}

void CreatFrameBone::CalcFrame(int t, GameObject* obj)
{
	XMFLOAT3 bonePos = Model::GetBoneAnimPosition(hModel, boneIndex, partIndex);
	//if (time == t) VFXManager::CreateVfx(bonePos, type);
}
