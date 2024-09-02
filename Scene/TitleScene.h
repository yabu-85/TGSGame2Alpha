#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
	int stageModel_;	//表示するStageの番号
	int time_;			//経過時間

public:
	TitleScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};