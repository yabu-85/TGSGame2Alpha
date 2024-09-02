#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
	int stageModel_;	//•\¦‚·‚éStage‚Ì”Ô†
	int time_;			//Œo‰ßŠÔ

public:
	TitleScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};