#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
	int time_;		//Œo‰ßŽžŠÔ

public:
	TitleScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};