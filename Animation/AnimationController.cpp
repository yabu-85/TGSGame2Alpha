#include "AnimationController.h"
#include "../Engine/Model.h"
#include "AnimationNotify.h"

AnimationController::AnimationController(int number, GameObject* obj)
	: modelNum_(number), currentAnim_(-1), pGameObject_(obj)
{
}

AnimationController::~AnimationController()
{
	for (int i = 0; i < animDatas_.size(); i++) {
		for (OneFrame* frame : animDatas_[i].frameList) delete frame;
	}
}

void AnimationController::Update()
{
	if (currentAnim_ != -1) {
		int animTime = (int)Model::GetAnimFrame(modelNum_);
		for (OneFrame* action : animDatas_[currentAnim_].frameList) {
			action->CalcFrame(animTime, pGameObject_);
		}
	}

}

void AnimationController::AddAnim(int s, int e)
{
	AnimData data;
	data.startFrame = s;
	data.endFrame = e;
	animDatas_.push_back(data);
}

void AnimationController::AddAnimNotify(int AnimId, OneFrame* action)
{
	animDatas_[AnimId].frameList.push_back(action);
}

AnimData AnimationController::GetAnim(int id)
{
	return animDatas_[id];
}

int AnimationController::GetAnimTime(int id)
{
	return  animDatas_[id].endFrame - animDatas_[id].startFrame;
}

void AnimationController::SetNextAnim(int id, float speed, int addStart, int addEnd)
{
	currentAnim_ = id;
	Model::SetAnimFrame(modelNum_, animDatas_.at(id).startFrame + addStart, animDatas_.at(id).endFrame + addEnd, speed);
}