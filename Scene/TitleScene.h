#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
	int time_;		//�o�ߎ���

public:
	TitleScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};